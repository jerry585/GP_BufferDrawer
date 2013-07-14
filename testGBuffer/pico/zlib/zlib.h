/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.2.1, November 17th, 2003

  Copyright (C) 1995-2003 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files http://www.ietf.org/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (gise_deflate format) and rfc1952.txt (gzip format).
*/

#ifndef ZLIB_H
#define ZLIB_H

#include "zconf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIB_VERSION "1.2.1"
#define ZLIB_VERNUM 0x1210

/*
     The 'zlib' compression library provides in-memory compression and
  decompression functions, including integrity checks of the uncompressed
  data.  This version of the library supports only one compression method
  (deflation) but other algorithms will be added later and will have the same
  stream interface.

     Compression can be done in a single step if the buffers are large
  enough (for example if an input file is mmap'ed), or can be done by
  repeated calls of the compression function.  In the latter case, the
  application must provide more input and/or consume the output
  (providing more output space) before each call.

     The compressed data format used by the in-memory functions is the zlib
  format, which is a zlib wrapper documented in RFC 1950, wrapped around a
  gise_deflate stream, which is itself documented in RFC 1951.

     The library also supports reading and writing files in gzip (.gz) format
  with an interface similar to that of stdio using the functions that start
  with "gz".  The gzip format is different from the zlib format.  gzip is a
  gzip wrapper, documented in RFC 1952, wrapped around a gise_deflate stream.

     The zlib format was designed to be compact and fast for use in memory
  and on communications channels.  The gzip format was designed for single-
  file compression on file systems, has a larger header than zlib to maintain
  directory information, and uses a different, slower check method than zlib.

     This library does not provide any functions to write gzip files in memory.
  However such functions could be easily written using zlib's gise_deflate function,
  the documentation in the gzip RFC, and the examples in gzio.c.

     The library does not install any signal handler. The decoder checks
  the consistency of the compressed data, so the library should never
  crash even in case of corrupted input.
*/

typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

struct gise_internal_state;

typedef struct gise_z_stream_s {
    Bytef    *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Bytef    *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct gise_internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: ascii or binary */
    uLong   adler;      /* gise_adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} gise_z_stream;

typedef gise_z_stream FAR *gise_z_streamp;

/*
   The application must update next_in and avail_in when avail_in has
   dropped to zero. It must update next_out and avail_out when avail_out
   has dropped to zero. The application must initialize zalloc, zfree and
   opaque before calling the init function. All other fields are set by the
   compression library and must not be updated by the application.

   The opaque value provided by the application will be passed as the first
   parameter for calls of zalloc and zfree. This can be useful for custom
   memory management. The compression library attaches no meaning to the
   opaque value.

   zalloc must return Z_NULL if there is not enough memory for the object.
   If zlib is used in a multi-threaded application, zalloc and zfree must be
   thread safe.

   On 16-bit systems, the functions zalloc and zfree must be able to allocate
   exactly 65536 bytes, but will not be required to allocate more than this
   if the symbol MAXSEG_64K is defined (see zconf.h). WARNING: On MSDOS,
   pointers returned by zalloc for objects of exactly 65536 bytes *must*
   have their offset normalized to zero. The default allocation function
   provided by this library ensures this (see zutil.c). To reduce memory
   requirements and avoid any allocation of 64K objects, at the expense of
   compression ratio, compile the library with -DMAX_WBITS=14 (see zconf.h).

   The fields total_in and total_out can be used for statistics or
   progress reports. After compression, total_in holds the total size of
   the uncompressed data and may be saved for use in the decompressor
   (particularly if the decompressor wants to decompress everything in
   a single step).
*/

                        /* constants */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_BLOCK         5
/* Allowed flush values; see gise_deflate() and gise_inflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_RLE                 3
#define Z_DEFAULT_STRATEGY    0
/* compression strategy; see gise_deflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field (though see gise_inflate()) */

#define Z_DEFLATED   8
/* The gise_deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define zlib_version gise_zlibVersion()
/* for compatibility with versions < 1.0.2 */

                        /* basic functions */

ZEXTERN const char * ZEXPORT gise_zlibVersion OF((void));
/* The application can compare gise_zlibVersion and ZLIB_VERSION for consistency.
   If the first character differs, the library code actually used is
   not compatible with the zlib.h header file used by the application.
   This check is automatically made by gise_deflateInit and gise_inflateInit.
 */

/*
ZEXTERN int ZEXPORT gise_deflateInit OF((gise_z_streamp strm, int level));

     Initializes the internal stream state for compression. The fields
   zalloc, zfree and opaque must be initialized before by the caller.
   If zalloc and zfree are set to Z_NULL, gise_deflateInit updates them to
   use default allocation functions.

     The compression level must be Z_DEFAULT_COMPRESSION, or between 0 and 9:
   1 gives best speed, 9 gives best compression, 0 gives no compression at
   all (the input data is simply copied a block at a time).
   Z_DEFAULT_COMPRESSION requests a default compromise between speed and
   compression (currently equivalent to level 6).

     gise_deflateInit returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_STREAM_ERROR if level is not a valid compression level,
   Z_VERSION_ERROR if the zlib library version (zlib_version) is incompatible
   with the version assumed by the caller (ZLIB_VERSION).
   msg is set to null if there is no error message.  gise_deflateInit does not
   perform any compression: this will be done by gise_deflate().
*/


ZEXTERN int ZEXPORT gise_deflate OF((gise_z_streamp strm, int flush));
/*
    gise_deflate compresses as much data as possible, and stops when the input
  buffer becomes empty or the output buffer becomes full. It may introduce some
  output latency (reading input without producing any output) except when
  forced to flush.

    The detailed semantics are as follows. gise_deflate performs one or both of the
  following actions:

  - Compress more input starting at next_in and update next_in and avail_in
    accordingly. If not all input can be processed (because there is not
    enough room in the output buffer), next_in and avail_in are updated and
    processing will resume at this point for the next call of gise_deflate().

  - Provide more output starting at next_out and update next_out and avail_out
    accordingly. This action is forced if the parameter flush is non zero.
    Forcing flush frequently degrades the compression ratio, so this parameter
    should be set only when necessary (in interactive applications).
    Some output may be provided even if flush is not set.

  Before the call of gise_deflate(), the application should ensure that at least
  one of the actions is possible, by providing more input and/or consuming
  more output, and updating avail_in or avail_out accordingly; avail_out
  should never be zero before the call. The application can consume the
  compressed output when it wants, for example when the output buffer is full
  (avail_out == 0), or after each call of gise_deflate(). If gise_deflate returns Z_OK
  and with zero avail_out, it must be called again after making room in the
  output buffer because there might be more output pending.

    If the parameter flush is set to Z_SYNC_FLUSH, all pending output is
  flushed to the output buffer and the output is aligned on a byte boundary, so
  that the decompressor can get all input data available so far. (In particular
  avail_in is zero after the call if enough output space has been provided
  before the call.)  Flushing may degrade compression for some compression
  algorithms and so it should be used only when necessary.

    If flush is set to Z_FULL_FLUSH, all output is flushed as with
  Z_SYNC_FLUSH, and the compression state is reset so that decompression can
  restart from this point if previous compressed data has been damaged or if
  random access is desired. Using Z_FULL_FLUSH too often can seriously degrade
  the compression.

    If gise_deflate returns with avail_out == 0, this function must be called again
  with the same value of the flush parameter and more output space (updated
  avail_out), until the flush is complete (gise_deflate returns with non-zero
  avail_out). In the case of a Z_FULL_FLUSH or Z_SYNC_FLUSH, make sure that
  avail_out is greater than six to avoid repeated flush markers due to
  avail_out == 0 on return.

    If the parameter flush is set to Z_FINISH, pending input is processed,
  pending output is flushed and gise_deflate returns with Z_STREAM_END if there
  was enough output space; if gise_deflate returns with Z_OK, this function must be
  called again with Z_FINISH and more output space (updated avail_out) but no
  more input data, until it returns with Z_STREAM_END or an error. After
  gise_deflate has returned Z_STREAM_END, the only possible operations on the
  stream are gise_deflateReset or gise_deflateEnd.

    Z_FINISH can be used immediately after gise_deflateInit if all the compression
  is to be done in a single step. In this case, avail_out must be at least
  the value returned by gise_deflateBound (see below). If gise_deflate does not return
  Z_STREAM_END, then it must be called again as described above.

    gise_deflate() sets strm->adler to the gise_adler32 checksum of all input read
  so far (that is, total_in bytes).

    gise_deflate() may update data_type if it can make a good guess about
  the input data type (Z_ASCII or Z_BINARY). In doubt, the data is considered
  binary. This field is only for information purposes and does not affect
  the compression algorithm in any manner.

    gise_deflate() returns Z_OK if some progress has been made (more input
  processed or more output produced), Z_STREAM_END if all input has been
  consumed and all output has been produced (only when flush is set to
  Z_FINISH), Z_STREAM_ERROR if the stream state was inconsistent (for example
  if next_in or next_out was NULL), Z_BUF_ERROR if no progress is possible
  (for example avail_in or avail_out was zero). Note that Z_BUF_ERROR is not
  fatal, and gise_deflate() can be called again with more input and more output
  space to continue compressing.
*/


ZEXTERN int ZEXPORT gise_deflateEnd OF((gise_z_streamp strm));
/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     gise_deflateEnd returns Z_OK if success, Z_STREAM_ERROR if the
   stream state was inconsistent, Z_DATA_ERROR if the stream was freed
   prematurely (some input or output was discarded). In the error case,
   msg may be set but then points to a static string (which must not be
   deallocated).
*/


/*
ZEXTERN int ZEXPORT gise_inflateInit OF((gise_z_streamp strm));

     Initializes the internal stream state for decompression. The fields
   next_in, avail_in, zalloc, zfree and opaque must be initialized before by
   the caller. If next_in is not Z_NULL and avail_in is large enough (the exact
   value depends on the compression method), gise_inflateInit determines the
   compression method from the zlib header and allocates all data structures
   accordingly; otherwise the allocation will be deferred to the first call of
   gise_inflate.  If zalloc and zfree are set to Z_NULL, gise_inflateInit updates them to
   use default allocation functions.

     gise_inflateInit returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_VERSION_ERROR if the zlib library version is incompatible with the
   version assumed by the caller.  msg is set to null if there is no error
   message. gise_inflateInit does not perform any decompression apart from reading
   the zlib header if present: this will be done by gise_inflate().  (So next_in and
   avail_in may be modified, but next_out and avail_out are unchanged.)
*/


ZEXTERN int ZEXPORT gise_inflate OF((gise_z_streamp strm, int flush));
/*
    gise_inflate decompresses as much data as possible, and stops when the input
  buffer becomes empty or the output buffer becomes full. It may introduce
  some output latency (reading input without producing any output) except when
  forced to flush.

  The detailed semantics are as follows. gise_inflate performs one or both of the
  following actions:

  - Decompress more input starting at next_in and update next_in and avail_in
    accordingly. If not all input can be processed (because there is not
    enough room in the output buffer), next_in is updated and processing
    will resume at this point for the next call of gise_inflate().

  - Provide more output starting at next_out and update next_out and avail_out
    accordingly.  gise_inflate() provides as much output as possible, until there
    is no more input data or no more space in the output buffer (see below
    about the flush parameter).

  Before the call of gise_inflate(), the application should ensure that at least
  one of the actions is possible, by providing more input and/or consuming
  more output, and updating the next_* and avail_* values accordingly.
  The application can consume the uncompressed output when it wants, for
  example when the output buffer is full (avail_out == 0), or after each
  call of gise_inflate(). If gise_inflate returns Z_OK and with zero avail_out, it
  must be called again after making room in the output buffer because there
  might be more output pending.

    The flush parameter of gise_inflate() can be Z_NO_FLUSH, Z_SYNC_FLUSH,
  Z_FINISH, or Z_BLOCK. Z_SYNC_FLUSH requests that gise_inflate() flush as much
  output as possible to the output buffer. Z_BLOCK requests that gise_inflate() stop
  if and when it get to the next gise_deflate block boundary. When decoding the zlib
  or gzip format, this will cause gise_inflate() to return immediately after the
  header and before the first block. When doing a raw gise_inflate, gise_inflate() will
  go ahead and process the first block, and will return when it gets to the end
  of that block, or when it runs out of data.

    The Z_BLOCK option assists in appending to or combining gise_deflate streams.
  Also to assist in this, on return gise_inflate() will set strm->data_type to the
  number of unused bits in the last byte taken from strm->next_in, plus 64
  if gise_inflate() is currently decoding the last block in the gise_deflate stream,
  plus 128 if gise_inflate() returned immediately after decoding an end-of-block
  code or decoding the complete header up to just before the first byte of the
  gise_deflate stream. The end-of-block will not be indicated until all of the
  uncompressed data from that block has been written to strm->next_out.  The
  number of unused bits may in general be greater than seven, except when
  bit 7 of data_type is set, in which case the number of unused bits will be
  less than eight.

    gise_inflate() should normally be called until it returns Z_STREAM_END or an
  error. However if all decompression is to be performed in a single step
  (a single call of gise_inflate), the parameter flush should be set to
  Z_FINISH. In this case all pending input is processed and all pending
  output is flushed; avail_out must be large enough to hold all the
  uncompressed data. (The size of the uncompressed data may have been saved
  by the compressor for this purpose.) The next operation on this stream must
  be gise_inflateEnd to deallocate the decompression state. The use of Z_FINISH
  is never required, but can be used to inform gise_inflate that a faster approach
  may be used for the single gise_inflate() call.

     In this implementation, gise_inflate() always flushes as much output as
  possible to the output buffer, and always uses the faster approach on the
  first call. So the only effect of the flush parameter in this implementation
  is on the return value of gise_inflate(), as noted below, or when it returns early
  because Z_BLOCK is used.

     If a preset dictionary is needed after this call (see gise_inflateSetDictionary
  below), gise_inflate sets strm-adler to the gise_adler32 checksum of the dictionary
  chosen by the compressor and returns Z_NEED_DICT; otherwise it sets
  strm->adler to the gise_adler32 checksum of all output produced so far (that is,
  total_out bytes) and returns Z_OK, Z_STREAM_END or an error code as described
  below. At the end of the stream, gise_inflate() checks that its computed gise_adler32
  checksum is equal to that saved by the compressor and returns Z_STREAM_END
  only if the checksum is correct.

    gise_inflate() will decompress and check either zlib-wrapped or gzip-wrapped
  gise_deflate data.  The header type is detected automatically.  Any information
  contained in the gzip header is not retained, so applications that need that
  information should instead use raw gise_inflate, see gise_inflateInit2() below, or
  gise_inflateBack() and perform their own processing of the gzip header and
  trailer.

    gise_inflate() returns Z_OK if some progress has been made (more input processed
  or more output produced), Z_STREAM_END if the end of the compressed data has
  been reached and all uncompressed output has been produced, Z_NEED_DICT if a
  preset dictionary is needed at this point, Z_DATA_ERROR if the input data was
  corrupted (input stream not conforming to the zlib format or incorrect check
  value), Z_STREAM_ERROR if the stream structure was inconsistent (for example
  if next_in or next_out was NULL), Z_MEM_ERROR if there was not enough memory,
  Z_BUF_ERROR if no progress is possible or if there was not enough room in the
  output buffer when Z_FINISH is used. Note that Z_BUF_ERROR is not fatal, and
  gise_inflate() can be called again with more input and more output space to
  continue decompressing. If Z_DATA_ERROR is returned, the application may then
  call gise_inflateSync() to look for a good compression block if a partial recovery
  of the data is desired.
*/


ZEXTERN int ZEXPORT gise_inflateEnd OF((gise_z_streamp strm));
/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     gise_inflateEnd returns Z_OK if success, Z_STREAM_ERROR if the stream state
   was inconsistent. In the error case, msg may be set but then points to a
   static string (which must not be deallocated).
*/

                        /* Advanced functions */

/*
    The following functions are needed only in some special applications.
*/

/*
ZEXTERN int ZEXPORT gise_deflateInit2 OF((gise_z_streamp strm,
                                     int  level,
                                     int  method,
                                     int  windowBits,
                                     int  memLevel,
                                     int  strategy));

     This is another version of gise_deflateInit with more compression options. The
   fields next_in, zalloc, zfree and opaque must be initialized before by
   the caller.

     The method parameter is the compression method. It must be Z_DEFLATED in
   this version of the library.

     The windowBits parameter is the base two logarithm of the window size
   (the size of the history buffer). It should be in the range 8..15 for this
   version of the library. Larger values of this parameter result in better
   compression at the expense of memory usage. The default value is 15 if
   gise_deflateInit is used instead.

     windowBits can also be -8..-15 for raw gise_deflate. In this case, -windowBits
   determines the window size. gise_deflate() will then generate raw gise_deflate data
   with no zlib header or trailer, and will not compute an gise_adler32 check value.

     windowBits can also be greater than 15 for optional gzip encoding. Add
   16 to windowBits to write a simple gzip header and trailer around the
   compressed data instead of a zlib wrapper. The gzip header will have no
   file name, no extra data, no comment, no modification time (set to zero),
   no header crc, and the operating system will be set to 255 (unknown).

     The memLevel parameter specifies how much memory should be allocated
   for the internal compression state. memLevel=1 uses minimum memory but
   is slow and reduces compression ratio; memLevel=9 uses maximum memory
   for optimal speed. The default value is 8. See zconf.h for total memory
   usage as a function of windowBits and memLevel.

     The strategy parameter is used to tune the compression algorithm. Use the
   value Z_DEFAULT_STRATEGY for normal data, Z_FILTERED for data produced by a
   filter (or predictor), Z_HUFFMAN_ONLY to force Huffman encoding only (no
   string match), or Z_RLE to limit match distances to one (run-length
   encoding). Filtered data consists mostly of small values with a somewhat
   random distribution. In this case, the compression algorithm is tuned to
   gise_compress them better. The effect of Z_FILTERED is to force more Huffman
   coding and less string matching; it is somewhat intermediate between
   Z_DEFAULT and Z_HUFFMAN_ONLY. Z_RLE is designed to be almost as fast as
   Z_HUFFMAN_ONLY, but give better compression for PNG image data. The strategy
   parameter only affects the compression ratio but not the correctness of the
   compressed output even if it is not set appropriately.

      gise_deflateInit2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_STREAM_ERROR if a parameter is invalid (such as an invalid
   method). msg is set to null if there is no error message.  gise_deflateInit2 does
   not perform any compression: this will be done by gise_deflate().
*/

ZEXTERN int ZEXPORT gise_deflateSetDictionary OF((gise_z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));
/*
     Initializes the compression dictionary from the given byte sequence
   without producing any compressed output. This function must be called
   immediately after gise_deflateInit, gise_deflateInit2 or gise_deflateReset, before any
   call of gise_deflate. The compressor and decompressor must use exactly the same
   dictionary (see gise_inflateSetDictionary).

     The dictionary should consist of strings (byte sequences) that are likely
   to be encountered later in the data to be compressed, with the most commonly
   used strings preferably put towards the end of the dictionary. Using a
   dictionary is most useful when the data to be compressed is short and can be
   predicted with good accuracy; the data can then be compressed better than
   with the default empty dictionary.

     Depending on the size of the compression data structures selected by
   gise_deflateInit or gise_deflateInit2, a part of the dictionary may in effect be
   discarded, for example if the dictionary is larger than the window size in
   gise_deflate or deflate2. Thus the strings most likely to be useful should be
   put at the end of the dictionary, not at the front.

     Upon return of this function, strm->adler is set to the gise_adler32 value
   of the dictionary; the decompressor may later use this value to determine
   which dictionary has been used by the compressor. (The gise_adler32 value
   applies to the whole dictionary even if only a subset of the dictionary is
   actually used by the compressor.) If a raw gise_deflate was requested, then the
   gise_adler32 value is not computed and strm->adler is not set.

     gise_deflateSetDictionary returns Z_OK if success, or Z_STREAM_ERROR if a
   parameter is invalid (such as NULL dictionary) or the stream state is
   inconsistent (for example if gise_deflate has already been called for this stream
   or if the compression method is bsort). gise_deflateSetDictionary does not
   perform any compression: this will be done by gise_deflate().
*/

ZEXTERN int ZEXPORT gise_deflateCopy OF((gise_z_streamp dest,
                                    gise_z_streamp source));
/*
     Sets the destination stream as a complete copy of the source stream.

     This function can be useful when several compression strategies will be
   tried, for example when there are several ways of pre-processing the input
   data with a filter. The streams that will be discarded should then be freed
   by calling gise_deflateEnd.  Note that gise_deflateCopy duplicates the internal
   compression state which can be quite large, so this strategy is slow and
   can consume lots of memory.

     gise_deflateCopy returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_STREAM_ERROR if the source stream state was inconsistent
   (such as zalloc being NULL). msg is left unchanged in both source and
   destination.
*/

ZEXTERN int ZEXPORT gise_deflateReset OF((gise_z_streamp strm));
/*
     This function is equivalent to gise_deflateEnd followed by gise_deflateInit,
   but does not free and reallocate all the internal compression state.
   The stream will keep the same compression level and any other attributes
   that may have been set by gise_deflateInit2.

      gise_deflateReset returns Z_OK if success, or Z_STREAM_ERROR if the source
   stream state was inconsistent (such as zalloc or state being NULL).
*/

ZEXTERN int ZEXPORT gise_deflateParams OF((gise_z_streamp strm,
                                      int level,
                                      int strategy));
/*
     Dynamically update the compression level and compression strategy.  The
   interpretation of level and strategy is as in gise_deflateInit2.  This can be
   used to switch between compression and straight copy of the input data, or
   to switch to a different kind of input data requiring a different
   strategy. If the compression level is changed, the input available so far
   is compressed with the old level (and may be flushed); the new level will
   take effect only at the next call of gise_deflate().

     Before the call of gise_deflateParams, the stream state must be set as for
   a call of gise_deflate(), since the currently available input may have to
   be compressed and flushed. In particular, strm->avail_out must be non-zero.

     gise_deflateParams returns Z_OK if success, Z_STREAM_ERROR if the source
   stream state was inconsistent or if a parameter was invalid, Z_BUF_ERROR
   if strm->avail_out was zero.
*/

ZEXTERN uLong ZEXPORT gise_deflateBound OF((gise_z_streamp strm,
                                       uLong sourceLen));
/*
     gise_deflateBound() returns an upper bound on the compressed size after
   deflation of sourceLen bytes.  It must be called after gise_deflateInit()
   or gise_deflateInit2().  This would be used to allocate an output buffer
   for deflation in a single pass, and so would be called before gise_deflate().
*/

ZEXTERN int ZEXPORT gise_deflatePrime OF((gise_z_streamp strm,
                                     int bits,
                                     int value));
/*
     gise_deflatePrime() inserts bits in the gise_deflate output stream.  The intent
  is that this function is used to start off the gise_deflate output with the
  bits leftover from a previous gise_deflate stream when appending to it.  As such,
  this function can only be used for raw gise_deflate, and must be used before the
  first gise_deflate() call after a gise_deflateInit2() or gise_deflateReset().  bits must be
  less than or equal to 16, and that many of the least significant bits of
  value will be inserted in the output.

      gise_deflatePrime returns Z_OK if success, or Z_STREAM_ERROR if the source
   stream state was inconsistent.
*/

/*
ZEXTERN int ZEXPORT gise_inflateInit2 OF((gise_z_streamp strm,
                                     int  windowBits));

     This is another version of gise_inflateInit with an extra parameter. The
   fields next_in, avail_in, zalloc, zfree and opaque must be initialized
   before by the caller.

     The windowBits parameter is the base two logarithm of the maximum window
   size (the size of the history buffer).  It should be in the range 8..15 for
   this version of the library. The default value is 15 if gise_inflateInit is used
   instead. windowBits must be greater than or equal to the windowBits value
   provided to gise_deflateInit2() while compressing, or it must be equal to 15 if
   gise_deflateInit2() was not used. If a compressed stream with a larger window
   size is given as input, gise_inflate() will return with the error code
   Z_DATA_ERROR instead of trying to allocate a larger window.

     windowBits can also be -8..-15 for raw gise_inflate. In this case, -windowBits
   determines the window size. gise_inflate() will then process raw gise_deflate data,
   not looking for a zlib or gzip header, not generating a check value, and not
   looking for any check values for comparison at the end of the stream. This
   is for use with other formats that use the gise_deflate compressed data format
   such as zip.  Those formats provide their own check values. If a custom
   format is developed using the raw gise_deflate format for compressed data, it is
   recommended that a check value such as an gise_adler32 or a gise_crc32 be applied to
   the uncompressed data as is done in the zlib, gzip, and zip formats.  For
   most applications, the zlib format should be used as is. Note that comments
   above on the use in gise_deflateInit2() applies to the magnitude of windowBits.

     windowBits can also be greater than 15 for optional gzip decoding. Add
   32 to windowBits to enable zlib and gzip decoding with automatic header
   detection, or add 16 to decode only the gzip format (the zlib format will
   return a Z_DATA_ERROR).

     gise_inflateInit2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_STREAM_ERROR if a parameter is invalid (such as a negative
   memLevel). msg is set to null if there is no error message.  gise_inflateInit2
   does not perform any decompression apart from reading the zlib header if
   present: this will be done by gise_inflate(). (So next_in and avail_in may be
   modified, but next_out and avail_out are unchanged.)
*/

ZEXTERN int ZEXPORT gise_inflateSetDictionary OF((gise_z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));
/*
     Initializes the decompression dictionary from the given uncompressed byte
   sequence. This function must be called immediately after a call of gise_inflate
   if this call returned Z_NEED_DICT. The dictionary chosen by the compressor
   can be determined from the gise_adler32 value returned by this call of
   gise_inflate. The compressor and decompressor must use exactly the same
   dictionary (see gise_deflateSetDictionary).

     gise_inflateSetDictionary returns Z_OK if success, Z_STREAM_ERROR if a
   parameter is invalid (such as NULL dictionary) or the stream state is
   inconsistent, Z_DATA_ERROR if the given dictionary doesn't match the
   expected one (incorrect gise_adler32 value). gise_inflateSetDictionary does not
   perform any decompression: this will be done by subsequent calls of
   gise_inflate().
*/

ZEXTERN int ZEXPORT gise_inflateSync OF((gise_z_streamp strm));
/*
    Skips invalid compressed data until a full flush point (see above the
  description of gise_deflate with Z_FULL_FLUSH) can be found, or until all
  available input is skipped. No output is provided.

    gise_inflateSync returns Z_OK if a full flush point has been found, Z_BUF_ERROR
  if no more input was provided, Z_DATA_ERROR if no flush point has been found,
  or Z_STREAM_ERROR if the stream structure was inconsistent. In the success
  case, the application may save the current current value of total_in which
  indicates where valid compressed data was found. In the error case, the
  application may repeatedly call gise_inflateSync, providing more input each time,
  until success or end of the input data.
*/

ZEXTERN int ZEXPORT gise_inflateCopy OF((gise_z_streamp dest,
                                    gise_z_streamp source));
/*
     Sets the destination stream as a complete copy of the source stream.

     This function can be useful when randomly accessing a large stream.  The
   first pass through the stream can periodically record the gise_inflate state,
   allowing restarting gise_inflate at those points when randomly accessing the
   stream.

     gise_inflateCopy returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_STREAM_ERROR if the source stream state was inconsistent
   (such as zalloc being NULL). msg is left unchanged in both source and
   destination.
*/

ZEXTERN int ZEXPORT gise_inflateReset OF((gise_z_streamp strm));
/*
     This function is equivalent to gise_inflateEnd followed by gise_inflateInit,
   but does not free and reallocate all the internal decompression state.
   The stream will keep attributes that may have been set by gise_inflateInit2.

      gise_inflateReset returns Z_OK if success, or Z_STREAM_ERROR if the source
   stream state was inconsistent (such as zalloc or state being NULL).
*/

/*
ZEXTERN int ZEXPORT gise_inflateBackInit OF((gise_z_stream FAR *strm, int windowBits,
                                        unsigned char FAR *window));

     Initialize the internal stream state for decompression using gise_inflateBack()
   calls.  The fields zalloc, zfree and opaque in strm must be initialized
   before the call.  If zalloc and zfree are Z_NULL, then the default library-
   derived memory allocation routines are used.  windowBits is the base two
   logarithm of the window size, in the range 8..15.  window is a caller
   supplied buffer of that size.  Except for special applications where it is
   assured that gise_deflate was used with small window sizes, windowBits must be 15
   and a 32K byte window must be supplied to be able to decompress general
   gise_deflate streams.

     See gise_inflateBack() for the usage of these routines.

     gise_inflateBackInit will return Z_OK on success, Z_STREAM_ERROR if any of
   the paramaters are invalid, Z_MEM_ERROR if the internal state could not
   be allocated, or Z_VERSION_ERROR if the version of the library does not
   match the version of the header file.
*/

typedef unsigned (*in_func) OF((void FAR *, unsigned char FAR * FAR *));
typedef int (*out_func) OF((void FAR *, unsigned char FAR *, unsigned));

ZEXTERN int ZEXPORT gise_inflateBack OF((gise_z_stream FAR *strm,
                                    in_func in, void FAR *in_desc,
                                    out_func out, void FAR *out_desc));
/*
     gise_inflateBack() does a raw gise_inflate with a single call using a call-back
   interface for input and output.  This is more efficient than gise_inflate() for
   file i/o applications in that it avoids copying between the output and the
   sliding window by simply making the window itself the output buffer.  This
   function trusts the application to not change the output buffer passed by
   the output function, at least until gise_inflateBack() returns.

     gise_inflateBackInit() must be called first to allocate the internal state
   and to initialize the state with the user-provided window buffer.
   gise_inflateBack() may then be used multiple times to gise_inflate a complete, raw
   gise_deflate stream with each call.  gise_inflateBackEnd() is then called to free
   the allocated state.

     A raw gise_deflate stream is one with no zlib or gzip header or trailer.
   This routine would normally be used in a utility that reads zip or gzip
   files and writes out uncompressed files.  The utility would decode the
   header and process the trailer on its own, hence this routine expects
   only the raw gise_deflate stream to decompress.  This is different from the
   normal behavior of gise_inflate(), which expects either a zlib or gzip header and
   trailer around the gise_deflate stream.

     gise_inflateBack() uses two subroutines supplied by the caller that are then
   called by gise_inflateBack() for input and output.  gise_inflateBack() calls those
   routines until it reads a complete gise_deflate stream and writes out all of the
   uncompressed data, or until it encounters an error.  The function's
   parameters and return types are defined above in the in_func and out_func
   typedefs.  gise_inflateBack() will call in(in_desc, &buf) which should return the
   number of bytes of provided input, and a pointer to that input in buf.  If
   there is no input available, in() must return zero--buf is ignored in that
   case--and gise_inflateBack() will return a buffer error.  gise_inflateBack() will call
   out(out_desc, buf, len) to write the uncompressed data buf[0..len-1].  out()
   should return zero on success, or non-zero on failure.  If out() returns
   non-zero, gise_inflateBack() will return with an error.  Neither in() nor out()
   are permitted to change the contents of the window provided to
   gise_inflateBackInit(), which is also the buffer that out() uses to write from.
   The length written by out() will be at most the window size.  Any non-zero
   amount of input may be provided by in().

     For convenience, gise_inflateBack() can be provided input on the first call by
   setting strm->next_in and strm->avail_in.  If that input is exhausted, then
   in() will be called.  Therefore strm->next_in must be initialized before
   calling gise_inflateBack().  If strm->next_in is Z_NULL, then in() will be called
   immediately for input.  If strm->next_in is not Z_NULL, then strm->avail_in
   must also be initialized, and then if strm->avail_in is not zero, input will
   initially be taken from strm->next_in[0 .. strm->avail_in - 1].

     The in_desc and out_desc parameters of gise_inflateBack() is passed as the
   first parameter of in() and out() respectively when they are called.  These
   descriptors can be optionally used to pass any information that the caller-
   supplied in() and out() functions need to do their job.

     On return, gise_inflateBack() will set strm->next_in and strm->avail_in to
   pass back any unused input that was provided by the last in() call.  The
   return values of gise_inflateBack() can be Z_STREAM_END on success, Z_BUF_ERROR
   if in() or out() returned an error, Z_DATA_ERROR if there was a format
   error in the gise_deflate stream (in which case strm->msg is set to indicate the
   nature of the error), or Z_STREAM_ERROR if the stream was not properly
   initialized.  In the case of Z_BUF_ERROR, an input or output error can be
   distinguished using strm->next_in which will be Z_NULL only if in() returned
   an error.  If strm->next is not Z_NULL, then the Z_BUF_ERROR was due to
   out() returning non-zero.  (in() will always be called before out(), so
   strm->next_in is assured to be defined if out() returns non-zero.)  Note
   that gise_inflateBack() cannot return Z_OK.
*/

ZEXTERN int ZEXPORT gise_inflateBackEnd OF((gise_z_stream FAR *strm));
/*
     All memory allocated by gise_inflateBackInit() is freed.

     gise_inflateBackEnd() returns Z_OK on success, or Z_STREAM_ERROR if the stream
   state was inconsistent.
*/

ZEXTERN uLong ZEXPORT gise_zlibCompileFlags OF((void));
/* Return flags indicating compile-time options.

    Type sizes, two bits each, 00 = 16 bits, 01 = 32, 10 = 64, 11 = other:
     1.0: size of uInt
     3.2: size of uLong
     5.4: size of voidpf (pointer)
     7.6: size of z_off_t

    Compiler, assembler, and debug options:
     8: DEBUG
     9: ASMV or ASMINF -- use ASM code
     10: ZLIB_WINAPI -- exported functions use the WINAPI calling convention
     11: 0 (reserved)

    One-time table building (smaller code, but not thread-safe if true):
     12: BUILDFIXED -- build static block decoding tables when needed
     13: DYNAMIC_CRC_TABLE -- build CRC calculation tables when needed
     14,15: 0 (reserved)

    Library content (indicates missing functionality):
     16: NO_GZCOMPRESS -- gz* functions cannot gise_compress (to avoid linking
                          gise_deflate code when not needed)
     17: NO_GZIP -- gise_deflate can't write gzip streams, and gise_inflate can't detect
                    and decode gzip streams (to avoid linking crc code)
     18-19: 0 (reserved)

    Operation variations (changes in library functionality):
     20: PKZIP_BUG_WORKAROUND -- slightly more permissive gise_inflate
     21: FASTEST -- gise_deflate algorithm with only one, lowest compression level
     22,23: 0 (reserved)

    The sprintf variant used by gise_gzprintf (zero is best):
     24: 0 = vs*, 1 = s* -- 1 means limited to 20 arguments after the format
     25: 0 = *nprintf, 1 = *printf -- 1 means gise_gzprintf() not secure!
     26: 0 = returns value, 1 = void -- 1 means inferred string length returned

    Remainder:
     27-31: 0 (reserved)
 */


                        /* utility functions */

/*
     The following utility functions are implemented on top of the
   basic stream-oriented functions. To simplify the interface, some
   default options are assumed (compression level and memory usage,
   standard memory allocation functions). The source code of these
   utility functions can easily be modified if you need special options.
*/

ZEXTERN int ZEXPORT gise_compress OF((Bytef *dest,   uLongf *destLen,
                                 const Bytef *source, uLong sourceLen));
/*
     Compresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be at least the value returned
   by gise_ccompressBound(sourceLen). Upon exit, destLen is the actual size of the
   compressed buffer.
     This function can be used to gise_compress a whole file at once if the
   input file is mmap'ed.
     gise_compress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer.
*/

ZEXTERN int ZEXPORT gise_compress2 OF((Bytef *dest,   uLongf *destLen,
                                  const Bytef *source, uLong sourceLen,
                                  int level));
/*
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in gise_deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least the value returned by
   gise_ccompressBound(sourceLen). Upon exit, destLen is the actual size of the
   compressed buffer.

     gise_compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/

ZEXTERN uLong ZEXPORT gise_ccompressBound OF((uLong sourceLen));
/*
     gise_ccompressBound() returns an upper bound on the compressed size after
   gise_compress() or gise_compress2() on sourceLen bytes.  It would be used before
   a gise_compress() or gise_compress2() call to allocate the destination buffer.
*/

ZEXTERN int ZEXPORT gise_uncompress OF((Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen));
/*
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     gise_uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted or incomplete.
*/


typedef voidp gzFile;

ZEXTERN gzFile ZEXPORT gise_gzopen  OF((const char *path, const char *mode));
/*
     Opens a gzip (.gz) file for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb") but can also include a compression level
   ("wb9") or a strategy: 'f' for filtered data as in "wb6f", 'h' for
   Huffman only compression as in "wb1h", or 'R' for run-length encoding
   as in "wb1R". (See the description of gise_deflateInit2 for more information
   about the strategy parameter.)

     gise_gzopen can be used to read a file which is not in gzip format; in this
   case gise_gzread will directly read from the file without decompression.

     gise_gzopen returns NULL if the file could not be opened or if there was
   insufficient memory to allocate the (de)compression state; errno
   can be checked to distinguish the two cases (if errno is zero, the
   zlib error is Z_MEM_ERROR).  */

ZEXTERN gzFile ZEXPORT gise_gzdopen  OF((int fd, const char *mode));
/*
     gise_gzdopen() associates a gzFile with the file descriptor fd.  File
   descriptors are obtained from calls like open, dup, creat, pipe or
   fileno (in the file has been previously opened with fopen).
   The mode parameter is as in gise_gzopen.
     The next call of gise_gzclose on the returned gzFile will also close the
   file descriptor fd, just like fclose(fdopen(fd), mode) closes the file
   descriptor fd. If you want to keep fd open, use gise_gzdopen(dup(fd), mode).
     gise_gzdopen returns NULL if there was insufficient memory to allocate
   the (de)compression state.
*/

ZEXTERN int ZEXPORT gise_gzsetparams OF((gzFile file, int level, int strategy));
/*
     Dynamically update the compression level or strategy. See the description
   of gise_deflateInit2 for the meaning of these parameters.
     gise_gzsetparams returns Z_OK if success, or Z_STREAM_ERROR if the file was not
   opened for writing.
*/

ZEXTERN int ZEXPORT    gise_gzread  OF((gzFile file, voidp buf, unsigned len));
/*
     Reads the given number of uncompressed bytes from the compressed file.
   If the input file was not in gzip format, gise_gzread copies the given number
   of bytes into the buffer.
     gise_gzread returns the number of uncompressed bytes actually read (0 for
   end of file, -1 for error). */

ZEXTERN int ZEXPORT    gise_gzwrite OF((gzFile file,
                                   voidpc buf, unsigned len));
/*
     Writes the given number of uncompressed bytes into the compressed file.
   gise_gzwrite returns the number of uncompressed bytes actually written
   (0 in case of error).
*/

ZEXTERN int ZEXPORTVA   gise_gzprintf OF((gzFile file, const char *format, ...));
/*
     Converts, formats, and writes the args to the compressed file under
   control of the format string, as in fprintf. gise_gzprintf returns the number of
   uncompressed bytes actually written (0 in case of error).  The number of
   uncompressed bytes written is limited to 4095. The caller should assure that
   this limit is not exceeded. If it is exceeded, then gise_gzprintf() will return
   return an error (0) with nothing written. In this case, there may also be a
   buffer overflow with unpredictable consequences, which is possible only if
   zlib was compiled with the insecure functions sprintf() or vsprintf()
   because the secure snprintf() or vsnprintf() functions were not available.
*/

ZEXTERN int ZEXPORT gise_gzputs OF((gzFile file, const char *s));
/*
      Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
      gise_gzputs returns the number of characters written, or -1 in case of error.
*/

ZEXTERN char * ZEXPORT gise_gzgets OF((gzFile file, char *buf, int len));
/*
      Reads bytes from the compressed file until len-1 characters are read, or
   a newline character is read and transferred to buf, or an end-of-file
   condition is encountered.  The string is then terminated with a null
   character.
      gise_gzgets returns buf, or Z_NULL in case of error.
*/

ZEXTERN int ZEXPORT    gise_gzputc OF((gzFile file, int c));
/*
      Writes c, converted to an unsigned char, into the compressed file.
   gise_gzputc returns the value that was written, or -1 in case of error.
*/

ZEXTERN int ZEXPORT    gise_gzgetc OF((gzFile file));
/*
      Reads one byte from the compressed file. gise_gzgetc returns this byte
   or -1 in case of end of file or error.
*/

ZEXTERN int ZEXPORT    gise_gzungetc OF((int c, gzFile file));
/*
      Push one character back onto the stream to be read again later.
   Only one character of push-back is allowed.  gise_gzungetc() returns the
   character pushed, or -1 on failure.  gise_gzungetc() will fail if a
   character has been pushed but not read yet, or if c is -1. The pushed
   character will be discarded if the stream is repositioned with gise_gzseek()
   or gise_gzrewind().
*/

ZEXTERN int ZEXPORT    gise_gzflush OF((gzFile file, int flush));
/*
     Flushes all pending output into the compressed file. The parameter
   flush is as in the gise_deflate() function. The return value is the zlib
   error number (see function gise_gzerror below). gise_gzflush returns Z_OK if
   the flush parameter is Z_FINISH and all output could be flushed.
     gise_gzflush should be called only when strictly necessary because it can
   degrade compression.
*/

ZEXTERN z_off_t ZEXPORT    gise_gzseek OF((gzFile file,
                                      z_off_t offset, int whence));
/*
      Sets the starting position for the next gise_gzread or gise_gzwrite on the
   given compressed file. The offset represents a number of bytes in the
   uncompressed data stream. The whence parameter is defined as in lseek(2);
   the value SEEK_END is not supported.
     If the file is opened for reading, this function is emulated but can be
   extremely slow. If the file is opened for writing, only forward seeks are
   supported; gise_gzseek then compresses a sequence of zeroes up to the new
   starting position.

      gise_gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error, in
   particular if the file is opened for writing and the new starting position
   would be before the current position.
*/

ZEXTERN int ZEXPORT    gise_gzrewind OF((gzFile file));
/*
     Rewinds the given file. This function is supported only for reading.

   gise_gzrewind(file) is equivalent to (int)gise_gzseek(file, 0L, SEEK_SET)
*/

ZEXTERN z_off_t ZEXPORT    gise_gztell OF((gzFile file));
/*
     Returns the starting position for the next gise_gzread or gise_gzwrite on the
   given compressed file. This position represents a number of bytes in the
   uncompressed data stream.

   gise_gztell(file) is equivalent to gise_gzseek(file, 0L, SEEK_CUR)
*/

ZEXTERN int ZEXPORT gise_gzeof OF((gzFile file));
/*
     Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/

ZEXTERN int ZEXPORT    gise_gzclose OF((gzFile file));
/*
     Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state. The return value is the zlib
   error number (see function gise_gzerror below).
*/

ZEXTERN const char * ZEXPORT gise_gzerror OF((gzFile file, int *errnum));
/*
     Returns the error message for the last error which occurred on the
   given compressed file. errnum is set to zlib error number. If an
   error occurred in the file system and not in the compression library,
   errnum is set to Z_ERRNO and the application may consult errno
   to get the exact error code.
*/

ZEXTERN void ZEXPORT gise_gzclearerr OF((gzFile file));
/*
     Clears the error and end-of-file flags for file. This is analogous to the
   clearerr() function in stdio. This is useful for continuing to read a gzip
   file that is being written concurrently.
*/

                        /* checksum functions */

/*
     These functions are not related to compression but are exported
   anyway because they might be useful in applications using the
   compression library.
*/

ZEXTERN uLong ZEXPORT gise_adler32 OF((uLong adler, const Bytef *buf, uInt len));

/*
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     uLong adler = gise_adler32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = gise_adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/

ZEXTERN uLong ZEXPORT gise_crc32   OF((uLong crc, const Bytef *buf, uInt len));
/*
     Update a running crc with the bytes buf[0..len-1] and return the updated
   crc. If buf is NULL, this function returns the required initial value
   for the crc. Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

     uLong crc = gise_crc32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = gise_crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
*/


                        /* various hacks, don't look :) */

/* gise_deflateInit and gise_inflateInit are macros to allow checking the zlib version
 * and the compiler's view of gise_z_stream:
 */
ZEXTERN int ZEXPORT gise_deflateInit_ OF((gise_z_streamp strm, int level,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT gise_inflateInit_ OF((gise_z_streamp strm,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT gise_deflateInit2_ OF((gise_z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size));
ZEXTERN int ZEXPORT gise_inflateInit2_ OF((gise_z_streamp strm, int  windowBits,
                                      const char *version, int stream_size));
ZEXTERN int ZEXPORT gise_inflateBackInit_ OF((gise_z_stream FAR *strm, int windowBits,
                                         unsigned char FAR *window,
                                         const char *version,
                                         int stream_size));
#define gise_deflateInit(strm, level) \
        gise_deflateInit_((strm), (level),       ZLIB_VERSION, sizeof(gise_z_stream))
#define gise_inflateInit(strm) \
        gise_inflateInit_((strm),                ZLIB_VERSION, sizeof(gise_z_stream))
#define gise_deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        gise_deflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy),           ZLIB_VERSION, sizeof(gise_z_stream))
#define gise_inflateInit2(strm, windowBits) \
        gise_inflateInit2_((strm), (windowBits), ZLIB_VERSION, sizeof(gise_z_stream))
#define gise_inflateBackInit(strm, windowBits, window) \
        gise_inflateBackInit_((strm), (windowBits), (window), \
        ZLIB_VERSION, sizeof(gise_z_stream))


#if !defined(ZUTIL_H) && !defined(NO_DUMMY_DECL)
    struct gise_internal_state {int dummy;}; /* hack for buggy compilers */
#endif

ZEXTERN const char   * ZEXPORT gise_zError           OF((int err));
ZEXTERN int            ZEXPORT gise_inflateSyncPoint OF((gise_z_streamp z));
ZEXTERN const uLongf * ZEXPORT gise_get_crc_table    OF((void));

#ifdef __cplusplus
}
#endif

#endif /* ZLIB_H */
