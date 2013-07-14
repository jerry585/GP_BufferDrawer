/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _DIRITERATOR_H_
#define _DIRITERATOR_H_

#include "gen.hpp"
#include "file.hpp"
#include "dir.hpp"


/**
   Allows to iterate over the entries of a directory.
   For each entry, you can obtain: its name (getName()), 
   whether the name points to a file or a directory (isDir()),
   and the size of the pointed file/directory (getFileSize()).
   
   When iterating over the content of a directory, the entries
   are not returned in alphabetical order, nor in the order
   they were created (added to the directory). The order of the
   returned entries may appear somewhat random; this is because
   the directory is implemented using a hashtable.

   An iterator is invalidated by a creation or removal of any entry
   to the underlying directory. While you're iterating over a
   directory, you're not allowed to use Dir::createFile(),
   Dir::createDir() or Dir::remove() on that directory, 
   or the iterator won't work anymore.

*/
class DirIterator {
public:
    /** Positions the iterator before the first entry of the directory.
        @param iniDir the directory on which you want to iterate.
    */
    DirIterator(Dir iniDir);

    /** Advance the iterator to the next entry.
        @return 
        - true if the iterator advanced without reaching the end (
        i.e. there was a next entry to advance to); 
        - false if the iterator was on the last entry before moveNext(), 
        while  now it's positioned past-the-end 
        (and you can't use it anymore).
        
        The first call to moveNext() (after creating the iterator),
        moves it to the first entry (if the directory is not empty).

        Example of iterating over all entries of a directory:
        <pre>
        Dir myDir = storage.getRootDir();
        for(DirIterator it(myDir); it.moveNext();) {
            entries.push_back(it.getName());
        }
        </pre>
    */
    bool moveNext();

    /** Get the name of the current entry.
        Use this variant if you know the entry name is a string without
        NULLs in the middle.
    */
    const char *getName() const { return name; }

    /** Get the name of the current entry.
        Use this variant if the name is not NULL-ended, e.g.
        if the name is binary data which may contain NULLs inside.
    */
    const void *getName(int *outNameLen) const {
        *outNameLen = nameLen;
        return name;
    }

    File openFile();
    Dir openDir();

private:
    bool moveNextNolock();

    friend class Dir;

    File file;
    int64 pos, fileSize;

    int  inode;
    int  nameLen;
    char name[kMaxNameLen + kDirExtraLen - 1];
};



#endif
