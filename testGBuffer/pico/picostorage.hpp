/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _PICOSTORAGE_H_
#define _PICOSTORAGE_H_

#include "dir.hpp"
#include "file.hpp"

   GIS_API bool createStorage(const char *path, int compressFlags = kCompressAll, bool safeCommit = true);
   GIS_API Dir openStorage(const char *path, int cacheSizeNbPages = 50);

#endif
