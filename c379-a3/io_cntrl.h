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
#include <poll.h>

typedef struct pollfd pollfd;
int get_fifo_name(char * str, int from, int to);
void write_to_fifo(int fd, char *str, int src, int dest);
int read_from_fifo(int fd, char *buf, int src, int dest);
void setup_server(int *sockfd, char *port);
void setup_client(int *sockfd, char *server_address, char *port);
int read_from_socket(int sockfd, char *str, int src, int dest);
void write_to_socket(int sockfd, char *str, int src, int dest);
void print_message(char *str, int src, int dest, int flag);

#endif
