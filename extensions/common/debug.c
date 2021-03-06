/*
 * Fluto - the web server.
 *
 * Copyright (c) 2016-2017, Dmitry Kobylin
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

int debugLevel = DLEVEL_INFO;

static const char* prefix[] = {
    "SILENT : ",
    "ERROR  : ",
    "WARNING: ",
    "INFO   : ",
    "NOISE  : ",
};

/*
 *
 */
void debugPrint(int level, char *fmt, ...)
{
    va_list ap;

    if (level != DLEVEL_SYS && level > debugLevel)
        return;

    if (level != DLEVEL_SYS)
        printf("(%d)%s", level, prefix[level]);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf(NL);
}
/*
 *
 */
void debugPrintBuffer(int level, char *data, int size)
{
    int i;

    if (level != DLEVEL_SYS && level > debugLevel)
        return;

#define _SPLIT  16
    for (i = 0; i < size; i++)
    {
        if ((i % _SPLIT) == 0)
        {
            if (i > 0)
                printf(NL);
            if (level != DLEVEL_SYS)
            {
                printf("(%d)%s%04X: ", level, prefix[level], i);
            } else {
                printf("%04X: ", i);
            }
        } else if ((i % (_SPLIT / 2)) == 0) {
            printf(" ");
        }
        printf("%02hhX ", *data++);
    }

    if (size != _SPLIT)
        printf(NL);
}

