/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "lru.hpp"
#include "cacheentry.hpp"
#include <assert.h>
#include <string.h>

inline int LRU::bucket(int inode, int block) {
    int ret = ((unsigned)inode * 29 + (unsigned)block * 31) & hashMask;
    assert(ret >= 0 && ret < kHashSize);
    return ret;
}

LRU::LRU(int iniSize) :
    kMaxSize(0),
    kHashSize(0),
    hashMask(0),
    entryTable(0),
    head(0),
    hashTable(0)
{
    kMaxSize  = iniSize;
    kHashSize = 32;
    while (kHashSize < (kMaxSize << 1)) {
        kHashSize <<= 1;
    }
    hashMask = kHashSize - 1;

    entryTable = new CacheEntry[kMaxSize];
    for (int i = 0; i < kMaxSize; ++i) {
        entryTable[i].mruNext = &entryTable[(i + 1) % kMaxSize];
        entryTable[i].mruPrev = &entryTable[(i + kMaxSize - 1) % kMaxSize];
    }
    head = &entryTable[0];
    hashTable = new CacheEntry*[kHashSize];
    memset(hashTable, 0, sizeof(CacheEntry*) * kHashSize);
}

LRU::~LRU() {
    delete[] hashTable;
    hashTable = 0;
    delete[] entryTable;
    entryTable = 0;
    head = 0;
    kMaxSize = kHashSize = hashMask = 0;
}

CacheEntry *LRU::findNotDirty() {
    //at least one entry should be non-dirty
    CacheEntry *entry = last();
    while (entry->isDirty()) {
        assert(entry != head);
        entry = entry->mruPrev;
    }
    return entry;
}

void LRU::moveToFront(CacheEntry *entry) {
    if (entry != head) {
        /*LOG(("move to front %i (%2i, %2i) instead of %i (%2i, %2i)", 
             entry - entryTable, entry->inode, entry->block,
             head - entryTable, head->inode, head->block));
        */
        if (entry != head->mruPrev) {
            entry->mruPrev->mruNext = entry->mruNext;
            entry->mruNext->mruPrev = entry->mruPrev;
            head->mruPrev->mruNext = entry;
            head->mruNext->mruPrev = head;
            entry->mruNext = head;
            entry->mruPrev = head->mruPrev;
            head->mruPrev  = entry;
        }
        head = entry;
    }
}

void LRU::moveToLast(CacheEntry *entry) {
    if (entry != head->mruPrev) {
        moveToFront(entry);
        head = entry->mruNext;
    }
}

CacheEntry *LRU::find(int inode, int block) {
    assert(inode >= 0);
    for (CacheEntry *entry = hashTable[bucket(inode, block)]; 
         entry; entry = entry->hashNext) {
        if (entry->getInode() == inode && entry->getBlock() == block) {
            return entry;
        }
    }
    return 0;
}

void LRU::initEntry(CacheEntry *entry, 
                    int inode, int block, int size, void *data,
                    int64 iniOldPos, int iniOldLen) {
    //assert(entry == head->mruPrev);
    assert(!find(inode, block)); //no duplicates
    assert(!entry->isDirty());
    if (entry->isInUse()) {
        removeFromHash(entry);
    }
    entry->init(inode, block, size, data, iniOldPos, iniOldLen);
    addToHash(entry);
    moveToFront(entry);
}

void LRU::remove(CacheEntry *entry) {
    assert(entry->isInUse());
    removeFromHash(entry);
    entry->init(-1, 0, 0, 0, 0, 0);
    moveToLast(entry);
}

void LRU::addToHash(CacheEntry *entry) {
    assert(entry);
    int slot = bucket(entry->getInode(), entry->getBlock());
    entry->hashNext = hashTable[slot];
    hashTable[slot] = entry;
}

void LRU::removeFromHash(CacheEntry *entry) {
    assert(entry);
    int slot = bucket(entry->getInode(), entry->getBlock());
    assert(hashTable[slot]);
    if (hashTable[slot] == entry) {
        hashTable[slot] = entry->hashNext;
        return;
    }
    for (CacheEntry *prev = hashTable[slot]; 
         prev->hashNext; prev = prev->hashNext) {
        if (prev->hashNext == entry) {
            prev->hashNext = entry->hashNext;
            return;
        }
    }
    assert(false); //not found
    return;
}

float LRU::computeHashConflictFactor() {
    int nElem = 0;
    int nSq = 0;
    int longest = 0;
    for (int i = 0; i < kHashSize; ++i) {
        int nConf = 0;
        for (CacheEntry *entry = hashTable[i]; entry; entry = entry->hashNext) {
            ++nConf;
            ++nElem;
        }
        nSq += nConf * nConf;
        if (nConf > longest) {
            longest = nConf;
        }
    }
    //printf("size %i, elems %i, longest %i\n", kHashSize, nElem, longest);
    return (float)nSq / nElem;
}

void LRU::assertNoneDirty() {
    for (CacheEntry *entry = entryTable, *end = entryTable + kMaxSize;
         entry < end; ++entry) {
        assert(!entry->isDirty());
    }
}

CacheEntry *LRU::last() { return head->mruPrev; }
