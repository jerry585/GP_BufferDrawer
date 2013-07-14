/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _DIR_H_
#define _DIR_H_

#include "file.hpp"


class DirIterator;
class DirEntry;

class GIS_API Dir {
public:
    typedef DirIterator Iterator;
	
	Dir();	//<-hyc

    File createFile(const void *name, int len);
    File createFile(const char *name);

    File openFile(const void *name, int len);
    File openFile(const char *name);

    Dir createDir(const void *name, int len);
    Dir createDir(const char *name);

    Dir openDir(const void *name, int len);
    Dir openDir(const char *name);
    
    bool remove(const void *name, int len);
    bool remove(const char *name);
    
    void removeAllEntries();
    operator bool() { return dirFile; }
    bool commit() { return dirFile.commit(); }

	const char* osFile(); //<-hyc.
private:
    File createFileFast(const void *name, int len);
    File createFileFast(const char *name);
    void removeAllEntriesNolock();
    void setSize(int64 size);
    int64 getSize();

    friend class PicoIterator;
    friend class DirIterator;
    friend Dir openStorage(const char *, int);

    enum { 
		kMaxEntryLen = kMaxNameLen + 6
	};
    enum FileType { kTypeFile = 1, kTypeDir }; 

    Dir(File iniFile);
 //   Dir();	//<-hyc

    void removeInode(int delInode);
    static void writeEntry(char *pos, const void *name, int len, int fileId);
    int64 readEntry(int64 pos, DirEntry *outEntry);
    static unsigned computeHash(const char *str, int len);
    int computeBucket(const void *str, int len);
    void growNBuckets();
    void shrinkNBuckets();
    int open(const void *name, int len);
    File internCreateFile(const void *name, int len, FileType fileType,
                              bool checkExisting);

    File dirFile;
    int nBuckets;
    //select a number of least significant bits from hash
    //if nBuckets is represented on k bits, 
    //bigMask has the lower k bits all set, 
    //and smallMask has the lower k-1 bits all set
    int bigMask, smallMask;
};

inline File Dir::createFile(const void *name, int len) {
    return internCreateFile(name, len, kTypeFile, true);
}

inline File Dir::createFileFast(const void *name, int len) {
    return internCreateFile(name, len, kTypeFile, false);
}

inline Dir Dir::createDir(const void *name, int len) {
    File file = 
        internCreateFile(name, len, kTypeDir, true);
    return file ? Dir(file) : Dir();
}



#endif
