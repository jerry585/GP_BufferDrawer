/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "storageparams.hpp"
#include "osfile.hpp"
#include <assert.h>
#include <string.h>

StorageParams::StorageParams(int iniInodeBlockShift, int iniIndirectBlockShift, 
                             int iniDataBlockShift, int iniDirBlockShift,
                             int iniCompressFlags) :
    inodeBlockShift(iniInodeBlockShift),
    indirectBlockShift(iniIndirectBlockShift),
    dataBlockShift(iniDataBlockShift),
    dirBlockShift(iniDirBlockShift),
    flags(iniCompressFlags) {
    updateFlags();
}

StorageParams::StorageParams(OsFile file) {
    unsigned char buf[8];
    int nRead = file.pread(buf, 8, 8);
    (void)nRead;
    assert(nRead == 8);
    unsigned char sizes = *(buf + 5);
    inodeBlockShift    = (sizes >> 4);
    indirectBlockShift = (sizes & 0xf);
    sizes = *(buf + 6);
    dataBlockShift     = (sizes >> 4);
    dirBlockShift      = (sizes & 0xf);
    flags = *(buf + 7);
    updateFlags();
}

void StorageParams::updateFlags() {
    compressInode     = flags & kCompressInode;
    compressIndirect  = flags & kCompressIndirect;
    compressData      = flags & kCompressData;
    safeCommit        = flags & kSafeCommit;
}

void StorageParams::write(OsFile file) const {
    assert(file.getSize() == 0);
//    const char sign[] = "pstorage";
	const char sign[] = "GBUFFER.";
    unsigned char header[kFileSignSize];
    memset(header, 0, sizeof(header));
    memcpy(header, sign, 8);
    unsigned char *buf = header + 8;
    *(buf + 5) = (inodeBlockShift << 4) | indirectBlockShift;
    *(buf + 6) = (dataBlockShift << 4)  | dirBlockShift;
    *(buf + 7) = (unsigned char)flags;
    file.pwrite(header, sizeof(header), 0);
}
