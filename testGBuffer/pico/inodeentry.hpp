/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "cache.hpp"
#include "gen.hpp"


struct InodeEntry {
    //InodeEntry(){}
    void readEntry(char *buf) {
        fileSize = ntoh5(buf);
        flags = *(unsigned char *)(buf + 7);
        char *bufPos = buf + kInodeHeaderSize;
        for (int i = 0; i < kNDirect + 2; ++i) {
            int64 pos = ntoh5(bufPos);
            int len = ntoh3(bufPos + 5);
            blocks[i] = Slice(pos, len);
            bufPos += 8;
        }
    }

    int64 fileSize;
    unsigned char flags;
    Slice blocks[kNDirect + 2];
};

