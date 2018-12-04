/**
* packet.h
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

void init_mutex(pthread_mutex_t *mutex);
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);
// #define PCKT_BUFFER_SIZE 20

// enum BUFF_OP {STORE, RM};

// typedef struct packet {
//     int num_fields;
//     int field[6];
// } Packet;
//
// Packet gen_packet(int n, ...);
// void add_field(Packet *pckt, int field);
// int str2pckt(char *str, Packet *pckt);
// void pckt2str(Packet pckt, char *buf);
// int pckt_buf_op(int op, Packet *buf, Packet *pckt);

#endif
