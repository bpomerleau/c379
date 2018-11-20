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

#ifndef PACKET_H
#define PACKET_H

#define PCKT_BUFFER_SIZE 20

enum BUFF_OP {STORE, RM};

typedef struct packet {
    int num_fields;
    int field[6];
} Packet;

Packet gen_packet(int n, ...);
void add_field(Packet *pckt, int field);
int str2pckt(char *str, Packet *pckt);
void pckt2str(Packet pckt, char *buf);
int pckt_buf_op(int op, Packet *buf, Packet *pckt);

#endif
