/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _CACHE_H_
#define _CACHE_H_

#include "file.hpp"
#include "osfile.hpp"
#include "vector.hpp"
#include "blockset.hpp"
#include "compress.hpp"
#include "storageparams.hpp"
#include "lru.hpp"
#include "mutex.hpp"
#include "freespace.hpp"
#include "slices.hpp"
#include "gen.hpp"
#include <assert.h>

/* 
file header layout:
    8 signature : 'pstorage'
    5
    1 block size: inode, indirect (shift, 4bit each)
    1 block size: data, dir       (shift, 4bit each)
    1 flags (compress)
   16 first 16 bytes of inode 0 (kMasterInode)
----

storage flags (1byte); bit 0 means least significativ bit
bit
 7-4 indirect block size (shift - 8)
   3 reserved
   2 inode file compressed
   1 indirect blocks compressed
   0 dir & files compressed
----

inode entry (for a total 64 bytes):
   5 file size
   1 == 0
   1 ref count (==1, reserved for future)
   1 flags
   5 x 8 direct blocks addr
   8 simple indirect addr
   8 double indirect addr
----

a block addr (8 bytes):
   5 position inside file (in bytes)
   3 block on-disk size 
----

a dir entry:
   1 name length
   x name (for 'name length' bytes)
   4 inode
   1 reserved (0)
----
*/


class CacheEntry;

class Cache {
public:
    enum Flags { kFlagIsDir = 1, kFlagNotCompressed = 2 };

    Cache(OsFile iniFile, int cacheSize);
    ~Cache();

    void read(int inode, int block,
              void *buf, int len, int pos);

    void write(int inode, int block, 
               const void * const inBuf, int len, int pos);

    unsigned char getFileFlags(int inode);
    void setFileFlags(int inode, bool isDir);

    int64 getFileSize(int inode);
    void setFileSize(int inode, int64 size);

    void reduceSize(int inode, int block, int newSize);
    char *getPage(int inode, int block, int *pageSize);
    int getPageSize(int inode, int block);
    void setDirty(int inode, int block);

    int allocInode(bool isDir);
    void freeInode(int inode);
    bool isDir(int inode) {
        return getFileFlags(inode) & kFlagIsDir;
    }
    
    bool isInodeUsed(int inode);

    bool commit();
    static void initFileHeader(OsFile file, const StorageParams &params);
    
    int getDataBlockShift() { return params.dataBlockShift; }
    int getDirBlockShift() { return params.dirBlockShift; }
    int getBlockShift(int inode) {
        return (inode == kMasterInode) ? params.inodeBlockShift : 
            params.dataBlockShift;
    } 
    void incRefCount() { ++refCount; }
    void decRefCount() { --refCount; if (refCount == 0) {delete this;} }
    Mutex *getMutex() { return &mutex; }
    
	const char* osFilepath(){return file.os_filepath();}
	bool readOnly(){return file.readOnly();}
private:
    //friend class CacheEntry;
    friend class Slices;

    bool isBlockCompressed(int inode, int block);
    int getBlockShift(int inode, int block);
    void writeInode(int inode, const char *entry);

    void setSize(int nbOfPages);
    void flushLevel(int level);
    void flushAll();
    void computeFreeSpace();

    void getPositionPosition(int inode, int block,
                             int *outPosInode, int *outPosBlock,
                             int *outPosInBlock);
    void readPosition(int inode, int block, int64 *outPos, int *outLen);
    void writePosition(int inode, int block, int64 pos, int len);
    
    void flush(CacheEntry *entry);

    CacheEntry *getEntry(int inode, int block);
    CacheEntry *getEntryFromDisk(int inode, int block);
    CacheEntry *clearEntry(int inode, int block);
    CacheEntry *getEntryToReplace();

    void setDirty(CacheEntry *entry);
    void accessSuper(bool doRead, 
                     char *&buf, int &len, int &pos);

    OsFile file;
    FreeSpace freeSpace;
    Vector<int> freeInodes;
    char superblock[kSuperblockSize];
    bool superblockDirty;
    int kNIndirect, kNDoubleIndirect;
    BlockSet mayOverwrite;
    Slices freeOnCommit;
    int diskBufSize;
    char *diskBuf;
    Compress compress;
    File inodeFile;
    StorageParams params;
    LRU lru;
    bool isInsideFlush;
    Mutex mutex;
    int refCount;
    int nInodes;
    int64 lastCommitedFileSize;
};

inline unsigned char Cache::getFileFlags(int inode) {
    unsigned char flags; 
    inodeFile.preadFast(&flags, 1, inode * kInodeSize + 7);
    return flags;
}

#endif
