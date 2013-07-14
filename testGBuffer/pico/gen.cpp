/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "gen.hpp"

void hton2(unsigned val, void *outBuf) {
    unsigned char *buf = (unsigned char *)outBuf;
    buf[1] = val & 0xff;
    val >>= 8;
    buf[0] = val & 0xff;
}

void hton3(unsigned val, void *outBuf) {
    unsigned char *buf = (unsigned char *)outBuf;
    buf[2] = val & 0xff;
    val >>= 8;
    buf[1] = val & 0xff;
    val >>= 8;
    buf[0] = val & 0xff;
}

void hton4(unsigned val, void *outBuf) {
    unsigned char *buf = (unsigned char *)outBuf;
    buf[3] = val & 0xff;
    val >>= 8;
    buf[2] = val & 0xff;
    val >>= 8;
    buf[1] = val & 0xff;
    val >>= 8;
    buf[0] = val & 0xff;
}

void hton5(int64 val, void *outBuf) {
    unsigned char *buf = (unsigned char *)outBuf;
    hton4((unsigned)(val >> 8), outBuf);
    ((unsigned char *)buf)[4] = val & 0xff;
}

void hton8(int64 val, void *outBuf) {
    char *buf = (char *)outBuf;
    hton4(val >> 32, buf);
    hton4(val & 0xffffffff, buf + 4);
}


unsigned ntoh2(const void *inBuf) {
    const unsigned char *buf = (const unsigned char *)inBuf;
    return (buf[0] << 8) | buf[1];
}

unsigned ntoh3(const void *inBuf) {
    const unsigned char *buf = (const unsigned char *)inBuf;
    return (buf[0] << 16) | (buf[1] << 8) | buf[2];  
}

unsigned ntoh4(const void *inBuf) {
    const unsigned char *buf = (const unsigned char *)inBuf;
    return (buf[0] << 24) | (buf[1] << 16) |
        (buf[2] <<  8) | buf[3];
}

int64 ntoh5(const void *inBuf) {
    const unsigned char *buf = (const unsigned char *)inBuf;
    return (((int64)ntoh4(buf)) << 8) | buf[4];
}

int64 ntoh8(const void *inBuf) {
    const unsigned char *buf = (const unsigned char *)inBuf;
    return ((int64)ntoh4(buf) << 32) | ntoh4(buf + 4);
}
