/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "slices.hpp"
#include "cache.hpp"

#include <assert.h>

Slice::Slice(char *data) :
    pos(ntoh5(data)),
    len(ntoh3(data + 5))
{
    assert(pos > 0 && len > 0);
}

static inline int sortFn(const void *p1, const void *p2) {
    const Slice *s1 = (const Slice *)p1;
    const Slice *s2 = (const Slice *)p2;
    return (s1->pos < s2->pos) ? -1 :
        (s1->pos > s2->pos) ? 1 : 0;
}

void Slices::sort() {
    qsort(begin(), size(), sizeof(Slice), sortFn);
}

void Slices::readUsedSpace(Cache *cache) {
    int64 inodeFileSize = cache->inodeFile.getSize();
    assert((inodeFileSize & (kInodeSize - 1)) == 0);
    //int nInodes = (int)(inodeFileSize / kInodeSize);
    
    char buf[kInodeSize];
    
    for (int pos = 0; pos < inodeFileSize; pos += kInodeSize) {
        cache->inodeFile.preadFast(buf, kInodeSize, pos);        
        if (*(int64*)buf == 0) {
            cache->freeInodes.push_back(pos >> kInodeSizeShift);
        } else {            
            readInode(cache, (pos >> kInodeSizeShift), buf);
        }
    }
}

void Slices::readInode(Cache *cache, int inode, char *buf) {
    int index = 0;
    for (char *p = buf + kInodeHeaderSize, *end = buf + kInodeSize; 
         p < end; p += 8, ++index) {
        if (*(int64 *)p != 0) {
            //printf("%i %i: ", inode, index);
            push_back(Slice(p));
            if (index >= kNDirect) {
                readUsedSpaceIndirect(cache, inode, -index + kNDirect - 1);
            }
        }
    }
}

void Slices::readUsedSpaceIndirect(Cache *cache, int inode, int block) {
    assert(inode >= 0 && block < 0);
    int pageSize;
    char *page = cache->getPage(inode, block, &pageSize);
    assert((pageSize & 7) == 0);
    for (int pos = 0; pos < pageSize; pos += 8) {
        if (*(int64 *)(page + pos) != 0) {
            //printf("%i %i %i: ", inode, block, index);
            push_back(Slice(page + pos));
            if (block == -2) { //double indirect
                readUsedSpaceIndirect(cache, inode, -(pos >> 3) - 3);
                page = cache->getPage(inode, block, &pageSize);
            }
        }
    }
}
