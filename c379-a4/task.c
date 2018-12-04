/**
* task.c
*
* developed for CMPUT379 Assignment 4 - a4tasks
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*/

#include "task.h"
#include "io_cntrlr.h"
#include "mutex.h"
#include <stdlib.h>
#include <sys/times.h>

extern Resource resources[NRES_TYPES];
extern Task tasks[NTASKS];
extern pthread_mutex_t mutex;
extern int num_tasks;
extern int niter;
extern clock_t start_time;

int get_resources(int task);
void release_resources(int task);

void *start_task(void *arg){
    int mytask = NTASKS;
    clock_t wait_start;
    int count;

    lock(&mutex);
    // choose a task to simulate
    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].state == PRE) {
            mytask = i;
            break;
        }
    }
    if (mytask == NTASKS) {
        printf("Error, unassigned thread.\n");
        exit(EXIT_FAILURE);
    }
    struct timespec busytime = {
        .tv_sec = (clock_t) tasks[mytask].busyTime/1000,
        .tv_nsec = (long) tasks[mytask].busyTime%1000 * 1000000
    };
    struct timespec idletime = {
        .tv_sec = (clock_t) tasks[mytask].idleTime/1000,
        .tv_nsec = (long) tasks[mytask].idleTime%1000 * 1000000
    };
    count = tasks[mytask].count;
    tasks[mytask].state = WAIT;
    unlock(&mutex);
    wait_start = times(NULL);

    // main simulation loop
    while (count != niter){

        lock(&mutex);
        tasks[mytask].state = WAIT;
        unlock(&mutex);

        if (get_resources(mytask) != 0) continue;

        lock(&mutex);
        tasks[mytask].waitTime += (int) ((times(NULL) - wait_start)*MSEC_PER_TICK);
        tasks[mytask].state = RUN;
        unlock(&mutex);

        // printf("%lu sleeping %i\n",pthread_self(), nanosleep(&busytime, NULL));
        nanosleep(&busytime, NULL);
        // printf("task %lu got this far\n", pthread_self());

        lock(&mutex);
        count = ++tasks[mytask].count;
        printf("task: %s (tid= %lu, iter= %i, time= %i msec)\n",
                tasks[mytask].name, pthread_self(),
                tasks[mytask].count, (int) ((times(NULL) - start_time)*MSEC_PER_TICK));
        tasks[mytask].state = IDLE;
        release_resources(mytask);
        unlock(&mutex);


        nanosleep(&idletime, NULL);

        wait_start = times(NULL);

    }

    return (void *) 0;
}

/*
* DO NOT call with lock ON
*/
int get_resources(int task) {
    lock(&mutex);
    for (int i = 0; i < tasks[task].nres_types; i++) {
        int id = tasks[task].resources[i].id;
        if (resources[id].avail >= tasks[task].resources[i].maxAvail){
            resources[id].avail -= tasks[task].resources[i].maxAvail;
            tasks[task].resources[i].avail = tasks[task].resources[i].maxAvail;
        }
        else {
            release_resources(task);
            unlock(&mutex);
            return 1;
        }
    }
    unlock(&mutex);
    return 0;
}

/*
* ONLY call with lock ON
*/
void release_resources(int task) {
    for (int i = 0; i < tasks[task].nres_types; i++){
        int id = tasks[task].resources[i].id;
        resources[id].avail += tasks[task].resources[i].avail;
        tasks[task].resources[i].avail = 0;
    }
}



//eof
