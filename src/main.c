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
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
/* */
#include "debug.h"
#include "main.h"
#include "server.h"
#include "version.h"

#define DEFAULT_PORT_NUMBER    8080
static int portNumber = DEFAULT_PORT_NUMBER;
static char *user = NULL;
static char *chDir = NULL;
static char *chRoot = NULL;
struct passwd *pwd = NULL;;

static void _printHead()
{
    debugPrint(DLEVEL_SYS, "Fluto v." VERSION_STRING " (c) 2016-2017 Dmitry Kobylin.");
    debugPrint(DLEVEL_SYS, "");
}
static void _printHelp()
{
    debugPrint(DLEVEL_SYS, "Usage:");
    debugPrint(DLEVEL_SYS, "    fluto <OPTIONS>");
    debugPrint(DLEVEL_SYS, "");
    debugPrint(DLEVEL_SYS, "Options:");
    debugPrint(DLEVEL_SYS, "    -h          Print this help.");
    debugPrint(DLEVEL_SYS, "    -p<Port>    Bind to port.");
    debugPrint(DLEVEL_SYS, "    -r=<Dir>    Do chroot to directory.");
    debugPrint(DLEVEL_SYS, "    -D=<Dir>    Change to directory.");
    debugPrint(DLEVEL_SYS, "    -u<User>    User to change to after startup.");
    debugPrint(DLEVEL_SYS, "    -d<L>       Debug level (0, 1, 2, 3, 4), default 3");
    debugPrint(DLEVEL_SYS, "                    0    SILENT ");
    debugPrint(DLEVEL_SYS, "                    1    ERROR  ");
    debugPrint(DLEVEL_SYS, "                    2    WARNING");
    debugPrint(DLEVEL_SYS, "                    3    INFO   ");
    debugPrint(DLEVEL_SYS, "                    4    NOISE  ");
    debugPrint(DLEVEL_SYS, "");
    debugPrint(DLEVEL_SYS, "");

    exit(EXIT_CODE_ERROR);
}
static void _getPwd()
{
    if (getuid() == 0)
    {
        pwd = getpwnam(user);
        if (!pwd)
        {
            debugPrint(DLEVEL_ERROR, "Unknown user \"%s\".", user);
            exit(EXIT_CODE_ERROR);
        }
    }
}
static void _changeUser()
{
    if (!pwd)
        return;

    if (setgroups(0, NULL) < 0)
    {
        debugPrint(DLEVEL_ERROR, "setgroups failed for user \"%s\".", user);
        exit(EXIT_CODE_ERROR);
    }
    if (setgid(pwd->pw_gid) < 0)
    {
        debugPrint(DLEVEL_ERROR, "setgid failed for user \"%s\".", user);
        exit(EXIT_CODE_ERROR);
    }
    if (initgroups(user, pwd->pw_gid) < 0)
    {
        debugPrint(DLEVEL_WARNING, "initgroups failed for user \"%s\".", user);
    }
    if (setuid(pwd->pw_uid) < 0)
    {
        debugPrint(DLEVEL_ERROR, "setuid failed for user \"%s\".", user);
        exit(EXIT_CODE_ERROR);
    }
}
static void _changeRoot()
{
    if (!chRoot)
        return;
    if (chroot(chRoot) < 0)
    {
        debugPrint(DLEVEL_ERROR, "Failed to chroot, dir \"%s\": %s.", chRoot, strerror(errno));
        terminate(EXIT_CODE_ERROR);
    } else {
        debugPrint(DLEVEL_INFO, "Do chroot to \"%s\".", chRoot);
    }
}
static void _changeDir()
{
    if (!chDir)
        return;
    if (chdir(chDir) < 0)
    {
        debugPrint(DLEVEL_ERROR, "Failed to change directory to \"%s\": %s.", chDir, strerror(errno));
        terminate(EXIT_CODE_ERROR);
    } else {
        debugPrint(DLEVEL_INFO, "Change directory to \"%s\".", chDir);
    }
}
/*
 *
 */
int main(int argc, char **argv)
{
    char **arg;

    serverInit();

    _printHead();

    arg = argv;
    while (argc--)
    {
        if (strcmp("-h", *arg) == 0 || strcmp("--help", *arg) == 0)
        {
            _printHelp();
        } else if (strlen(*arg) == 3 && strncmp("-d", *arg, 2) == 0) {
            char *c;
            c = *arg + 2;
            if (*c < '0' || *c > '4')
            {
                debugPrint(DLEVEL_ERROR, "Invalid value of \"-d\" option.");
                terminate(EXIT_CODE_ERROR);
            }
            debugLevel = *c - '0';
        } else if (strlen(*arg) >= 3 && strncmp("-p", *arg, 2) == 0) {
            char *c;
            c = *arg + 2;
            portNumber = atoi((const char*)c);
            if (portNumber <= 0)
            {
                debugPrint(DLEVEL_ERROR, "Invalid value of \"-p\" option.");
                terminate(EXIT_CODE_ERROR);
            }
        } else if (strlen(*arg) >= 4 && strncmp("-r=", *arg, 3) == 0) {
            chRoot = *arg + 3;
        } else if (strlen(*arg) >= 4 && strncmp("-D=", *arg, 3) == 0) {
            chDir = *arg + 3;
        } else if (strlen(*arg) >= 3 && strncmp("-u", *arg, 2) == 0) {
            user = *arg + 2;
        }

        arg++;
    }

    _getPwd();
    _changeRoot();
    _changeDir();
    _changeUser();

    serverRun(portNumber);

    terminate(EXIT_CODE_SUCCESS);
    return 0;
}
/*
 *
 */
void terminate(int code)
{
    serverTerminate();
    exit(code);
}

