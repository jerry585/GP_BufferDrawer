/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _STORAGEPARAMS_H_
#define _STORAGEPARAMS_H_

class OsFile;

struct StorageParams {
    StorageParams(int iniInodeBlockShift, int iniIndirectBlockShift, 
                  int iniDataBlockShift, int iniDirBlockShift,
                  int iniCompressFlags);

    StorageParams(OsFile file);

    void write(OsFile file) const;

    int inodeBlockShift, indirectBlockShift;
    int dataBlockShift, dirBlockShift;
    unsigned char flags;
    bool compressInode, compressIndirect, compressData;
    bool safeCommit;

private:
    void updateFlags();
};


#endif
