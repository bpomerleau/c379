/**
* io_cntrl.h
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/
#ifndef IOCNTRL_H
#define IOCNTRL_H

#define _XOPEN_SOURCE 700
#include "constants.h"
#include <stdio.h>
#include <stdbool.h>
#include <poll.h>

int open_file(FILE **file_stream, char *filename);
void get_line(char *line, FILE *file_stream, bool *eof);
void close_file(FILE *file_stream);
void print_message(char *str, int src, int dest, int flag);

#endif
