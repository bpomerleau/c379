/**
* switch.h
*
* developed for CMPUT379 Assignment 3
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*
*/

#ifndef SOCKET_CNTRL_H
#define SOCKET_CNTRL_H

#define _XOPEN_SOURCE 700
#include "constants.h"

void setup_server(int *sockfd, char *port);
void setup_client(int *sockfd, char *server_address, char *port);
int read_from_socket(int sockfd, char *str);
int write_to_socket(int sockfd, char *str);

    #endif
