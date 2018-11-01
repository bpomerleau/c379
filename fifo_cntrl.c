/**
* fifo_cntrl.c
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "fifo_cntrl.h"
#include <stdio.h>

int get_fifo_name(char * str, int from, int to){
    snprintf(str, FILENAME_BUFFER, "fifo-%i-%i", from, to);
    return 0;
}
