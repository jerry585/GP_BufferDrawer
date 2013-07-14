/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _SLICES_H_
#define _SLICES_H_

#include "vector.hpp"
#include "gen.hpp"

class Cache;

struct Slice {
    Slice() :
        pos(0),
        len(0)
    {}

    Slice(int64 iniPos, int iniLen) : 
        pos(iniPos), 
        len(iniLen) 
    {}

    Slice(char *data);

    int64 pos;
    int len;
};

class Slices : public Vector<Slice> {
public:
    void readUsedSpaceIndirect(Cache *cache, int inode, int block);
    void readInode(Cache *cache, int inode, char *buf);
    void readUsedSpace(Cache *cache);
    void sort();
};


#endif
