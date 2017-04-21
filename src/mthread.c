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
#include <signal.h>
#include <pthread.h>
/* */
#include "debug.h"
#include "main.h"
#include "mthread.h"


/*
 *
 */
void mthreadMutexInitW(struct mthreadMutex_t *mmutex)
{
    int r;
    r = pthread_mutex_init(&mmutex->mutex, NULL);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Mutex init failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
void mthreadMutexDestroyW(struct mthreadMutex_t *mmutex)
{
    int r;
    r = pthread_mutex_destroy(&mmutex->mutex);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Mutex destroy failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
void mthreadMutexLockW(struct mthreadMutex_t *mmutex)
{
    int r;
    r = pthread_mutex_lock(&mmutex->mutex);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Mutex lock failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
void mthreadMutexUnlockW(struct mthreadMutex_t *mmutex)
{
    int r;
    r = pthread_mutex_unlock(&mmutex->mutex);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Mutex unlock failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}

