/**
* fifo_cntrl.h
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#ifndef FIFOCNTRL_H
#define FIFOCNTRL_H

#include "constants.h"
#include <poll.h>

typedef struct pollfd pollfd;

int get_fifo_name(char * str, int from, int to);
void write_tofifo(int fd, char *str);
int read_fromfifo(int fd, char *buf);

#endif
