#include "timespec.h"

/**
 * Set a standard format time to timespec format
 * */
struct timespec timespecFormat(int seconds, int nanoseconds)
{
    struct timespec result;

    result.tv_sec = (time_t)(seconds + (int)(nanoseconds / (int)1E9));
    result.tv_nsec = (long)(nanoseconds % (int)1E9);

    return result;
}

/**
 * Sums two timespec stuctures
 **/
struct timespec timeSum(struct timespec a, struct timespec b)
{
    struct timespec result;

    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec > (int)1E9 - 1)
    {
        result.tv_nsec = result.tv_nsec % (int)1E9;
        result.tv_sec = 1 + a.tv_sec + b.tv_sec;
    }
    else
    {
        result.tv_sec = a.tv_sec + b.tv_sec;
    }
    return result;
}

struct timespec timeDiff(struct timespec end, struct timespec start) // return ms
{
    struct timespec result;
    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        result.tv_sec = end.tv_sec - start.tv_sec - 1;
        result.tv_nsec = 1E9 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        result.tv_sec = end.tv_sec - start.tv_sec;
        result.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return result;
}

long double timeToMs(struct timespec a)
{
    return (long double)(a.tv_sec * (long double)1E3 + a.tv_nsec / (long double)1E6);
}