/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _LRU_H_
#define _LRU_H_

#include "gen.hpp"

class CacheEntry;

class LRU {
public:
    LRU(int size);
    ~LRU();

    CacheEntry *find(int inode, int block);
    void moveToFront(CacheEntry *entry);
    void initEntry(CacheEntry *entry, int inode, int block, 
                   int size, void *data, int64 iniOldPos, int iniOldLen);
    void remove(CacheEntry *entry);
    CacheEntry *first() { return head; }
    CacheEntry *last();

    float computeHashConflictFactor();
    void assertNoneDirty();
    CacheEntry *findNotDirty();

private:
    void moveToLast(CacheEntry *entry);
    void addToHash(CacheEntry *entry);
    void removeFromHash(CacheEntry *entry);
    int bucket(int inode, int slice);


    int kMaxSize;
    int kHashSize;
    int hashMask;

    CacheEntry *entryTable;
    CacheEntry *head;
    CacheEntry **hashTable;
};

#endif
