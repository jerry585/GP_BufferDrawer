/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _PICOFILE_H_
#define _PICOFILE_H_

#include "gen.hpp"


class Cache;
class Dir;
class InodeFile;
class Slices;


class GIS_API File {
public:
    File(const File &otherFile);
    ~File();
    File &operator=(const File &otherFile);
	File():cache(0),inode(0){}	//<-hyc
    int pread(void *buf, int nBytes, int64 pos) const;
    void pwrite(const void *buf, int nBytes, int64 pos);

    int64 getSize() const;
    void setSize(int64 size);
    operator bool() const { return cache != 0; }

    bool commit();

private:
    void preadFast(void *buf, int nBytes, int64 pos) const;
    void pwriteFast(const void *buf, int nBytes, int64 pos);

    void shrinkSize(int64 oldSize, int64 newSize);

    enum { kMaxBlockSize  = (1 << 24) };
#define File_kMaxFileSize (((int64)1) << 40)
    friend class Cache;
    friend class Dir;
    friend class DirIterator;
    friend class InodeFile;
    friend class Slices;
    friend GIS_API Dir openStorage(const char *, int);

//    File();
    File(Cache *iniCache, int iniInode);
    void readSlice(void *buf, int slice, int size, int pos);

    Cache *cache;
    int inode;
};




#endif
