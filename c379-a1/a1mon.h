/**
* a1mon.h
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program header
*
*/


#ifndef A1MON_H
#define A1MON_H

#define _XOPEN_SOURCE 700

#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
// #include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
// #include <sys/times.h>
#include <sys/resource.h>
// #include <time.h>
#include <unistd.h>

#define CPU_HARD_LIMIT 600 //seconds
#define CPU_SOFT_LIMIT CPU_HARD_LIMIT

#define LINE_BUFFER_SIZE 100

void sig_handler(int signal);

#endif
