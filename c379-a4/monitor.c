/**
* monitor.c
*
* developed for CMPUT379 Assignment 4
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*/

#include "monitor.h"
#include "task.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>


extern Task tasks[NTASKS];
extern pthread_mutex_t mutex;
extern int num_tasks;
extern int monitorTime;

void *start_monitor(void *arg) {

    struct timespec period = {
        .tv_sec = (time_t) monitorTime/1000,
        .tv_nsec = (long) monitorTime%1000 * 1000000
    };

    char wait_str[MAX_NAME_SIZE*NTASKS];
    char run_str[MAX_NAME_SIZE*NTASKS];
    char idle_str[MAX_NAME_SIZE*NTASKS];


    while (1) {
        // reset variables
        wait_str[0] = '\0';
        run_str[0] = '\0';
        idle_str[0] = '\0';

        lock(&mutex);
        for (int i = 0; i < num_tasks; i++){
            if (tasks[i].state == WAIT) {
                strcat(wait_str, tasks[i].name);
                strcat(wait_str, " ");
            }
            if (tasks[i].state == RUN) {
                strcat(run_str, tasks[i].name);
                strcat(run_str, " ");
            }
            if (tasks[i].state == IDLE) {
                strcat(idle_str, tasks[i].name);
                strcat(idle_str, " ");
            }
        }
        unlock(&mutex);

        printf("monitor: [WAIT] %s\n"
              "         [RUN] %s\n"
              "         [IDLE] %s\n",
              wait_str, run_str, idle_str);

        nanosleep(&period, NULL);

    }
    return (void *) 0;
}

//eof
