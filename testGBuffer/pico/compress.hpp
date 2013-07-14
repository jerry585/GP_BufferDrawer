/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _COMPRESS_HPP_
#define _COMPRESS_HPP_

struct gise_z_stream_s;


class Compress {
public:
    Compress();
    ~Compress();

    void doDeflate(int inLen, void *inData,
                 int *outLen, void **outData);

    void doInflate(int inLen, void *inData,
                 int *outLen, void **outData);

private:
    void initInfStrm();
    void initDefStrm();
    
    int compressBufSize;
    char *compressBuf;
    gise_z_stream_s *infStrm, *defStrm;
};



#endif
