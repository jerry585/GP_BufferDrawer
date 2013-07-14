/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _OFFSETFILE_H_
#define _OFFSETFILE_H_

#include "gen.hpp"

GISE_BEGIN_NAMESPACE(pico)

template <class T>
class OffsetFile : public T {
public:
    OffsetFile(const T &iniT) :
    T(iniT),
    offset(0)
    {}

	//<---added by hyc.20080130
	OffsetFile():T(),offset(0)
	{}
	
	OffsetFile& operator = (const T& iniT)
	{
		*(T*)this = iniT;
		offset = 0;
		return *this;
	}
	//=============

    int read(void *buf, int nBytes) {
        int nDone = T::pread(buf, nBytes, offset);
        offset += nDone;
        return nDone;
    }

    void write(const void *buf, int nBytes) {
        T::pwrite(buf, nBytes, offset);
        offset += nBytes;
    }

    int64 seek(int64 pos, SeekWhence whence) {
        int64 newOff;
        switch (whence) {
        case kSeekSet :
            newOff = pos;
            break;
        case kSeekCur :
            newOff = offset + pos;
            break;
        case kSeekEnd :
            newOff = T::getSize() + pos;
            break;
        default:
            newOff = 0;
            assert(false);
        }
        if (newOff < 0 || newOff > File_kMaxFileSize) {
            assert(false);
            //throw CustomException(Exception::INVALID_SEEK, "seek");
        }
        offset = newOff;
        return offset;
    }

    int64 tell() {
        return offset;
    }

private:
    int64 offset;
};

GISE_END_NAMESPACE

#endif
