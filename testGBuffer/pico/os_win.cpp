/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#define WIN32_LEAN_AND_MEAN 1

#include <Windows.h>
#include "os.hpp"
#include <assert.h>
//GISE_BEGIN_NAMESPACE(pico)

/*
    int os_findfirst(const std::string &path, os_pDir *outpDir,
                     bool *outFound,
                     std::string &outName, bool *isDir) {
        WIN32_FIND_DATA data;
        std::string pathStar = path + '*';
        HANDLE h = FindFirstFile(pathStar.c_str(), &data);
        *outpDir = h;
        if (h == INVALID_HANDLE_VALUE) {
            //int err = GetLastError();
            //if (err == ERROR_NO_MORE_FILES) {
            *outFound = false;
            return 0;
        }
        outName = data.cFileName;
        *isDir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        *outFound = true;
        return 0;
    }

    int os_findnext(os_pDir pDir, bool *outFound,
                    std::string &outName,
                    bool *isDir) {
        WIN32_FIND_DATA data;
        bool ok = FindNextFile(pDir, &data);
        if (!ok) {
            *outFound = false;
            return 0;
        }
        outName = data.cFileName;
        *isDir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        *outFound = true;
        return 0;
    }
    
    int os_findclose(os_pDir pDir) {
        FindClose(pDir);
        return 0;
    }

*/

/*
    int os_exists_dir(const char *name, bool *exists) {
        unsigned attr = GetFileAttributes(name);
        if (attr == INVALID_FILE_ATTRIBUTES) {
            *exists = false;
        } else {
            *exists = attr & FILE_ATTRIBUTE_DIRECTORY;
        }
        return 0;
    }
*/

void os_mutex_init(os_pMutex &pmutex) {
    pmutex = new CRITICAL_SECTION;
    InitializeCriticalSection((CRITICAL_SECTION*)pmutex);
}

void os_mutex_lock(os_pMutex pmutex) {
    EnterCriticalSection((CRITICAL_SECTION*)pmutex);
}

void os_mutex_unlock(os_pMutex pmutex) {
    LeaveCriticalSection((CRITICAL_SECTION*)pmutex);
}

void os_mutex_destroy(os_pMutex pmutex) {
    DeleteCriticalSection((CRITICAL_SECTION*)pmutex);
    delete (CRITICAL_SECTION*)pmutex;
}

    /*
    bool os_mkdir(const char *path) {
        return CreateDirectory(path, 0);
    }
    */

bool os_open(const char *path, os_OpenMode mode, int *pfd, bool *readonly) {
    unsigned createDisp;
    switch(mode) {
    case OSMODE_EXISTING:
        createDisp = OPEN_EXISTING;
        break;
    case OSMODE_CREATE:
        createDisp = CREATE_NEW;
        break;
    case OSMODE_EXISTING_OR_CREATE:
        createDisp = OPEN_ALWAYS;
        break;
    default:
        assert(0);
        createDisp = OPEN_EXISTING;
    }

	*readonly = false;
#ifdef UNICODE
	wchar_t w_path[1024];
	MultiByteToWideChar(CP_ACP,0,path,-1,w_path,1024);
    *(HANDLE*)pfd = CreateFile(w_path, GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, 0, createDisp,
                     FILE_ATTRIBUTE_NORMAL, 0);

	//如果不能以写的方式打开，请求只读模式,hyc 20101116
	if (*(HANDLE*)pfd == INVALID_HANDLE_VALUE)
	{
		*(HANDLE*)pfd = CreateFile(w_path, GENERIC_READ,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, 0, createDisp,
                     FILE_ATTRIBUTE_NORMAL, 0);
		*readonly = true;
	}
#else
	*(HANDLE*)pfd = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, createDisp,
		FILE_ATTRIBUTE_NORMAL, 0);

	//如果不能以写的方式打开，请求只读模式,hyc 20101116
	if (*(HANDLE*)pfd == INVALID_HANDLE_VALUE)
	{
		*(HANDLE*)pfd = CreateFile(path, GENERIC_READ,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, 0, createDisp,
                     FILE_ATTRIBUTE_NORMAL, 0);
		*readonly = true;
	}
#endif
    return (*(HANDLE*)pfd != INVALID_HANDLE_VALUE);
}

bool os_close(int fd) {
    return CloseHandle((HANDLE)fd);
}

bool os_delete(const char *name) {
#ifdef UNICODE
	wchar_t w_name[1024];
	MultiByteToWideChar(CP_ACP,0,name,-1,w_name,1024);

    return DeleteFile(w_name) ||
        (GetLastError() == ERROR_FILE_NOT_FOUND);
#else
	return DeleteFile(name) ||
        (GetLastError() == ERROR_FILE_NOT_FOUND);
#endif
}

bool os_pread(int fd, void *buf, int len, int64 pos, int *lenRead) {
	//Modified by hyc 20080203.WinCE ReadFile OVERLAPPED Must be NULL.
#ifdef _WIN32_WCE	
	assert((pos >> 32) == 0i64);
	int low = pos & 0xffffffff;
	return (SetFilePointer((HANDLE)fd,low,NULL,FILE_BEGIN) != 0xffffffff) &&
		(ReadFile((HANDLE)fd, buf, len, (unsigned long *)lenRead, NULL) ||
        GetLastError() == ERROR_HANDLE_EOF);
#else
    OVERLAPPED over;
    over.hEvent     = 0;
    over.Offset     = pos & 0xffffffff;
    over.OffsetHigh = pos >> 32;
    return ReadFile((HANDLE)fd, buf, len, (unsigned long *)lenRead, &over) ||
        GetLastError() == ERROR_HANDLE_EOF;
#endif
}

bool os_pwrite(int fd, const void *buf, int len, int64 pos) {
	//Modified by hyc 20080203.WinCE WriteFile OVERLAPPED Must be NULL.
#ifdef _WIN32_WCE
	int low = pos & 0xffffffff;    
    unsigned long lenDone;
    return (SetFilePointer((HANDLE)fd,low,NULL,FILE_BEGIN) != 0xffffffff)&&
    		WriteFile((HANDLE)fd, buf, len, &lenDone, NULL);
#else
    OVERLAPPED over;
    over.hEvent     = 0;
    over.Offset     = pos & 0xffffffff;
    over.OffsetHigh = pos >> 32;
    unsigned long lenDone;
    return WriteFile((HANDLE)fd, buf, len, &lenDone, &over);
#endif
}

    /*
bool os_read(int fd, void *buf, int len, int *lenRead) {
    return ReadFile((HANDLE)fd, buf, len, (unsigned long *)lenRead, 0);
    //ERROR_HANDLE_EOF
}

bool os_write(int fd, const void *buf, int len) {
    unsigned long lenDone;
    return WriteFile((HANDLE)fd, buf, len, &lenDone, 0);
}
    */

bool os_flush(int fd) {
    return FlushFileBuffers((HANDLE)fd);
}
#include <winbase.h>
bool os_seek(int fd, int64 pos, int whence, int64 *pNewPos) {
//    return SetFilePointerEx((HANDLE)fd, *(PLARGE_INTEGER)(&pos), 
  //                          (PLARGE_INTEGER)pNewPos, whence);

//	SetFilePointer((HANDLE)fd,pos,pos,whence);
	DWORD low,high;
	low  = pos & 0x00000000ffffffff;
	high = (pos & 0xffffffff00000000) >> 32;
	low = SetFilePointer((HANDLE)fd,low,(long*)&high,whence);
	if (GetLastError() != 0)
		return false;
	if (pNewPos)
		*pNewPos = ((int64)high) << 32 | low;
	return true;
}

bool os_getSize(int fd, int64 *pSize) {
 //   return GetFileSizeEx((HANDLE)fd, (PLARGE_INTEGER)pSize);
	DWORD low,high;
	low = GetFileSize((HANDLE)fd, &high);
	
	*pSize = ((int64)high) << 32 | low;
	return true;
}

bool os_setSize(int fd, int64 newSize){
    //int64 initialPos;
    return 
        os_seek(fd, newSize, FILE_BEGIN, 0) &&
        SetEndOfFile((HANDLE)fd);
    //ret = os_seek(fd, initialPos, SEEK_SET, 0);
}

    int os_getError() {
        return GetLastError();
    }

    /*
bool os_errMessage(int errCode, char *buf, unsigned bufSize) {
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  0, errCode, 0, buf, bufSize, 0);
    return 0;
}
    */

//GISE_END_NAMESPACE
