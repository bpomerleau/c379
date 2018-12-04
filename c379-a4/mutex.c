/**
* packet.c
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/
#include "mutex.h"
#include "constants.h"

#include <stdlib.h>
// #include <stdarg.h>
#include <stdio.h>
// #include <string.h>
// #include <stdbool.h>

#include <pthread.h>


void init_mutex(pthread_mutex_t *mutex){
    if (pthread_mutex_init(mutex, NULL) != 0) {
        printf("Error initialising mutex\n");
        exit(EXIT_FAILURE);
    }
}

void lock(pthread_mutex_t *mutex){
    if (pthread_mutex_lock(mutex) != 0) printf("Error locking mutex\n");
    // printf("lock %lu\n", pthread_self());
}

void unlock(pthread_mutex_t *mutex){
    if (pthread_mutex_unlock(mutex) != 0) printf("Error locking mutex\n");
    // printf("unlock %lu\n", pthread_self());
}

// Packet gen_packet(int n, ...){
//     Packet pckt;
//     va_list valist;
//     pckt.num_fields = 0;
//
//     va_start(valist,n);
//
//     for (int i = 0; i < n; i++){
//         pckt.field[i] = va_arg(valist, int);
//         pckt.num_fields++;
//     }
//
//     va_end(valist);
//
//
//
//     return pckt;
// }
//
//
//
// void add_field(Packet *pckt, int field){
//     if (pckt->num_fields < 7)
//     pckt->field[pckt->num_fields++] = field;
//     return;
// }
//
//
//
//
// int str2pckt(char *str, Packet *pckt){
//
//     pckt->num_fields = 0;
//     char *tok;
//     if ( (tok = strtok(str, " \n")) == NULL) return -1;
//     pckt->field[pckt->num_fields++] = atoi(tok);
//
//     while ((tok = strtok(NULL, " \n")) != NULL) pckt->field[pckt->num_fields++] = atoi(tok);
//
//     return 0;
// }
//
//
//
//
// void pckt2str(Packet pckt, char *buf){
//     char add[MAX_PACKET_LENGTH] = "\0";
//     for (int i = 0; i < pckt.num_fields; i++) {
//         snprintf(add, MAX_PACKET_LENGTH, " %i", pckt.field[i]);
//         strcat(buf, add);
//     }
// }
//
//
//
//
// int pckt_buf_op(int op, Packet *buf, Packet *pckt){
//     // implements a FIFO ring buffer
//     static int head = 0;
//     static int tail = 0;
//     static bool full = false;
//     static bool empty = true;
//     // printf("State before exec:\n head=%i, tail=%i, full=%i, empty=%i", head, tail, full, empty);
//     switch(op) {
//         case STORE:
//         // printf("Store in buf\n");
//             if (full) return -1;
//             buf[head++] = *pckt;
//             head %= PCKT_BUFFER_SIZE;
//             empty = false;
//             full = (head == tail) ? true : false;
//             break;
//
//         case RM:
//             if (empty) return -1;
//             // printf("Remove from buf\n");
//             *pckt = buf[tail++];
//             tail %= PCKT_BUFFER_SIZE;
//             full = false;
//             empty = (head == tail) ? true : false;
//             break;
//
//         default:
//             printf("what on earth are you trying to do?");
//     }
//
//     // printf("State after exec:\n head=%i, tail=%i, full=%i, empty=%i", head, tail, full, empty);
//     return 0;
//
//
// }







//EOF
