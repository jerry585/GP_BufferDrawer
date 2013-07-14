/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _OS_HPP_
#define _OS_HPP_

#include "gen.hpp"

#if defined (_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define PICOSTORAGE_OS_WIN 1
#else
#define PICOSTORAGE_OS_POSIX 1
#endif


typedef void *os_pDir;
typedef void *os_pMutex;

//#if OS_WIN
//#include <Windows.h>
//typedef CRITICAL_SECTION os_mutex_t;
//typedef HANDLE os_pDir;
    /*
#ifndef SEEK_SET
    enum { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };
#endif
    */
//#else
//typedef pthread_mutex_t os_mutex_t;
//typedef DIR *os_pDir;
//#endif

enum os_OpenMode {OSMODE_EXISTING, OSMODE_CREATE, OSMODE_EXISTING_OR_CREATE};

    /** the windows FindFirstFile/FindNextFile api is not nice,
        the posix opendir/readdir/closedir is better (more orthogonal).
        I use findfirst-style because I can implement it with opendir/readdir,
        but not the other way around.
    */
/*
    int os_findfirst(const std::string &path, os_pDir *outpDir, bool *outFound, 
                     std::string &outName, bool *isDir);
    int os_findnext(os_pDir pDir, bool *outFound,
                    std::string &outName, bool *isDir);
    int os_findclose(os_pDir pDir);
*/
    //int os_exists_dir(const char *name, bool *exists);

    void os_mutex_init(os_pMutex &pmutex);
    void os_mutex_lock(os_pMutex pmutex);
    void os_mutex_unlock(os_pMutex pmutex);
    void os_mutex_destroy(os_pMutex pmutex);
    
    bool os_open(const char *path, os_OpenMode openMode, int *pfd, bool* readonly);
    bool os_close(int fd);
GIS_API bool os_delete(const char *name);

    //bool os_mkdir(const char *path);

    bool os_pread(int fd, void *buf, int len, int64 pos, int *pLenRead);
    bool os_pwrite(int fd, const void *buf, int len, int64 pos);
    
    bool os_read(int fd, void *buf, int len, int *pLenRead);
    bool os_write(int fd, const void *buf, int len);

bool os_flush(int fd);
    
    bool os_seek(int fd, int64 pos, int whence, int64 *pNewPos);
    bool os_getSize(int fd, int64 *pSize);
    bool os_setSize(int fd, int64 size);
    int  os_getError();


#endif
