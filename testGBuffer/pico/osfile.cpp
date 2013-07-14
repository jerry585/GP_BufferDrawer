/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "osfile.hpp"
#include "exception.hpp"
#include "os.hpp"

#include <stdio.h>
#include <string.h>
#include <cassert>

OsFile::OsFile() :
    fd(-1),filepath(0),refcount(0)
{}

OsFile::OsFile(int iniFd,const char* file,bool readOnly) :
    fd(iniFd),
    timesRead(0),
    timesWrite(0),
    bytesRead(0),
    bytesWrite(0),
	bReadOnly(readOnly)
{
	//hyc,20111114
	refcount = new int;
	*refcount = 1;

	filepath = new char[strlen(file)+1];
	strcpy(filepath,file);
}

OsFile::~OsFile() {
	//hyc,20111114
	if (refcount && --*refcount == 0)
	{
		delete refcount;
		delete[] filepath;
	}
}

void OsFile::close() {
    if (!os_close(fd)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
    fd = -1;
    //fprintf(stderr, "reads %i, bytes %i; writes %i, bytes %i\n",
    //        timesRead, bytesRead, timesWrite, bytesWrite);
}

void OsFile::flush() {
	if (bReadOnly)
	{
		assert(false);
		return;
	}
    if (!os_flush(fd)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
}

int OsFile::pread(void *buf, int nBytes, int64 pos) {
    int nRead;
    if (!os_pread(fd, buf, nBytes, pos, &nRead)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
    timesRead++;
    bytesRead += nRead;
    return nRead;
}

void OsFile::pwrite(const void *buf, int nBytes, int64 pos) {
	//hyc
	if (bReadOnly)
	{
		assert(false);
		return;
	}

    if (!os_pwrite(fd, buf, nBytes, pos)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
    timesWrite++;
    bytesWrite += nBytes;
}

int64 OsFile::getSize() {
    int64 size;
    if (!os_getSize(fd, &size)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
    return size;
}

void OsFile::setSize(int64 size) {
	//hyc
	if (bReadOnly)
	{
		assert(false);
		return;
	}
    if (!os_setSize(fd, size)) {
		assert(false);
        throw PICOSTORAGE_EX(SystemEx);
    }
}

/*
int OsFile::read(void *buf, int nBytes) {
    int nRead;
    if (int err = os_read(fd, buf, nBytes, &nRead)) {
        throw SystemException(err, "OsFile::read");
    }
    return nRead;
}

void OsFile::write(const void *buf, int nBytes) {
    if (int err = os_write(fd, buf, nBytes)) {
        throw SystemException(err, "OsFile::write");
    }
}

int64 OsFile::seek(int64 pos, SeekWhence whence) {
    int64 newPos;
    if (int err = os_seek(fd, pos, whence, &newPos)) {
        throw SystemException(err, "OsFile::seek");
    }
    return newPos;
}

int64 OsFile::tell() {
    return seek(0, kSeekCur);
}

*/
