/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _GEN_HPP_
#define _GEN_HPP_


#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 int64;
#else
typedef long long int64;
#endif

//uint32 hton32(uint32 val);
//uint64 hton64(uint64 val);

void hton2(unsigned val, void *outBuf);
unsigned ntoh2(const void *inBuf);

void hton3(unsigned val, void *outBuf);
unsigned ntoh3(const void *inBuf);

void hton4(unsigned val, void *outBuf);
unsigned ntoh4(const void *inBuf);

void hton5(int64 val, void *outBuf);
int64 ntoh5(const void *inBuf);

enum SeekWhence { kSeekSet = 0, kSeekCur = 1, kSeekEnd = 2 };

enum Flags { 
    kSafeCommit       = 8,
    kCompressInode    = 4, 
    kCompressIndirect = 2,
    kCompressData     = 1,
    
    kCompressAll      =  kCompressInode | kCompressIndirect | kCompressData
};

enum { 
    kMasterInode    = 0,
    kRootInode      = 1,

    kInodeFileSizePos   = 0,
    kInodeFileSizeSize  = 5,
    kInodeBlockSizePos  = 5,
    kInodeBlockSizeSize = 3,
    kInodeFlagsPos      = 15,
    kInodeFlagsSize     = 1,
    
    kInodeHeaderSize    = 8,
    kNDirect            = 1,

    kInodeSize          = kInodeHeaderSize + (kNDirect + 2) * 8, //32
    kInodeSizeShift     = 5, //kInodeSize represented as power of 2. keep in sync
    
    kMaxNameLen         = 255,

    kFileSignSize       = 16,
    kSuperblockPos      = kFileSignSize,
    kSuperblockSize     = kInodeHeaderSize + 8,
    kFileHeaderSize     = kFileSignSize + kSuperblockSize,

    kDirExtraLen        = 5
};


#if defined(_MSC_VER)
#   define GIS_DECLSPEC_EXPORT __declspec(dllexport)
#   define GIS_DECLSPEC_IMPORT __declspec(dllimport)
#else
#   define GIS_DECLSPEC_EXPORT /**/
#   define GIS_DECLSPEC_IMPORT /**/
#endif
#ifdef GIS_API_EXPORTS
#   define GIS_API GIS_DECLSPEC_EXPORT
#else
#   define GIS_API GIS_DECLSPEC_IMPORT
#endif




#endif
