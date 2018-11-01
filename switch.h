/**
* switch.h
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#ifndef SWITCH_H
#define SWITCH_H

#define _XOPEN_SOURCE 700
#include "constants.h"
#include "fifo_cntrl.h"
#include <stdio.h>

typedef struct Rule {
    unsigned int srcIP_lo, srcIP_hi, destIP_lo, destIP_hi;
    int actionType, actionVal;
    int pri;
    int pktCount;
} Rule;

typedef struct Switch {
    int num;
    unsigned int IPlow, IPhigh;
    int ports[3];
    pollfd fds_rd[3];
    int fds_wr[3];
    FILE *traffic_stream;
    Rule flow_table[100];
    int n_rules;
} Switch;

void start_switch(int num,
                const char *trafficfile,
                int port1,
                int port2,
                unsigned int IPlow,
                unsigned int IPhigh);

#endif
