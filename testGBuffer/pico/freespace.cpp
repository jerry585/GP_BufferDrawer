/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "freespace.hpp"
#include "slices.hpp"
#include "log.hpp"

#include <stdio.h>
#include <assert.h>

FreeSpace::FreeSpace(int64 iniFileSize) :
    fileSize(iniFileSize),
    lastAllocEndPos(0)
{
    //printFreeSpace();
}

FreeSpace::~FreeSpace() {
    //printFreeSpace();
}

void FreeSpace::printFreeSpace() {
    HashTable::EntryVect entries;
    beginToSize.getAllEntries(entries);
    int totalFree = 0, nSlices = 0;
    Slices slices;
    for (HashTable::Entry *itp = entries.begin(), *endp = entries.end();
         itp != endp; ++itp) {
        slices.push_back(Slice(itp->key, itp->value));
    }
    slices.sort();
    fprintf(stderr, "free space: ");
    for (Slice *it = slices.begin(), *end = slices.end();
         it != end; ++it) {
        fprintf(stderr, "<%i, %i, %i>; ", (int)it->pos, it->len, 
               (int)(it->pos + it->len));
        totalFree += (int)it->len;
        ++nSlices;
    }
    fprintf(stderr, "total: %i; slices %i\n", totalFree, nSlices);
}

void FreeSpace::delSlice(int64 pos, int64 size) {
    bool found;
    found = beginToSize.erase(pos);
    assert(found);
    found = endToSize.erase(pos + size);
    assert(found);
}

void FreeSpace::addSlice(int64 pos, int64 size) {
    bool addedNew;
    addedNew = beginToSize.insert(pos, size);
    assert(addedNew);
    addedNew = endToSize.insert(pos + size, size);
    assert(addedNew);
}

void FreeSpace::free(int64 pos, int64 size) {
    assert(pos > 0 && size > 0);
    int64 prevSize, nextSize;
    if (endToSize.find(pos, &prevSize)) {
        pos  -= prevSize;
        size += prevSize;
        delSlice(pos, prevSize);
    }
    if (beginToSize.find(pos + size, &nextSize)){
        delSlice(pos + size, nextSize);
        size += nextSize;
    }
    
    if (pos + size == fileSize) {
        fileSize = pos; //shrink
    } else {
        addSlice(pos, size);
    }
    //assert(pos != 74261726);
}

int64 FreeSpace::alloc(int size) {
    int64 allocPos, allocSize;
    //try to allocate exactly after the last alloc (contigously)
    if (false && lastAllocEndPos &&
        beginToSize.find(lastAllocEndPos, &allocSize) &&
        allocSize >= size) {
        allocPos  = lastAllocEndPos;
        //LOG(("contigous alloc %i", size));
    } else {
        //find a suitable empty block
        HashTable::Entry *allocSlice = beginToSize.findSliceForAlloc(size);
        if (!allocSlice) { //didn't find any adequate slice
            //allocate by growind the file
            allocPos = fileSize;
            fileSize += size;
            lastAllocEndPos = 0;
            //LOG(("grow alloc %i", size));
            return allocPos;
        }
        assert(allocSlice);
        allocPos  = allocSlice->key;
        allocSize = allocSlice->value;
        //LOG(("hole alloc %i in %i", size, allocSize));
    }
    assert(allocSize >= size);
    delSlice(allocPos, allocSize);
    if (allocSize > size) {
        addSlice(allocPos + size, allocSize - size);
    }
    lastAllocEndPos = allocPos + size;
    return allocPos;
}

