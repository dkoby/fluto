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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* */
#include "client.h"
#include "common.h"
#include "debug.h"
#include "http.h"
#include "lstate.h"
#include "mthread.h"
#include "server.h"
#include "token.h"
#ifdef EXTENSION_LSQLITE3
    #include <lsqlite3.h>
#endif

#if 0
    #define DEBUG_THIS
#endif

static void * _run(void *arg);
static void _done(void *arg);
static int _service(struct client_t *client);
static int _getChars(char *buf, int len, void *arg);

/*
 * Initialize client. If init failed then client must close socket and remove
 * self from server's list.
 */
int clientInit(struct client_t *client, int sock, char *addr, int port)
{
    client->sock = sock;
    client->port = port;
    client->addr = malloc(strlen(addr) + 1 /* 0-terminator */);
    if (!client->addr)
        goto error;
    strcpy(client->addr, addr);
    /* */
    client->request.keepAlive = 0;

    DEBUG_CLIENT(DLEVEL_NOISE, "%s", "Start client");

    client->getChars = _getChars;
    if (tokenInit(&client->token, _getChars, client) != 0)
    {
        DEBUG_CLIENT(DLEVEL_ERROR, "%s", "Token init failed");
        goto error;
    }

    client->luaState = luaL_newstate();
    if (!client->luaState)
    {
        DEBUG_CLIENT(DLEVEL_ERROR, "%s", "Lua state init failed");
        goto error;
    }
    luaL_openlibs(client->luaState);
#ifdef EXTENSION_LSQLITE3
    luaopen_lsqlite3(client->luaState);
    lua_setglobal(client->luaState, "sqlite3");
#endif

    if (lstateInit0(client) < 0)
        goto error;

    mthreadCreateW(&client->mthread, _run, client);
    return 0;
error:
    serverRemoveClient(client);
    return -1;
}
/*
 * Read data from socket.
 */
static int _getChars(char *buf, int len, void *arg)
{
    return read(((struct client_t *)arg)->sock, buf, len);
}
/*
 * Called by server to stop client.
 */
void clientStop(struct client_t *client)
{
    DEBUG_CLIENT(DLEVEL_INFO, "%s", "Stopping client");
    mthreadCancelW(&client->mthread);
    mthreadJoinW(&client->mthread);
}
/*
 * Called by server when client removed from server's list.
 */
void clientDestroy(void *arg)
{
    struct client_t *client = arg;

    /* Release resource used by thread. Alternative to join. */
    mthreadDetachW(&client->mthread);

    close(client->sock);
    if (client->addr)
    {
        free(client->addr);
        client->addr = NULL;
    }
    tokenDestroy(&client->token);
    lua_close(client->luaState);

    DEBUG_CLIENT(DLEVEL_NOISE, "%s", "Client destroyed");
    free(client);
}
/*
 *
 */
static void * _run(void *arg)
{
    struct client_t *client = arg;

    mthreadInit();
    MTHREAD_CLEANUP_PUSH(_done, client);

    DEBUG_CLIENT(DLEVEL_NOISE, "%s", "Client run");
    while (_service(client))
        ;
    DEBUG_CLIENT(DLEVEL_NOISE, "%s", "Client done");

    MTHREAD_CLEANUP_POP();
    return (void*)0;
}
/*
 *
 */
static void _done(void *arg)
{
    struct client_t *client = arg;

    DEBUG_CLIENT(DLEVEL_NOISE, "%s", "Client terminating");
    serverRemoveClient(client);
}
/*
 * Service request from client.
 *
 * RETURN
 *     1 if request is complete and waiting for another request, 0 if
 *     request contain error or keep-alive does not set.
 */
static int _service(struct client_t *client)
{
    int error;

    tokenReset(&client->token);
    if (lstateInit1(client) < 0)
        return 0;
    error = httpProcessRequest(client);
    if (error < 0)
        return 0;
    if (lstateProcessRequest(client, error))
    {
        if (client->request.keepAlive)
            return 1;
        else
            return 0;
    }
    return 0;
}

