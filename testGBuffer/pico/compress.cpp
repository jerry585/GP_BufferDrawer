/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "compress.hpp"

//for z_ prefix to zlib symbols
//#define Z_PREFIX  //<-hyc
#include "zlib/zlib.h"
#include "log.hpp"

#include <stdlib.h>
#include <assert.h>

Compress::Compress() :
    compressBufSize(64),
    compressBuf((char*)malloc(compressBufSize)),
    infStrm(0),
    defStrm(0)
{
    assert(compressBuf);
}

Compress::~Compress() {
    if (infStrm) {
        int ret = gise_inflateEnd(infStrm);
        (void)ret;
        assert(ret == Z_OK);
        delete infStrm;
        infStrm = 0;
    }

    if (defStrm) {
        int ret = gise_deflateEnd(defStrm);
        (void)ret;
        //assert(ret == Z_OK);
        delete defStrm;
        defStrm = 0;
    }

    free(compressBuf);
    compressBuf = 0;
}

void Compress::initInfStrm() {
    infStrm = new gise_z_stream;
    infStrm->zalloc = Z_NULL;
    infStrm->zfree  = Z_NULL;
    int ret = gise_inflateInit2(infStrm, -13);
    (void)ret;
    assert(ret == Z_OK);
}

void Compress::initDefStrm() {
    defStrm = new gise_z_stream;
    defStrm->zalloc = Z_NULL;
    defStrm->zfree  = Z_NULL;
    defStrm->data_type = Z_BINARY;
    //Z_DEFAULT_COMPRESSION
    int ret = gise_deflateInit2(defStrm, 1, Z_DEFLATED,
                       -13, 8, Z_DEFAULT_STRATEGY);
    (void)ret;
    assert(ret == Z_OK);    
}

void Compress::doInflate(int inLen, void *inData,
                     int *outLen, void **outData) {
    assert(inData && inLen);
    if (!infStrm) { initInfStrm(); }
    infStrm->next_in   = (unsigned char *)inData;
    infStrm->avail_in  = inLen;
    infStrm->next_out  = (unsigned char *)compressBuf;
    infStrm->avail_out = compressBufSize;
    do {
        int ret = ::gise_inflate(infStrm, Z_NO_FLUSH);
        if (ret == Z_OK) {
            assert(infStrm->avail_out == 0);
            //LOG(("increase gise_inflate buf from %i", compressBufSize));
            compressBufSize <<= 1;
            compressBuf = (char*)realloc(compressBuf, compressBufSize);
            assert(compressBuf);
            infStrm->next_out  = (unsigned char *)compressBuf + infStrm->total_out;
            infStrm->avail_out = compressBufSize - infStrm->total_out;
        } else {
            assert(ret == Z_STREAM_END);
            assert(infStrm->avail_in == 0);
            *outLen  = infStrm->total_out;
            ret = gise_inflateReset(infStrm);
            assert(ret == Z_OK);
            break;
        }
    } while (true);
    *outData = compressBuf;
}

void Compress::doDeflate(int inLen, void *inData,
                    int *outLen, void **outData) {
    if (!defStrm) { initDefStrm(); }
    defStrm->next_in   = (unsigned char *)inData;
    defStrm->avail_in  = inLen;
    int sizeBound = gise_deflateBound(defStrm, inLen);
    if (compressBufSize < sizeBound) {
        //LOG(("increase compress buf from %i to %i", compressBufSize, sizeBound));        
        compressBuf     = (char*)realloc(compressBuf, sizeBound);
        compressBufSize = sizeBound;
    }

    defStrm->next_out  = (unsigned char *)compressBuf;
    defStrm->avail_out = compressBufSize;
    
    int ret = gise_deflate(defStrm, Z_FINISH);
    assert(ret == Z_STREAM_END);
    *outLen  = defStrm->total_out;
    *outData = compressBuf;
    ret = gise_deflateReset(defStrm);
    assert(ret == Z_OK);
}

