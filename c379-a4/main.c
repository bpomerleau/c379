/**
* main.c
*
* developed for CMPUT379 Assignment 2 - a2sdn
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "main.h"
#include "task.h"
#include "monitor.h"
#include "io_cntrlr.h"
#include "mutex.h"
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
// #include <stdio.h>
// #include <stdbool.h>
#include <pthread.h>

#include <unistd.h>

void parse_file(char *filename);
void parse_line(char *line);
void print_info();

Resource resources[NRES_TYPES];
Task tasks[NTASKS];
pthread_mutex_t mutex;
pthread_t tids[NTASKS+1];
int num_res_types;
int num_tasks;
int niter;
int monitorTime;
clock_t start_time;

int main(int argc, char *argv[]){
    // a4tasks inputFile monitorTime NITER
    if (argc == 4 ) {

        // init globals
        num_res_types = 0;
        num_tasks = 0;
        niter =
        start_time = times(NULL);
        monitorTime = atoi(argv[2]);
        niter = atoi(argv[3]);
        init_mutex(&mutex);

        parse_file(argv[1]);
        // start tasks with NITER, reserve tids[0] for monitor
        for (int i = 1; i <= num_tasks; i++){
            if (pthread_create(&tids[i], NULL, start_task, (void *) &niter) != 0)
                exit(EXIT_FAILURE);
        }
        //start monitor with monitorTime
        if (pthread_create(&tids[0], NULL, start_monitor, (void *) &monitorTime) != 0)
            exit(EXIT_FAILURE);

        // wait for threads to finish execution
        for (int i = 1; i <= num_tasks; i++){
            if (pthread_join(tids[i], NULL) != 0) {
                printf("Error joining thread");
                exit(EXIT_FAILURE);
            }
        }

        // cancel monitor
        if (pthread_cancel(tids[0]) != 0) printf("Problem cancelling thread.\n");


        // print exit information
        print_info();


    }
    else printf("3 input arguments expected.");
}

/**
*   Only safe to call before multi-threading begun
*/
void parse_file(char *filename){
    FILE *file_stream;
    if (!open_file(&file_stream, filename)) return;

    bool endoffile = false;
    while (true){
        char line[LINE_BUFFER];
        get_line(line, file_stream, &endoffile);
        if (endoffile) {close_file(file_stream); return;}
        else parse_line(line);
    }
}

void parse_line(char * line) {
    char *tok;
    tok = strtok(line, " \n");
    if (strcmp(tok, "resources") == 0) {
        char *res_val[NRES_TYPES];
        while ((tok = strtok(NULL, " ")) != NULL) {
            res_val[num_res_types++] = tok;
        }
        for (int i = 0; i < num_res_types; i++){
            strcpy(resources[i].name, strtok(res_val[i], ":"));
            resources[i].id = i;
            resources[i].maxAvail = atoi(strtok(NULL, ":"));
            resources[i].avail = resources[i].maxAvail;
        }
    }
    else if (strcmp(tok, "task") == 0){
        strcpy(tasks[num_tasks].name, strtok(NULL, " "));
        tasks[num_tasks].state = PRE;
        tasks[num_tasks].count = 0;
        tasks[num_tasks].busyTime = atoi(strtok(NULL, " "));
        tasks[num_tasks].idleTime = atoi(strtok(NULL, " "));
        tasks[num_tasks].totalTime = 0;
        tasks[num_tasks].waitTime = 0;
        tasks[num_tasks].nres_types = 0;
        char *res_val[NRES_TYPES];
        while ((tok = strtok(NULL, " ")) != NULL) {
            res_val[tasks[num_tasks].nres_types++] = tok;
        }
        for (int i = 0; i < tasks[num_tasks].nres_types; i++){
            strcpy(tasks[num_tasks].resources[i].name, strtok(res_val[i], ":"));
            tasks[num_tasks].resources[i].maxAvail = atoi(strtok(NULL, ":"));
            tasks[num_tasks].resources[i].avail = 0;
            for (int j = 0; j < num_res_types; j++){
                if (strcmp(tasks[num_tasks].resources[i].name, resources[j].name) == 0){
                    tasks[num_tasks].resources[i].id = resources[j].id;
                }
            }
        }
        num_tasks++;
    }
}

void print_info(){
    printf("System Resources:\n");
    for (int i = 0; i < num_res_types; i++){
        printf("%s: (maxAvail=%5d, held= %5d)\n",
                resources[i].name, resources[i].maxAvail,
                resources[i].maxAvail - resources[i].avail);
    }
    printf("\nSystem Tasks:\n");
    for (int i = 0; i < num_tasks; i++){
        printf("[%i] %s (%s, runTime= %i, idleTime= %i):\n"
               "\t(tid= %lu)\n",
                i, tasks[i].name,
                (tasks[i].state == PRE) ? "PRE" :   // sorta ugly I know, but also sorta fun
                (tasks[i].state == WAIT) ? "WAIT" :
                (tasks[i].state == RUN) ? "RUN" :
                (tasks[i].state == IDLE) ? "IDLE" : "What else do you want??",
                tasks[i].busyTime, tasks[i].idleTime,
                tids[i]
                );

        for (int j = 0; j < tasks[i].nres_types; j++){
            printf("\t\t%s: (needed= %5d, held= %5d)\n",
                    tasks[i].resources[j].name,
                    tasks[i].resources[j].maxAvail,
                    tasks[i].resources[j].avail
                    );
        }
        printf("\tRUN: %i times, WAIT: %i msec)\n",
                tasks[i].count, tasks[i].waitTime);
    }
    printf("Running time= %5d msec\n", (int) ((times(NULL) - start_time)*MSEC_PER_TICK));
}


// start timer if delay message
// if (strcmp(strtok(NULL, " "), "delay") == 0){
//     delay = true;
//     int millis = atoi(strtok(NULL, " "));
//     struct timeval time = {
//         .tv_sec = (time_t) millis/1000,
//         .tv_usec = (suseconds_t) millis%1000 * 1000
//     };
//     struct timeval interval = {
//         .tv_sec = 0,
//         .tv_usec = 0
//     };
//     struct itimerval itimer = {
//         .it_interval = interval,
//         .it_value = time
//     };
//     setitimer(ITIMER_REAL, &itimer, NULL);
//     printf("Entering a delay period for %i millisec.\n", millis);
//     continue;
