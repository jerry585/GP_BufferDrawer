/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "diriterator.hpp"
#include "cache.hpp"
#include <assert.h>

DirIterator::DirIterator(Dir iniDir) :
    file(iniDir.dirFile),
    pos(-1),
    fileSize(iniDir.getSize()),
    inode(0),
    nameLen(0)
{
    assert(file);
}

bool DirIterator::moveNext() {
    Lock lock(file.cache->getMutex());
    return moveNextNolock();
}

bool DirIterator::moveNextNolock() {
    assert(pos >= -1);
    assert(pos < fileSize);
    if (pos == -1) { //before first entry
        pos = 0;
    } else {
        file.preadFast(&nameLen, 1, pos);
        assert(nameLen > 0);
        pos += nameLen + kDirExtraLen;
    }
    while (pos < fileSize) {
        file.preadFast(&nameLen, 1, pos);
        if (nameLen) {
            break;
        }
        int blockSize = (1 << file.cache->getDataBlockShift());
        pos += blockSize - (pos % blockSize);
    }
    if (pos >= fileSize) {
        return false;
    }
    //readEntry();

    assert(pos >= 0);
    assert(nameLen > 0);    
    file.preadFast(name, nameLen + kDirExtraLen - 1, pos + 1);
    inode = ntoh4(name + nameLen);
    name[nameLen] = 0;
    return true;
}

File DirIterator::openFile() {
    return file.cache->isDir(inode) ? 
        File() : File(file.cache, inode);
}

Dir DirIterator::openDir() {
    return file.cache->isDir(inode) ?
        Dir(File(file.cache, inode)) : Dir();
}
