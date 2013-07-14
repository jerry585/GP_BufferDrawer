/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "dir.hpp"
#include "diriterator.hpp"
#include "cache.hpp"
#include "log.hpp"

#include <string.h>
#include <assert.h>

Dir::Dir() :
    dirFile(),
    nBuckets(0),
    bigMask(0),
    smallMask(0)
{
}

Dir::Dir(File iniFile) :
    dirFile(iniFile),
    nBuckets(0),
    bigMask(0),
    smallMask(0)
{
    assert(dirFile);
    int blockShift = dirFile.cache->getDataBlockShift();
    int blockSize = (1 << blockShift);
    (void)blockSize;
    int64 fileSize = getSize();
    //dir file size is always multible of blocks
    assert((fileSize & (blockSize - 1)) == 0);
    assert(fileSize >= blockSize); //??
    nBuckets = (int)(fileSize >> blockShift);
    assert(nBuckets >= 1);
    bigMask = 0;
    while (bigMask < nBuckets - 1) {
        bigMask = (bigMask << 1) | 1;
    }
    smallMask = (bigMask >> 1);
}

inline void Dir::setSize(int64 size) {
    dirFile.cache->setFileSize(dirFile.inode, size);
}

//<-- not inline anymore. MSVC Release link error. hyc.
/*inline */int64 Dir::getSize() {
    return dirFile.cache->getFileSize(dirFile.inode);
}

File Dir::openFile(const char *name) {
    return openFile(name, strlen(name));
}

Dir Dir::openDir(const char *name) {
    return openDir(name, strlen(name));
}

File Dir::createFile(const char *name) {
    return createFile(name, strlen(name));
}

Dir Dir::createDir(const char *name) {
    return createDir(name, strlen(name));
}

bool Dir::remove(const char *name) {
    return remove(name, strlen(name));
}

char *findPos(const unsigned char *pos, int pageSize, 
              const unsigned char * const name, int len) {
    const unsigned char *end = pos + pageSize - len - kDirExtraLen;
    while (pos <= end) {
        if (len == *pos && !memcmp(name, pos + 1, len)) {
            return (char *)pos;
        }
        pos += *pos + kDirExtraLen;
    }
    assert(pos > end);
    //not found
    return 0;
}

static inline int findInode(char *page, int pageSize, const void *name, int len) {
    char *pos;
    if (!page || 
        !(pos = findPos((unsigned char *)page, pageSize, (unsigned char *)name, len))) {
        return 0;
    }
    int inode = ntoh4(pos + len + 1);
    assert(inode > 0);
    return inode;
}

int Dir::open(const void *name, int len) {
    assert(len > 0 && len <= kMaxNameLen);
    Cache *cache = dirFile.cache;
    int inode = dirFile.inode;
    int block = computeBucket(name, len);
    int pageSize;
    char *page = cache->getPage(inode, block, &pageSize);
    return findInode(page, pageSize, name, len);
}

File Dir::openFile(const void *name, int len) {
    Cache *cache = dirFile.cache;
    Lock lock(cache->getMutex());
    int openInode = open(name, len);
    return (openInode && !cache->isDir(openInode)) ?
        File(cache, openInode) : File();
}

Dir Dir::openDir(const void *name, int len) {
    Cache *cache = dirFile.cache;
    Lock lock(cache->getMutex());
    int openInode = open(name, len);
    return (openInode && cache->isDir(openInode)) ?
        Dir(File(cache, openInode)) : Dir();
}

File Dir::internCreateFile(const void *name, int len, 
                                   FileType fileType, 
                                   bool checkExisting) {
	//added by hyc 20111116
	if (dirFile.cache->readOnly())
		return File();

    assert(len > 0 && len <= kMaxNameLen);
    Cache *cache = dirFile.cache;
    Lock lock(cache->getMutex());
    int inode = dirFile.inode;
    int block = computeBucket(name, len);
    int pageSize = 0;
    char *page = cache->getPage(inode, block, &pageSize);
    if (checkExisting && findInode(page, pageSize, name, len)) {
        return File(); //can't open new because already exists
    }

    //name not already existing, can create new
    int entryLen = len + kDirExtraLen;
    int blockSize = (1 << dirFile.cache->getDirBlockShift());
    if (pageSize + entryLen > (blockSize >> 1)) {
        growNBuckets();
        block = computeBucket(name, len);
        pageSize = cache->getPageSize(inode, block);
    }
    while(pageSize + entryLen > blockSize) {
        growNBuckets();
        block = computeBucket(name, len);
        pageSize = cache->getPageSize(inode, block);
    }
    int newInode = 
        cache->allocInode(fileType == kTypeDir);
    if (fileType == kTypeDir) {
        cache->setFileSize(newInode, (1 << dirFile.cache->getDataBlockShift()));
    }
    char buf[kMaxEntryLen]; //may use entryLen instead of kMaxEntryLen if supported
    writeEntry(buf, name, len, newInode);
    cache->write(inode, block, buf, entryLen, pageSize);
    return File(cache, newInode);
}

inline void Dir::removeInode(int delInode) {
    assert(delInode > 0);
    Cache *cache = dirFile.cache;
    if (cache->isDir(delInode)) {
        File f(cache, delInode);
        Dir(f).removeAllEntriesNolock();
    }
    cache->freeInode(delInode);
}

void Dir::removeAllEntries() {
    Lock lock(dirFile.cache->getMutex());
    removeAllEntriesNolock();
}

void Dir::removeAllEntriesNolock() {
	//added by hyc 20111116
	if (dirFile.cache->readOnly())
		return ;

    for (DirIterator it(*this); it.moveNextNolock();) {
        removeInode(it.inode);
    }

    setSize(0);
    setSize(1 << dirFile.cache->getDataBlockShift());
    nBuckets = 1;
    bigMask = 0;
    smallMask = 0;
}

bool Dir::remove(const void *name, int len) {
	//added by hyc 20111116
	if (dirFile.cache->readOnly())
		return false;

    assert(len > 0 && len <= kMaxNameLen);
    Cache *cache = dirFile.cache;
    Lock lock(cache->getMutex());
    int inode = dirFile.inode;
    int block = computeBucket(name, len);
    int pageSize;
    char *page = cache->getPage(inode, block, &pageSize);
    char *pos;
    if (!page || 
        !(pos = findPos((unsigned char *)page, pageSize, (const unsigned char *)name, len))) {
        return false;
    }
    int delInode = ntoh4(pos + len + 1);
    int entryLen = len + kDirExtraLen;
    int newPageSize = pageSize - entryLen;
    memmove(pos, pos + entryLen, page + newPageSize - pos);
    cache->reduceSize(inode, block, newPageSize);
    if (newPageSize == 0) {
        shrinkNBuckets();
    }
    removeInode(delInode);
    return true;
}

void Dir::growNBuckets() {
    if (nBuckets == bigMask + 1) {
        bigMask = (bigMask << 1) | 1;
        smallMask = (bigMask >> 1);
    }
    Cache *cache = dirFile.cache;
    int inode = dirFile.inode;
    int blockSizeShift = cache->getDataBlockShift();
    assert(getSize() == (nBuckets << blockSizeShift));
    ++nBuckets;
    setSize(nBuckets << blockSizeShift);
    int lastBucket = nBuckets - 1;
    int splitBucket = lastBucket & smallMask;
    int splitPageSize = cache->getPageSize(inode, splitBucket);
    char *copyPage = new char[splitPageSize];
    cache->read(inode, splitBucket, copyPage, splitPageSize, 0);

    char *src     = copyPage;
    char *endSrc  = copyPage + splitPageSize;
    int posDst1 = 0;
    int posDst2 = 0;
    while (src < endSrc) {
        int nameLen = *(unsigned char *)src;
        assert(nameLen > 0);
        int totalLen = nameLen + kDirExtraLen;
        assert(src + totalLen <= endSrc); 

        int bucket = computeBucket(src + 1, nameLen);
        assert(bucket == splitBucket || bucket == lastBucket);
        int &writePos = (bucket == splitBucket) ? posDst1 : posDst2;
        cache->write(inode, bucket, src, totalLen, writePos);
        writePos += totalLen;
        src += totalLen;
        //assert(src <= endSrc);
    }
    assert(src == endSrc);
    assert(posDst1 >= 0 && posDst2 >= 0);
    assert(posDst1 + posDst2 == splitPageSize);
    cache->reduceSize(inode, splitBucket, posDst1);
    src = endSrc = 0;
    delete[] copyPage;
    copyPage = 0;
}

void Dir::shrinkNBuckets() {
    assert(nBuckets >= 0);
    int blockSizeShift = dirFile.cache->getDataBlockShift();
    Cache *cache = dirFile.cache;
    int inode = dirFile.inode;
    assert(getSize() == (nBuckets << blockSizeShift));
    LOG(("shrink from last %i", nBuckets - 1));
    while (nBuckets > 1 && 
           (cache->getPageSize(inode, nBuckets - 1) == 0)) {
        --nBuckets;
        if (smallMask == nBuckets - 1) {
            bigMask   = (bigMask >> 1);
            smallMask = (bigMask >> 1);
        }
    }
    setSize((nBuckets << blockSizeShift));
    LOG(("shrink to last %i", nBuckets - 1));
}

unsigned Dir::computeHash(const char *str, int len) {
    unsigned hash = 0;
    for (const char *it = str, *end = str + len; it < end; ++it) {
        //hash = 5 * hash + *it;
        hash = (16777619 * hash) ^ *it; 
    }
    return hash;
}

int Dir::computeBucket(const void *str, int len) {
    unsigned hash = computeHash((char*)str, len);
    int bin = hash & bigMask;
    if (bin >= nBuckets) {
        bin = hash & smallMask; //drop the most significant bit
    }
    assert(bin >= 0 && bin < nBuckets);
    return bin;
}

void Dir::writeEntry(char *pos, const void *name, int len, int inode) {
    assert(len > 0 && len <= 255);
    *(unsigned char *)pos = len;
    memcpy(pos + 1, name, len);
    hton4(inode, pos + len + 1);
    *(pos + len + 5) = 0;
}

const char* Dir::osFile()
{
	return dirFile.cache->osFilepath();
}

