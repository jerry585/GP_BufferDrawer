/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _OSFILE_HPP_
#define _OSFILE_HPP_

#include "gen.hpp"

class OsDir;

class OsFile {
public:
    OsFile();
	//==added by hyc 20111114
	OsFile(const OsFile& osf)
		:fd(osf.fd),
		timesRead(osf.timesRead),
		timesWrite(osf.timesWrite),
		bytesRead(osf.bytesRead),
		bytesWrite(osf.bytesWrite),
		refcount(osf.refcount),
		filepath(osf.filepath),
		bReadOnly(osf.bReadOnly)
	{
		if (refcount)
			++(*refcount);
	}
	//=======================

    ~OsFile();

    int pread(void *buf, int nBytes, int64 pos);
    void pwrite(const void *buf, int nBytes, int64 pos);

    int64 getSize();
    void setSize(int64 size);

    void close();
    void flush();
    operator bool() { return fd != -1; }

	const char* os_filepath(){return filepath;}
	bool readOnly(){return bReadOnly;}
private:
	//==added by hyc 20111114
	const OsFile& operator = (const OsFile&); //disable
	//====================

    friend class OsDir;
    OsFile(int fd, const char* file,bool readonly);

    int fd;
    int timesRead;
    int timesWrite;
    int bytesRead;
    int bytesWrite;

	//<-added by hyc 20101114.
	int* refcount;
	char* filepath; 
	bool bReadOnly;
	//
};

#endif
