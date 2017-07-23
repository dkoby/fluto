/*
 *
 */
#include <stdlib.h>
/* */
#include "stimer.h"
/*
 *
 */
void stimer_gettime(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}
/*
 * Return delta in ms between current time and timeval given in argument.
 */
uint32_t stimer_deltatime(struct timeval *tv)
{
    uint32_t ret;

    struct timeval ctv;
    gettimeofday(&ctv, NULL);

    ret  = (ctv.tv_sec * 1000 + ctv.tv_usec / 1000);
    ret -= (tv->tv_sec * 1000 + tv->tv_usec / 1000);

    return ret;
}

