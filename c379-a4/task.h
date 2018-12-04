/**
* task.h
*
* developed for CMPUT379 Assignment 4
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*/

#ifndef TASK_H
#define TASK_H

#define _XOPEN_SOURCE 700
#include "constants.h"

typedef struct Resource {
    char name[MAX_NAME_SIZE+1];
    int id;
    int maxAvail;
    int avail;
} Resource;

typedef struct Task {

    char name[MAX_NAME_SIZE+1];
    int state;
    int count;
    int busyTime;
    int idleTime;
    int totalTime;
    int waitTime;
    Resource resources[NRES_TYPES];
    int nres_types;

} Task;

enum STATES {PRE, WAIT, RUN, IDLE};

void *start_task(void *arg);

#endif
