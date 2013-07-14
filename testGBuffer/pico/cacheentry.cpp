/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "cacheentry.hpp"
#include "gen.hpp"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

// ceilingPow2(a) == ceilingPow2(b)
static inline bool samePow2(unsigned a, unsigned b) {
    unsigned x = --a ^ --b;
    return ((x < a) && (x < b)) || (a == b);
}

static inline int ceilingPow2(int x) {
    --x;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return x + 1;
}

CacheEntry::CacheEntry() :
    mruPrev(0),
    mruNext(0),
    hashNext(0),
    inode(-1),
    block(0),
    size(0),
    data(0),
    dirty(false),
    oldPos(0),
    oldLen(0)
{}

CacheEntry::~CacheEntry() {
    if (data) {
        free(data);
        data = 0;
        size = 0;
    }
    mruPrev = 0;
    mruNext = 0;
    hashNext = 0;
    inode = -1;
    block = 0;
    dirty = false;
    oldPos = 0;
    oldLen = 0;
}

void CacheEntry::init(int iniInode, int iniBlock, 
                      int iniSize, void *iniData,
                      int64 iniOldPos, int iniOldLen) {
    dirty = false;
    inode = iniInode;
    block = iniBlock;
    assert((size == 0) == (data == 0));
    assert((iniSize == 0) == (iniData == 0));
    setSize(iniSize);
    if (iniSize) {
        memcpy(data, iniData, iniSize);
    }
    oldPos = iniOldPos;
    oldLen = iniOldLen;
}

void CacheEntry::setSize(int newSize) {
    assert((size == 0) == (data == 0));
    assert(size >= 0);

    if (!samePow2(newSize, size)) {
        if (newSize == 0) {
            assert(data);
            free(data);
            data = 0;
        } else {
            data = realloc(data, ceilingPow2(newSize));
            assert(data);
        }
    }
    size = newSize;
}

void CacheEntry::removeIndirectTrailingZeros() {
    if (size) {
        assert((size & 0x7) == 0);
        int64 *it = (int64 *)data + (size >> 3) - 1;
        int64 *end = (int64 *)data - 1; 
        while (it > end && !*it) {
            --it;
        }
        int newSize = ((it + 1 - (int64*)data) << 3);
        assert(newSize <= size);
        shrinkSize(newSize);
    }
}

bool CacheEntry::test() {
    int ceiling[] = {0, 1, 2, 4, 4, 
                     8, 8, 8, 8, 16, 16};
    int n = sizeof(ceiling)/sizeof(ceiling[0]);
    for (int k = 0; k < n; ++k) {
        assert(ceilingPow2(k) == ceiling[k]);
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            /*
            printf("%i, %i: %i, %i, (%i)\n",
                   i, j, ceilingPow2(i), ceilingPow2(j),
                   (int)(samePow2(i, j)));
            */
            assert(samePow2(i, j) == samePow2(j, i));
            assert(samePow2(i, j) == (ceilingPow2(i) == ceilingPow2(j)));
        }
    }
    return 0;
}
