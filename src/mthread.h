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
#ifndef _MTHREAD_H
#define _MTHREAD_H

#include <pthread.h>

struct mthreadMutex_t {
    pthread_mutex_t mutex;
};
struct mthread_t {
    pthread_t thread;
};

void mthreadMutexInitW(struct mthreadMutex_t *mutex);
void mthreadMutexDestroyW(struct mthreadMutex_t *mmutex);
void mthreadMutexLockW(struct mthreadMutex_t *mmutex);
void mthreadMutexUnlockW(struct mthreadMutex_t *mmutex);
void mthreadCreateW(
        struct mthread_t *mthread, void *(*startRoutine)(void *), void *startArg);
void mthreadJoinW(struct mthread_t *mthread);
#if 0
void mthreadKillW(struct mthread_t *mthread, int sig);
#endif
void mthreadCancelW(struct mthread_t *mthread);
void mthreadDetachW(struct mthread_t *mthread);
void mthreadInit();
#define MTHREAD_CLEANUP_PUSH(cleanup, arg) pthread_cleanup_push(cleanup, arg)
#define MTHREAD_CLEANUP_POP()              pthread_cleanup_pop(1 /* execute */)

#endif

