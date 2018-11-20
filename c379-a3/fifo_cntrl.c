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
#include <unistd.h>
#include <string.h>

int get_fifo_name(char * str, int from, int to){
    snprintf(str, FILENAME_BUFFER, "fifo-%i-%i", from, to);
    return 0;
}

void write_tofifo(int fd, char *str) {
    printf("Transmitting: %s\n", str);
    if (write(fd, str, strlen(str)+1) == -1) perror("write error on fifo");
    return;
}

int read_fromfifo(int fd, char *buf){
    char c = '0';
    int n = 0;
    while (c != '\0') {
        if (read(fd, &c, 1) <= 0) return -1;//EOF or error, don't use buf;
        buf[n++] = c;
    }
    printf("Received: %s\n", buf);
    return 0;
}
