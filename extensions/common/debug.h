#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>

#define NL "\r\n"
#define TS "    "

#define DLEVEL_SYS     (-1)
#define DLEVEL_SILENT   0
#define DLEVEL_ERROR    1
#define DLEVEL_WARNING  2
#define DLEVEL_INFO     3
#define DLEVEL_NOISE    4

void debugPrint(int level, char *fmt, ...);
void debugPrintBuffer(int level, char *data, int size);

extern int debugLevel;

#endif

