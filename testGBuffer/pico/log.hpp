/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _LOG_HPP_
#define _LOG_HPP_


extern bool gLogEnabled;

void enableLog(const char *file);
void disableLog();
void doLog(const char *format, ...);

#define LOG(what) if (gLogEnabled) { doLog what; } else {}


#endif
