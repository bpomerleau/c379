/**
* io_cntrl.h
*
* developed for CMPUT379 Assignment 4
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*/
#ifndef IOCNTRLR_H
#define IOCNTRLR_H

#define _XOPEN_SOURCE 700
#include "constants.h"
#include <stdio.h>
#include <stdbool.h>

int open_file(FILE **file_stream, char *filename);
void get_line(char *line, FILE *file_stream, bool *eof);
void close_file(FILE *file_stream);

#endif
