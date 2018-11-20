/**
* a1jobs.h
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program header
*
*/

#ifndef A1JOBS_H
#define A1JOBS_H

#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#define MAXJOBS 32

/*--- type declarations --------------------------------------------------- */

typedef struct job {
    int index;
    int pid;
    char command[100];
    int state;
} JOB;

enum pstate{running, suspended, terminated};

/*--- function declarations ------------------------------------------------*/

void setup(const struct rlimit *new_limit, struct tms *tms_buf, clock_t *wall_before);
void print_jobs(JOB jobs[], int num_jobs);
void run_command(char *out_str[], int i );

#endif
