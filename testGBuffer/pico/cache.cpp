/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "cache.hpp"
#include "vector.hpp"
#include "cacheentry.hpp"
#include "log.hpp"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* inlines */

inline void Cache::setFileFlags(int inode, bool isDir) {
    unsigned char flags = (isDir ? kFlagIsDir : 0);
    inodeFile.pwriteFast(&flags, 1, inode * kInodeSize + 7);
}

inline bool Cache::isBlockCompressed(int inode, int block) {
    return (block < 0) ? params.compressIndirect : 
        (inode == kMasterInode) ? params.compressInode : 
        params.compressData;
}

inline int Cache::getBlockShift(int inode, int block) {
    return (block < 0) ? params.indirectBlockShift :
        (inode == kMasterInode) ? params.inodeBlockShift : 
        params.dataBlockShift;
}

Cache::Cache(OsFile iniFile, int cacheSize) :
    file(iniFile),
    freeSpace(iniFile.getSize()),
    superblockDirty(false),
    kNIndirect(0),
    kNDoubleIndirect(0),
    diskBufSize(64),
    diskBuf((char*)malloc(diskBufSize)),
    inodeFile(this, kMasterInode),
    params(iniFile),
    lru(cacheSize),
    isInsideFlush(false),
    refCount(0),
    nInodes(0)
{
    assert(diskBuf);

    int nRead = file.pread(superblock, kSuperblockSize, kSuperblockPos);
    assert(nRead == kSuperblockSize);
    (void)nRead;

    kNIndirect        = (1 << (params.indirectBlockShift - 3));
    kNDoubleIndirect  = kNIndirect * kNIndirect;

   	//======Modified by hyc 20100613.
//    computeFreeSpace();
	lastCommitedFileSize = file.getSize();
	//==========
    //freeSpace.printFreeSpace();
    int64 inodeFileSize = getFileSize(kMasterInode);
    assert((inodeFileSize & (kInodeSize - 1)) == 0);
    nInodes = (inodeFileSize >> kInodeSizeShift);
    assert(freeInodes.empty() ||
           (freeInodes.back() + 1 < nInodes));
}

Cache::~Cache() {
    assert(lastCommitedFileSize <= file.getSize());
    if (lastCommitedFileSize < file.getSize()) {
        file.setSize(lastCommitedFileSize);
    }
    file.close();
    assert(!file);
    free(diskBuf);
    diskBuf = 0;
    diskBufSize = 0;
    kNIndirect = kNDoubleIndirect = 0;
}

void Cache::initFileHeader(OsFile file,
                           const StorageParams &params) {
    params.write(file);
    unsigned char superblock[kSuperblockSize];
    memset(superblock, 0, sizeof(superblock));
    hton5(kInodeSize, superblock); //inode-file size
    file.pwrite(superblock, sizeof(superblock), kSuperblockPos);
}

bool Cache::commit() {
	//hyc,20111116
	if (file.readOnly())
	{
		return false;
	}
    //printf("hash factor %f\n", computeHashConflictFactor());
    //assert((nInodes << kInodeSizeShift) == getFileSize(kMasterInode));
    if ((nInodes << kInodeSizeShift) != getFileSize(kMasterInode)) {
        setFileSize(kMasterInode, (nInodes << kInodeSizeShift));
    }

    flushAll();
    lru.assertNoneDirty();
    if (superblockDirty) {
        LOG(("write superblock"));
        if (params.safeCommit) { 
            file.flush();
            file.pwrite(superblock, kSuperblockSize, kSuperblockPos);
            file.flush();
        } else {
            file.pwrite(superblock, kSuperblockSize, kSuperblockPos);
        }
    }

    for (Slice *it = freeOnCommit.begin(), *end = freeOnCommit.end();
         it != end; ++it) {
        freeSpace.free(it->pos, it->len); 
    }
    LOG(("freeOnCommit %i, mayOverwrite %i", 
         freeOnCommit.size(), mayOverwrite.size()));
    freeOnCommit.clear();
    mayOverwrite.clear();
    lastCommitedFileSize = freeSpace.getFileSize();
    assert(lastCommitedFileSize <= file.getSize());
    file.setSize(lastCommitedFileSize);
    //LOG(("commit done"));

	return true;
}

void Cache::flushAll() {
    CacheEntry *entry = lru.last();
    assert(entry != lru.first());
    while (true) {
        if (entry->isDirty()) {
            flush(entry);
            int inode = entry->getInode();
            int block = entry->getBlock();
            //flushPendingPos();
            if (inode != entry->getInode() || block != entry->getBlock()) {
                entry = lru.last();
                continue;
            }
        }
        if (entry == lru.first()) {
            break;
        }
        entry = entry->mruPrev;
    }
}

void Cache::computeFreeSpace() {
    Slices slices;
    slices.readUsedSpace(this);
    slices.sort();
    
    //now slices are now sorted ascendingly on pos
    int64 prevEndPos = kFileHeaderSize;
    for (Slice *it = slices.begin(), *end = slices.end();
         it != end; ++it) {
        assert(prevEndPos <= it->pos);
        if (prevEndPos < it->pos) {
            freeSpace.free(prevEndPos, it->pos - prevEndPos);
        }
        prevEndPos = it->pos + it->len;
    }
    int64 fileSize = file.getSize();
    assert(prevEndPos <= fileSize);
    if (prevEndPos < fileSize) {
        //warn -- perhaps resume with rollback
        LOG(("file shrink from %i to %i", fileSize, (int)prevEndPos));
        file.setSize(prevEndPos);
    }
    lastCommitedFileSize = prevEndPos;
}

bool Cache::isInodeUsed(int inode) {
    int64 buf;
    inodeFile.preadFast(&buf, sizeof(buf), inode * kInodeSize);
    return (buf != 0);
}

inline void Cache::writeInode(int inode, const char *entry) {
    int shift = params.inodeBlockShift - kInodeSizeShift;
    write(kMasterInode, inode >> shift, entry, kInodeSize, 
          (inode & ((1 << shift) - 1)) << kInodeSizeShift);
}

int Cache::allocInode(bool isDir) {
    int nHoles = freeInodes.size();
    static const int kMaxSearch = 10;
    int nSearch = kMaxSearch < nHoles ? kMaxSearch : nHoles;
    int *minPos = 0;
    for (int *it = freeInodes.begin(), 
             *end = freeInodes.begin() + nSearch;
         it < end; ++it) {
        if (!minPos || *it < *minPos) {
            minPos = it;
        }
    }
    int inode;
    if (minPos) {
        inode = *minPos;
        *minPos = *(freeInodes.end() - 1);
        freeInodes.pop_back();
    } else {
        //no holes
        assert(freeInodes.size() == 0);
        inode = nInodes++;
        //setFileSize(kMasterInode, (nInodes << kInodeSizeShift));
    }
    char entry[kInodeSize];
    memset(entry, 0, sizeof(entry));
    entry[6] = 1; //initial ref count to 1
    entry[7] = isDir ? kFlagIsDir : 0;
    //inodeFile.pwriteFast(entry, kInodeSize, (inode << kInodeSizeShift));
    writeInode(inode, entry);
    return inode;
}

void Cache::freeInode(int inode) {
    //int64 inodeFileSize = inodeFile.getSize();
    //assert((inode + 1) * kInodeSize <= inodeFileSize);
    assert(inode < nInodes);
    if (inode + 1 == nInodes) {
        int lastInode = inode - 1;
        while (!isInodeUsed(lastInode)) {
            --lastInode;
        }
        int newNInodes = lastInode + 1;
        inodeFile.shrinkSize(nInodes << kInodeSizeShift, 
                             newNInodes << kInodeSizeShift);
        nInodes = newNInodes;
        //setFileSize(kMasterInode, (nInodes << kInodeSizeShift));
    } else {
        //set block size to 0 to indicate unused inode
        char entry[kInodeSize];
        memset(entry, 0, kInodeSize);
        //inodeFile.pwriteFast(entry, sizeof(entry), (inode << kInodeSizeShift));
        writeInode(inode, entry);
        freeInodes.push_back(inode);
    }
}

int64 Cache::getFileSize(int inode) {
    assert(inode >= 0);
    char buf[5];
    inodeFile.preadFast(buf, kInodeFileSizeSize, (int64)inode << kInodeSizeShift);
    return ntoh5(buf); 
}

void Cache::setFileSize(int inode, int64 size) {
    assert(inode >= 0);
    assert(size >= 0 && size <= File_kMaxFileSize);
    char buf[5];
    hton5(size, buf);
    inodeFile.pwriteFast(buf, sizeof(buf), (int64)inode << kInodeSizeShift);
}

char *Cache::getPage(int inode, int block, int *pageSize) {
    CacheEntry *entry = getEntry(inode, block);
    *pageSize = entry->getSize();
    return (char *)(entry->getData());
}

int Cache::getPageSize(int inode, int block) {
    int size;
    getPage(inode, block, &size);
    return size;
}

void Cache::accessSuper(bool doRead, 
                        char *&buf, int &len, int &pos) { 
    int leftInSuper   = kSuperblockSize - pos;
    int accessInSuper =  (leftInSuper < len) ? 
        leftInSuper : len;
    if (doRead) {
        memcpy(buf, superblock + pos, accessInSuper);
    } else { //write
        memcpy(superblock + pos, buf, accessInSuper);
        superblockDirty = true;
    }
    buf += accessInSuper;
    pos += accessInSuper;
    len -= accessInSuper;
}

void Cache::read(int inode, int block, void * const inBuf, 
                 int len, int pos) {
    assert(len > 0 && pos >= 0);

    char *buf = (char *) inBuf;
    if (inode == kMasterInode && block == 0 && 
        pos < kSuperblockSize) {
        accessSuper(true, buf, len, pos);
        if (!len) {
            return;
        }
    }

    CacheEntry *entry = getEntry(inode, block);
    int dataLen = entry->getSize();
    int dataAfterPos = dataLen > pos ? dataLen - pos : 0;
    int readFromData = dataAfterPos < len ? dataAfterPos : len;
    int readAfterData = len - readFromData;

    if (readFromData) {
        memcpy(buf, (char *)entry->getData() + pos, readFromData);
        buf += readFromData;
    }
    if (readAfterData) {
        memset(buf, 0, readAfterData);
    }
    //lru.moveToFront(entry);
}

void Cache::write(int inode, int block, 
                  const void * const inBuf, int len, int pos) {
    int blockSize = (1 << getBlockShift(inode, block));
    assert(pos >= 0 && len > 0 && pos + len <= blockSize);
    if (len == blockSize && !lru.find(inode, block)) {
        // no need to read as will be fully overwritten anyway
        reduceSize(inode, block, 0);
    }

    char *buf = (char *)inBuf;
    if (inode == kMasterInode && block == 0 && 
        pos < kSuperblockSize) {
        accessSuper(false, buf, len, pos);
        if (len == 0) {
            return;
        }
    }

    CacheEntry *entry = getEntry(inode, block);
    assert(entry);
    int oldSize = entry->getSize();
    entry->growSize(pos + len);
    if (oldSize < pos) {
        memset((char *)entry->getData() + oldSize, 0, pos - oldSize);
    }
    memcpy((char *)entry->getData() + pos, buf, len);
    //lru.moveToFront(entry);
    setDirty(entry);
}

void Cache::getPositionPosition(int inode, int block, 
                         int *outPosInode, int *outPosBlock,
                         int *outPosInBlock) {
    if ((block >= 0  && block < kNDirect ) || 
        block == -1 || block == -2) {
        //inside inode
        int posInInode = (block >= 0) ? block : -block + kNDirect - 1;
        int64 pos =  inode * kInodeSize + posInInode * 8 + kInodeHeaderSize;
        *outPosInode   = kMasterInode; //inode of the inode file
        *outPosBlock   = (int) (pos >> params.inodeBlockShift);
        *outPosInBlock = (int) (pos & ((1 << params.inodeBlockShift) - 1));
        return;
    }

    *outPosInode   = inode;
    if (block >= 0) { //data block   
        assert(block >= kNDirect);
        block -= kNDirect;
        if (block < kNIndirect) {
            //a simple indirect, update in simple indirect block ("-1")
            *outPosBlock   = -1;
            *outPosInBlock = block * 8;
            return;
        }

        block -= kNIndirect;
        assert(block >=0 && block < kNDoubleIndirect);
        int indirectBlock = block / kNIndirect;
        assert(indirectBlock >= 0 && indirectBlock < kNIndirect);
        *outPosBlock   = -indirectBlock - 3;
        *outPosInBlock = block % kNIndirect * 8;
        return;
    }

    //indirection block
    //block < 0 denotes indirect blocks, which are not
    //part of file data, but contain pointers to file data blocks
    
    assert(block <= -3);
    block = - block - 3;
    assert(block >= 0 && block < kNIndirect);

    //update in the double indirect block ("-2")
    *outPosBlock   = -2;
    *outPosInBlock = block * 8;
}

void Cache::readPosition(int inode, int block, int64 *outPos, int *outLen) {
    assert(inode >= 0);
    assert(outPos && outLen);

    char buf[8];
    int posInode, posBlock, posInBlock;
    getPositionPosition(inode, block, &posInode, &posBlock, &posInBlock);
    read(posInode, posBlock, buf, 8, posInBlock);
    *outPos = ntoh5(buf);
    *outLen = ntoh3(buf + 5);
} 

void Cache::writePosition(int inode, int block, int64 pos, int len) {
    assert(inode >= 0);
    assert(pos >= 0 && pos < File_kMaxFileSize);  //1 << 40
    assert(len >= 0 && len < File::kMaxBlockSize); //1 << 24

    int posInode, posBlock, posInBlock;
    getPositionPosition(inode, block, &posInode, &posBlock, &posInBlock);
    char newPos[8];
    hton5(pos, newPos);
    hton3(len, newPos + 5);
    write(posInode, posBlock, newPos, 8, posInBlock);
}

void Cache::flush(CacheEntry *entry) {
    assert(entry->isDirty());
    entry->setDirty(false);
    bool isCompressed = isBlockCompressed(entry->getInode(), entry->getBlock());

    if (entry->getBlock() < 0) {
        //for indirect blocks
        //don't write trailing zero-entries
        entry->removeIndirectTrailingZeros();
    }
    int oldLen = entry->getOldLen();
    int64 oldPos = entry->getOldPos();
    int dataSize = entry->getSize();
    if (oldLen == 0 && dataSize == 0) {
        return;
    }
    void *data   = entry->getData();

    int diskSize = 0;
    int64 pos    = 0;
    assert((data != 0) == (dataSize != 0));

    if (dataSize) {
        void *diskData;
        if (isCompressed) {
            compress.doDeflate(dataSize, data, &diskSize, &diskData);
        } else {
            diskSize = dataSize;
            diskData = data;
        }
        assert(diskSize);
        pos = freeSpace.alloc(diskSize);
        file.pwrite(diskData, diskSize, pos);
    }
    
    assert(diskSize || oldLen);
    bool wasWrittenBefore = oldPos >= lastCommitedFileSize ||
        mayOverwrite.find(entry->getInode(), entry->getBlock());
    
    if (diskSize && pos < lastCommitedFileSize) {
        mayOverwrite.add(entry->getInode(), entry->getBlock());
    }
    
    if (oldLen && !wasWrittenBefore) {
        freeOnCommit.push_back(Slice(oldPos, oldLen));
    }

    //LOG(("(%i, %2i)  -->  <%4i, %4i>", inode, block, (int)(pos), diskSize));
    entry->setOldPosition(pos, diskSize);
    writePosition(entry->getInode(), entry->getBlock(), pos, diskSize);
}

CacheEntry *Cache::getEntryToReplace() {
    CacheEntry *entry = lru.last();
    if (isInsideFlush) { //avoid recursion on flush()
        entry = lru.findNotDirty();
    } else {
        while (entry->isDirty()) {
            isInsideFlush = true;
            flush(entry);
            isInsideFlush = false; //todo: handle exceptions thrown by flush()
            entry = lru.last();
        }
    }    
    assert(!entry->isDirty());
    return entry;
}

CacheEntry *Cache::getEntryFromDisk(int inode, int block) {
    int64 pos;
    int diskLen;
    int dataSize = 0;
    void *data = 0;
    readPosition(inode, block, &pos, &diskLen);
    if (CacheEntry *tmp = lru.find(inode, block)) {
        return tmp;
    }
    CacheEntry *entry = getEntryToReplace();
    if (CacheEntry *tmp = lru.find(inode, block)) {
        return tmp;
    }
    //assert(!lru.find(inode, block));

    if (diskLen) {
        bool isCompressed = isBlockCompressed(inode, block);
        assert(pos);
        if (diskLen > diskBufSize) {
            diskBufSize = diskLen;
            diskBuf = (char*)realloc(diskBuf, diskBufSize);
            assert(diskBuf);
        }

        //LOG(("(%i, %2i)  <--   <%4i, %4i>", inode, block, (int)pos, diskLen));
        int nRead = file.pread(diskBuf, diskLen, pos);
        (void)nRead;
        assert(nRead == diskLen);
        if (isCompressed) {
            compress.doInflate(diskLen, diskBuf, &dataSize, &data);
        } else {
            dataSize = diskLen;
            data     = diskBuf;
        }
    }
    lru.initEntry(entry, inode, block, dataSize, data, pos, diskLen);
    return entry;
}

CacheEntry *Cache::getEntry(int inode, int block) {
    assert(inode >= 0);
    CacheEntry *entry = lru.find(inode, block);
    if (!entry) {
        entry = getEntryFromDisk(inode, block);
    }
    lru.moveToFront(entry);
    return entry;
}

void Cache::reduceSize(int inode, int block, int newSize) {
    assert(inode >= 0 && newSize >= 0);
    CacheEntry *entry = lru.find(inode, block);
    if (entry) {
        if (entry->getSize() > newSize) {
            entry->shrinkSize(newSize);
            setDirty(entry);
        }
    } else {
        int64 pos;
        int len;
        readPosition(inode, block, &pos, &len);
        if (newSize < len) {
            if (newSize == 0) {
                entry = getEntryToReplace();
                lru.initEntry(entry, inode, block, 0, 0, pos, len);
            } else {
                entry = getEntry(inode, block);
                entry->shrinkSize(newSize);
            }
            setDirty(entry);
        }
    }
}

void Cache::setDirty(CacheEntry *entry) {
    assert(entry);
    if (!entry->isDirty()) {
        entry->setDirty(true);
        int64 oldPos = entry->getOldPos();
        int oldLen   = entry->getOldLen();
        /*
        int64 pos;
        int len;
        readPosition(entry->getInode(), entry->getBlock(), 
                     &pos, &len);
        assert(oldPos == pos && oldLen == len);
        */
        
        bool wasWrittenBefore = oldPos >= lastCommitedFileSize ||
            mayOverwrite.find(entry->getInode(), entry->getBlock());
        
        if (oldLen && wasWrittenBefore) {
            freeSpace.free(oldPos, oldLen);
        }
        /*
        if (mayOverwrite.find(entry->getInode(), entry->getBlock())) {
            //assert(oldLen); //??
            if (oldLen) {
                freeSpace.free(oldPos, oldLen);
            }
        } else {
            mayOverwrite.add(entry->getInode(), entry->getBlock());
            if (oldLen) {
                freeOnCommit.push_back(Slice(oldPos, oldLen));
            }
        }
        */
    }
}
