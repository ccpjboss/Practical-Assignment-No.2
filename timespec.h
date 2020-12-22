#include <time.h>

struct timespec timespecFormat(int seconds, int nanoseconds);
struct timespec timeSum(struct timespec a, struct timespec b);
struct timespec timeDiff(struct timespec end, struct timespec start);
long double timeToMs(struct timespec a);