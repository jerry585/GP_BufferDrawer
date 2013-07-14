/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "osdir.hpp"
#include "osfile.hpp"
#include "exception.hpp"
#include "os.hpp"

OsFile OsDir::openFileHelper(const char *name, int mode) {
    int fd;
	bool readonly;
    return os_open(name, (os_OpenMode)mode, &fd, &readonly) ?
        OsFile(fd,name,readonly) : OsFile();
}

OsFile OsDir::openFile(const char *name) {
    return openFileHelper(name, OSMODE_EXISTING);
}

OsFile OsDir::createFile(const char *name) {
    return openFileHelper(name, OSMODE_CREATE);
}

bool OsDir::remove(const char *name) {
    return os_delete(name);
}
