/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _CACHEENTRY_H_
#define _CACHEENTRY_H_

#include "gen.hpp"


class CacheEntry {
public:
    CacheEntry();
    ~CacheEntry();

    void init(int iniInode, int iniBlock, int iniSize, void *iniData,
              int64 iniOldPos, int iniOldLen);
    bool isInUse() { return inode != -1; }
    void growSize(int newSize) { if (size < newSize) { setSize(newSize); } }
    void shrinkSize(int newSize) { if (newSize < size) { setSize(newSize); } }
    void removeIndirectTrailingZeros();
    int64 getOldPos() { return oldPos; }
    int getOldLen() { return oldLen; }
    void setOldPosition(int64 pos, int len) {
        oldPos = pos;
        oldLen = len;
    }
    bool isDirty() { return dirty; }
    void setDirty(bool newDirty) { dirty = newDirty; }
    int getInode() { return inode; }
    int getBlock() { return block; }
    int getSize() { return size; }
    void *getData() { return data; }

    CacheEntry *mruPrev, *mruNext;
    CacheEntry *hashNext;

private:
    void setSize(int newSize);
    static bool test();

    int inode;
    int block;
    int size;
    void *data;
    bool dirty;
    int64 oldPos;
    int oldLen;
};

#endif
