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
#ifdef WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
/* */
#include <debug.h>
#include <llist.h>
#include "client.h"
#include "main.h"
#include "mthread.h"

#define SERVER_LISTEN_QUEUE_LENGTH    100
#define SERVER_MAX_CLIENTS            1000

struct server_t {
    int sock;                   /* Server's socket. */
    struct llist_t *clientList; /* List of clients. */
    int numClients;             /* Number of clients in list. */

    struct mthreadMutex_t mmutex;
};

static struct server_t server;

static struct client_t * _newClient();
#ifdef WINDOWS
    static void _sigAction(int sig);
#else
    static void _sigAction(int sig, siginfo_t *siginfo, void *context);
#endif

/*
 *
 */
void serverInit()
{
    server.sock = -1;
    server.clientList = NULL;
    server.numClients = 0;
    mthreadMutexInitW(&server.mmutex);
}
/*
 *
 */
void serverRun(int portNumber)
{
    struct sockaddr_in serverAddr;

    /*
     * Hook signal handler
     */
#ifdef WINDOWS
    signal(SIGINT, _sigAction);
    signal(SIGTERM, _sigAction);
#else
    {
        static struct sigaction act;

        memset(&act, 0, sizeof(act));
        act.sa_sigaction = _sigAction;
        act.sa_flags = SA_SIGINFO | SA_RESETHAND;
        if (sigaction(SIGINT, &act, NULL) == -1)
        {
            debugPrint(DLEVEL_ERROR, "Sigaction failed, %s.", strerror(errno));
            terminate(EXIT_CODE_ERROR);
        }
        if (sigaction(SIGTERM, &act, NULL) == -1)
        {
            debugPrint(DLEVEL_ERROR, "Sigaction failed, %s.", strerror(errno));
            terminate(EXIT_CODE_ERROR);
        }
    }
#endif
    /*
     * XXX Ignore SIGPIPE.
     */
#ifndef WINDOWS
    signal(SIGPIPE, SIG_IGN);    
#endif
    /*
     * Open server socket.
     */
    server.sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server.sock < 0)
    {
        debugPrint(DLEVEL_ERROR, "Failed to open server socket, %s", strerror(errno));
        terminate(EXIT_CODE_ERROR);
    }

#if 1
    {
#ifdef WINDOWS
        int enable = 1;
        if (setsockopt(server.sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) < 0)
            debugPrint(DLEVEL_ERROR, "setsockopt(SO_REUSEADDR) failed");
#else
        int enable = 1;
        if (setsockopt(server.sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            debugPrint(DLEVEL_ERROR, "setsockopt(SO_REUSEADDR) failed");
#endif
    }
#endif

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((unsigned short)portNumber);

    if (bind(server.sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        debugPrint(DLEVEL_ERROR, "Failed to bind server to port %d, %s", portNumber, strerror(errno));
        terminate(EXIT_CODE_ERROR);
    }

    debugPrint(DLEVEL_INFO, "Binding to port %d success.", portNumber);

    if (listen(server.sock, SERVER_LISTEN_QUEUE_LENGTH) < 0)
    {
        debugPrint(DLEVEL_ERROR, "Failed to listen port %d, %s", portNumber, strerror(errno));
        terminate(EXIT_CODE_ERROR);
    }

    /*
     * Main loop.
     */
    {
        fd_set rfds;
        int sel;
        struct timeval timeout;

        while (1)
        {
            timeout.tv_sec  = 1;
            timeout.tv_usec = 0;

            FD_ZERO(&rfds);
            FD_SET(server.sock, &rfds);

            sel = select(server.sock + 1, &rfds, NULL, NULL, &timeout);
            if (sel < 0)
            {
                if (errno == EINTR)
                {
                    /* NOTREACHED _sigAction must terminate program */
#if 1
                    debugPrint(DLEVEL_ERROR, "Select EINTR %s", strerror(errno));
#endif
                }

                debugPrint(DLEVEL_ERROR, "Select failed %s", strerror(errno));
                terminate(EXIT_CODE_ERROR);
            }
            if (sel == 0)
            {
                /* timeout */
                continue;
            }
            if (FD_ISSET(server.sock, &rfds))
            {
                int clientSock;
                struct sockaddr_in addr;
                socklen_t addrLen;
                char *clientHostAddr;
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen;
                struct client_t *client;

                debugPrint(DLEVEL_NOISE, "Incoming connection...");

                client = NULL;
                do
                {
                    addrLen = sizeof(struct sockaddr_in);
                    clientSock = accept(server.sock, (struct sockaddr*)&addr, &addrLen);
                    if (clientSock < 0)
                    {
                        debugPrint(DLEVEL_WARNING, "Accept failed %s", strerror(errno));
                        break;
                    }
#if 0
                    /*
                     * Test with only one connection - drop further connecitons.
                     */
                    {
                        static int nClients = 0;
                        nClients++;
                        if (nClients > 1)
                        {
                            close(clientSock);
                            clientSock = -1;
                            break;
                        }
                    }
#endif
                    clientHostAddr = inet_ntoa(addr.sin_addr);
                    if (!clientHostAddr)
                    {
                        debugPrint(DLEVEL_WARNING, "ERROR on inet_ntoa");
                        break;
                    }
                    clientAddrLen = sizeof(clientAddr);
                    if (getpeername(clientSock, (struct sockaddr*)&clientAddr, &clientAddrLen) < 0)
                    {
                        debugPrint(DLEVEL_WARNING, "ERROR on getpeername: %s.", strerror(errno));
                        break;
                    }

                    debugPrint(DLEVEL_INFO, "Accepted connection from %s:%d", clientHostAddr, ntohs(clientAddr.sin_port));

                    client = _newClient();
                    if (!client)
                        break;
                    if (clientInit(client, clientSock, clientHostAddr, ntohs(clientAddr.sin_port)) < 0)
                    {
                        /*
                         * NOTE
                         * If init failed then client must remove self from
                         * server's list. On remove clientDestroy() will be
                         * called.
                         */
                        break;
                    }
                    debugPrint(DLEVEL_INFO, "Number of clients %d", server.numClients);
                } while (0);
                /* Close socket if error occured before client was initialized. */
                if (!client && clientSock >= 0)
                    close(clientSock);
            }
        }
    }
}
/*
 * Terminate server.
 */
void serverTerminate()
{
    debugPrint(DLEVEL_INFO, "Server terminating.");
    if (server.sock >= 0)
    {
        debugPrint(DLEVEL_INFO, "Closing server socket.");
        close(server.sock);
    }
    /*
     * Stop all clients.
     */
    mthreadMutexLockW(&server.mmutex); /* NOTE lock */
    if (server.numClients > 0)
    {
        struct llist_t *lc, *ln;

        debugPrint(DLEVEL_INFO, "Terminate clients.");
        lc = server.clientList;
        while (lc)
        {
            ln = lc->next;
            clientStop(lc->p);
            lc = ln;
        }
    }
    mthreadMutexUnlockW(&server.mmutex); /* NOTE unlock */

    /*
     * XXX Need wait for all clients termination.
     * Clients will remove self from server's list on termination.
     * Imposible to perform pthread_join, because client list will be
     * dinamicaly changed. Make simple sleep.
     */
    {
        int nwait;
        int numClients;

        nwait = 10;
        do
        {
            mthreadMutexLockW(&server.mmutex); /* NOTE lock */
            numClients = server.numClients;
            mthreadMutexUnlockW(&server.mmutex); /* NOTE unlock */
            if (numClients <= 0)
                break;
            debugPrint(DLEVEL_INFO, "Wait clients...");
#ifdef WINDOWS
            Sleep(1000);
#else
            sleep(1);
#endif
        } while (nwait-- > 0);
    }

#ifdef WINDOWS
#warning XXX REMOVE THAT
    ExitProcess(1);
#endif

    server.clientList = NULL;
    server.numClients = 0;
    mthreadMutexDestroyW(&server.mmutex);
    debugPrint(DLEVEL_INFO, "Termination complete.");
}
/*
 * Allocate new client and add it to server's list.
 *
 * RETURN
 *     Pointer to new client.
 */
static struct client_t * _newClient()
{
    struct client_t *client;
    struct llist_t *head;

    client = NULL;

    mthreadMutexLockW(&server.mmutex); /* NOTE Lock. */

    if (server.numClients >= SERVER_MAX_CLIENTS)
    {
        debugPrint(DLEVEL_WARNING,
                "Failed to allocate new client, limit reached (%d)", SERVER_MAX_CLIENTS);
        goto done;
    }

    client = malloc(sizeof(struct client_t));
    if (!client)
    {
        debugPrint(DLEVEL_WARNING,
                "Failed to allocate new client.");
        goto done;
    }

    head = llistAdd(server.clientList, client, clientDestroy, client);
    if (!head)
    {
        debugPrint(DLEVEL_WARNING,
                "Failed to add client to list.");
        free(client);
        client = NULL;
        goto done;
    }

    server.numClients++;
    server.clientList = head;
done:
    mthreadMutexUnlockW(&server.mmutex); /* NOTE Unlock. */
    return client;
}
/*
 * Called by client to remove self from server's list.
 */
void serverRemoveClient(struct client_t * client)
{
    mthreadMutexLockW(&server.mmutex); /* NOTE lock */
    server.clientList = llistRemove(
            server.clientList, llistFind(server.clientList, client));
    server.numClients--;
    debugPrint(DLEVEL_INFO, "Number of clients %d", server.numClients);
    mthreadMutexUnlockW(&server.mmutex); /* NOTE unlock */
}
/*
 * Signal handler.
 */
#ifdef WINDOWS
static void _sigAction(int sig)
#else
static void _sigAction(int sig, siginfo_t *siginfo, void *context)
#endif
{
    switch (sig)
    {
        case SIGINT:
        case SIGTERM:
            debugPrint(DLEVEL_INFO,
                    "Caught signal (signal %d).", sig);
            terminate(EXIT_CODE_SIGNAL);

            /* NOTREACHED */

            /* SA_RESETHAND specified for this handler, we can process as usual */
#ifdef WINDOWS
            /* XXX TODO */
#else
            kill(getpid(), sig);
#endif
            break;
        default:
            debugPrint(DLEVEL_WARNING,
                    "Unknown signal received (%d).", sig);
            break;
    }
}

