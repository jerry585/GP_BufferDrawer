/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "file.hpp"
#include "cache.hpp"
#include "exception.hpp"
#include "log.hpp"

#include <string.h>
#include <assert.h>

File::File(const File &otherFile) :
    cache(otherFile.cache),
    inode(otherFile.inode)
{
    if (cache) {
        cache->incRefCount();
    }
}
/*
File::File() :
    cache(0),
    inode(0)
{}*/

File::File(Cache *iniCache, int iniInode) :
    cache(iniCache),
    inode(iniInode)
{
    assert(cache != 0 && inode >= kMasterInode);
    cache->incRefCount();
}

File::~File() {
    if (cache) {
        cache->decRefCount();
        cache = 0;
        inode = 0;
    }
}

File &File::operator=(const File &otherFile) {
    if (cache) {
        cache->decRefCount();
    }
    cache = otherFile.cache;
    inode = otherFile.inode;
    if (cache) {
        cache->incRefCount();
    }
    return *this;
}

bool File::commit() {
    assert(cache);
    Lock lock(cache->getMutex());
    return cache->commit(); 
}

void File::preadFast(void *startBuf, 
                         int leftToRead, int64 pos) const {
    assert(*this);
    assert(leftToRead >= 0 && pos >= 0);
    char *buf = (char *)startBuf;
    int blockShift = cache->getBlockShift(inode);
    int blockSize  = (1 << blockShift);
    while (leftToRead) {
        int block = (pos >> blockShift);
        int posInBlock = pos & (blockSize - 1);
        int leftInBlock = blockSize - posInBlock;
        int readFromBlock = 
            (leftInBlock < leftToRead) ? leftInBlock : leftToRead;
        cache->read(inode, block, buf, readFromBlock, posInBlock);
        buf += readFromBlock;
        pos += readFromBlock;
        leftToRead -= readFromBlock;
    }
}

void File::pwriteFast(const void *startBuf, 
                      int leftToWrite, int64 pos) {
    assert(*this);
    assert(leftToWrite >= 0 && pos >= 0);
    char *buf = (char *)startBuf;
    int blockShift = cache->getBlockShift(inode);
    int blockSize  = (1 << blockShift);
    while (leftToWrite) {
        int block = (pos >> blockShift);
        int posInBlock = pos & (blockSize - 1);
        int leftInBlock = blockSize - posInBlock;
        int writeToBlock = 
            (leftInBlock < leftToWrite) ? leftInBlock : leftToWrite;
        cache->write(inode, block, buf, writeToBlock, posInBlock);
        buf += writeToBlock;
        pos += writeToBlock;
        leftToWrite -= writeToBlock;
    }
}

int File::pread(void *buf, int nBytes, int64 pos) const {
    Lock lock(cache->getMutex());
    int64 size = cache->getFileSize(inode);
    if (pos + nBytes > size) {
        nBytes = (pos < size) ? size - pos : 0;
    }
    preadFast(buf, nBytes, pos);
    return nBytes;
}

void File::pwrite(const void *buf, int nBytes, int64 pos) {
	//hyc 20111116
	if (cache->readOnly())
	{
		return;
	}
    Lock lock(cache->getMutex());
    if (pos + nBytes > cache->getFileSize(inode)) {
        cache->setFileSize(inode, pos + nBytes);
    }
    pwriteFast(buf, nBytes, pos);
}

int64 File::getSize() const {
    Lock lock(cache->getMutex());
    return cache->getFileSize(inode);
}

void File::setSize(int64 newSize) {
    Lock lock(cache->getMutex());
    assert(newSize >= 0);
    int64 oldSize = cache->getFileSize(inode);
    if (newSize == oldSize) {
        return; //nothing to do
    }
    if (newSize < oldSize) {
        shrinkSize(oldSize, newSize);
    }
    cache->setFileSize(inode, newSize);
}

void File::shrinkSize(int64 oldSize, int64 newSize) {
    //shrink
    //todo: try to use newSize - 1 below
    int blockShift = cache->getDataBlockShift();
    int blockMask  = (1 << blockShift) - 1;
    int newLastBlock     = newSize >> blockShift;
    int newLastBlockSize = newSize & blockMask;
    if (!newLastBlockSize) {
        --newLastBlock;
    } else {
        assert(newLastBlockSize > 0);
        cache->reduceSize(inode, newLastBlock, newLastBlockSize);
    }
    int oldLastBlock = (oldSize - 1) >> blockShift;
    for (int block = newLastBlock + 1; 
         block <= oldLastBlock; ++block) {
        cache->reduceSize(inode, block, 0);
    }
}
