#ifndef _STIMER_H
#define _STIMER_H

#include <stdint.h>
#include <sys/time.h>
/* */
void stimer_gettime(struct timeval *tv);
uint32_t stimer_deltatime(struct timeval *tv);

#endif

