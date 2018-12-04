#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unistd.h>

#define NRES_TYPES 10
#define NTASKS 25
#define MAX_NAME_SIZE 32
#define LINE_BUFFER 1500
#define TICKS_PER_SEC (sysconf(_SC_CLK_TCK))
#define MSEC_PER_TICK (long int)((1/(float)TICKS_PER_SEC)*1000)

#endif
