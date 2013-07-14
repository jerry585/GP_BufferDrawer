/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _FREESPACE_H_
#define _FREESPACE_H_

#include "hashtable.hpp"
#include "gen.hpp"


class FreeSpace {
public:
    FreeSpace(int64 iniFileSize);
    ~FreeSpace();

    void free(int64 pos, int64 size);
    int64 alloc(int size);
    //void declareFileSize(int64 iniFileSize) { fileSize = iniFileSize; }
    void printFreeSpace();
    int64 getFileSize() const { return fileSize; }

private:
    void delSlice(int64 pos, int64 size);
    void addSlice(int64 pos, int64 size);

    HashTable beginToSize, endToSize;
    int64 fileSize;
    int64 lastAllocEndPos;
};



#endif
