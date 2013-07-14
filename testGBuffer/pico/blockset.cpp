/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "blockset.hpp"

bool BlockSet::find(int inode, int block) {
    assert(inode >= 0);
    return hashTable.find(((int64)block << 32) | inode);
}

bool BlockSet::add(int inode, int block) {
    assert(inode >= 0);
    bool added = hashTable.insert(((int64)block << 32) | inode, 0);
    //(void)added;
    //assert(added);
    return added;
}

void BlockSet::clear() {
    hashTable.clear();
}

void BlockSet::getBlocks(BlockVect &outBlocks) {
    outBlocks.clear();
    HashTable::EntryVect entries;
    hashTable.getAllEntries(entries);
    //Block block;
    for (HashTable::Entry *it = entries.begin(), *end = entries.end();
         it != end; ++it) {
        int64 key = it->key;
        int inode = (int)key;
        int block = (int)(key >> 32);
        outBlocks.push_back(Block(inode, block));
    }
}

