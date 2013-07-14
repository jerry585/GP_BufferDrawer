/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "log.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

bool gLogEnabled = false;

static FILE * logSink = 0;

void enableLog(const char *fileName) {
    if (!strcmp(fileName, "-")) {
        logSink = stderr;
    } else {
        logSink = fopen(fileName, "w");
    }
    gLogEnabled = true;
}

void disableLog() {
    gLogEnabled = false;
    if (logSink != stderr) {
        fclose(logSink);
    }
    logSink = 0;
}

void doLog(const char *format, ...) {
    va_list arglist;
    char buf[256];
    assert(strlen(format) < sizeof(buf) - 2);
    sprintf(buf, "%s\n", format);
    va_start(arglist, format);
    vfprintf(logSink, buf, arglist);
    va_end(arglist);
    fflush(logSink);
}

