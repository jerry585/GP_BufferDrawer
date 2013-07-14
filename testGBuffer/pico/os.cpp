/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include "os.hpp"
#include "exception.hpp"
#include <assert.h>

#if PICOSTORAGE_OS_WIN

#include "os_win.cpp"

#else

//#include "os_posix.cpp"

#endif

