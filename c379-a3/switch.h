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
    int srcIP_lo, srcIP_hi, destIP_lo, destIP_hi;
    int actionType, actionVal;
    int pri;
    int pktCount;
} Rule;

typedef struct Switch {
    int id;
    int IPlow, IPhigh;
    int ports[3];
    int n_rules;
} Switch;

void start_switch(int id,
                const char *trafficfile,
                int port1,
                int port2,
                int IPlow,
                int IPhigh,
                char *serv,
                char *port);

#endif
