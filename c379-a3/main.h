/**
* main.h
*
* developed for CMPUT379 Assignment 2 - a2sdn
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program header
*
*/

#ifndef MAIN_H
#define MAIN_H

#define _XOPEN_SOURCE 700
#include "controller.h"
#include "switch.h"
#include "constants.h"

/*--- type declarations --------------------------------------------------- */

typedef struct job {
    int index;
    int pid;
    char command[100];
    int state;
} JOB;

enum pstate{running, suspended, terminated};

/*--- function declarations ------------------------------------------------*/

// void setup(const struct rlimit *new_limit, struct tms *tms_buf, clock_t *wall_before);
// void print_jobs(JOB jobs[], int num_jobs);
// void run_command(char *out_str[], int i );

#endif
