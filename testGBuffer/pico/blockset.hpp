/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _BLOCKSET_H_
#define _BLOCKSET_H_

#include "hashtable.hpp"
#include "vector.hpp"


class BlockSet {
public:

    struct Block {
        Block(int iniInode, int iniBlock) :
            inode(iniInode),
            block(iniBlock)
        {}

        int inode;
        int block;
    };
    typedef Vector<Block> BlockVect;

    bool find(int inode, int block);
    bool add(int inode, int block);
    void clear();
    void getBlocks(BlockVect &outBlocks);
    int size() { return hashTable.size(); }

private:
    HashTable hashTable;
};

#endif
