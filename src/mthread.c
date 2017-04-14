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
 * RETURN
 *     0 on success, error code otherwise.
 */
static int mthreadMutexInit(struct mthreadMutex_t *mmutex)
{
    return pthread_mutex_init(&mmutex->mutex, NULL);
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
static int mthreadMutexDestroy(struct mthreadMutex_t *mmutex)
{
    return pthread_mutex_destroy(&mmutex->mutex);
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
static int mthreadMutexLock(struct mthreadMutex_t *mmutex)
{
    return pthread_mutex_lock(&mmutex->mutex);
}
/*
 * RETURN
 *     0 on success, error code otherwise.
 */
static int mthreadMutexUnlock(struct mthreadMutex_t *mmutex)
{
    return pthread_mutex_unlock(&mmutex->mutex);
}
/*
 *
 */
void mthreadMutexInitW(struct mthreadMutex_t *mmutex)
{
    int r;
    r = mthreadMutexInit(mmutex);
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
    r = mthreadMutexDestroy(mmutex);
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
    r = mthreadMutexLock(mmutex);
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
    r = mthreadMutexUnlock(mmutex);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Mutex unlock failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 *
 */
static int mthreadCreate(struct mthread_t *mthread, void *(*startRoutine)(void *), void *startArg)
{
    return pthread_create(&mthread->thread, NULL, startRoutine, startArg);
}
/*
 *
 */
void mthreadCreateW(
        struct mthread_t *mthread, void *(*startRoutine)(void *), void *startArg)
{
    int r;
    r = mthreadCreate(mthread, startRoutine, startArg);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Thread create failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 *
 */
static int mthreadJoin(struct mthread_t *mthread)
{
    return pthread_join(mthread->thread, NULL);
}
/*
 *
 */
void mthreadJoinW(struct mthread_t *mthread)
{
    int r;
    r = mthreadJoin(mthread);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Thread join failed, (error %d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
#if 0
/*
 *
 */
static int mthreadKill(struct mthread_t *mthread, int sig)
{
    return pthread_kill(mthread->thread, sig);
}
/*
 *
 */
void mthreadKillW(struct mthread_t *mthread, int sig)
{
    int r;
    r = mthreadKill(mthread, sig);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Thread kill failed, (%d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
#endif
/*
 *
 */
static int mthreadCancel(struct mthread_t *mthread)
{
    return pthread_cancel(mthread->thread);
}
/*
 *
 */
void mthreadCancelW(struct mthread_t *mthread)
{
    int r;
    r = mthreadCancel(mthread);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Thread cancel failed, (%d).", r);
        exit(EXIT_CODE_ERROR);
    }
}
/*
 *
 */
static int mthreadDetach(struct mthread_t *mthread)
{
    return pthread_detach(mthread->thread);
}
/*
 *
 */
void mthreadDetachW(struct mthread_t *mthread)
{
    int r;
    r = mthreadDetach(mthread);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "Thread detach failed, (%d).", r);
        exit(EXIT_CODE_ERROR);
    }
}

/*
 * Must be called in running thread. Generic thread initialization.
 *
 * ARGS
 *     cleanup    Cleanup callback.
 *     context    Argument to path to cleanup callback.
 */
void mthreadInit()
{
    int r;
    int oldstate, oldtype;

    /*
     * Enable cancelation points for functions read(), select(), ... .
     */
    r = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "%s, setcancelstate", __FUNCTION__);
    }
    r = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
    if (r != 0)
    {
        debugPrint(DLEVEL_ERROR, "%s, setcanceltype.", __FUNCTION__);
    }
}

