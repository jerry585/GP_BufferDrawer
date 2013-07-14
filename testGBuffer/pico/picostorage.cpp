/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "picostorage.hpp"
#include "osfile.hpp"
#include "osdir.hpp"
#include "cache.hpp"
#include "storageparams.hpp"

#include <assert.h>

bool createStorage(const char *path,
                   int flags,
                   int inodeBlockShift,
                   int indirectBlockShift,
                   int dataBlockShift,
                   int dirBlockShift) {
    assert(inodeBlockShift > 5 && indirectBlockShift > 5 &&
           dataBlockShift > 5 && dirBlockShift > 5);
    
    OsFile file = OsDir::createFile(path);
    if (!file) {
        return false;
    }
    
    StorageParams params(inodeBlockShift, indirectBlockShift, 
                         dataBlockShift, dirBlockShift,
                         flags);
    
    Cache::initFileHeader(file, params);
    
    Cache *cache = new Cache(file, 10); //some small cache size
    int rootInode = cache->allocInode(true);
    (void)rootInode;
    assert(rootInode == kRootInode);
    cache->setFileSize(rootInode, (1 << dataBlockShift));
    cache->commit();
    delete cache;
    cache = 0;
    return true;
}

bool createStorage(const char *path, 
                       int compressFlags, bool safeCommit) {
    int flags = compressFlags;
    if (safeCommit) {
        flags |= kSafeCommit;
    }
            
    return createStorage(path, flags, 
                         13, 13, 13, 11);
}

Dir openStorage(const char *path, int cacheSizeNbPages) {
    OsFile file = OsDir::openFile(path);
    if (!file) {
        return Dir();
    }

	//======Added by hyc 20110524
	char strMagic[9]={0};
	const char sign[] = "GBUFFER.";
	const char sign2[] = "PS..NCI.";
	int nread = file.pread(strMagic,8,0);
	if (nread != 8 || (strncmp(strMagic,sign,8) != 0 && strncmp(strMagic,sign2,8) != 0))
		return Dir();
	//=======

    Cache *cache = new Cache(file, cacheSizeNbPages);
    return Dir(File(cache, kRootInode));
}
