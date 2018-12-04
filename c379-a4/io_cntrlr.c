/**
* io_cntrl.c
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "io_cntrlr.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
#include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/signal.h>
// #include <netdb.h>
// #include <errno.h>

int open_file(FILE **file_stream, char *filename) {
    if ((*file_stream = fopen(filename, "r")) == NULL) {perror("Opening file"); return 0;}
    else return 1;
}

void get_line(char *line, FILE *file_stream, bool *eof){
    char cpy[LINE_BUFFER];
    char *tok;
    while (fgets(line, LINE_BUFFER, file_stream) != NULL) {
        strcpy(cpy, line);
        if ((tok = strtok(cpy, " \n")) == NULL) continue;
        else if (tok[0] == '#') continue;
        else return;
    }
    *eof = true;
}

void close_file(FILE *file_stream){
    if (fclose(file_stream) < 0) perror("Closing file");
}

// void print_message(char *str, int src, int dest, int flag){
//     char source[5], destination[5];
//
//     snprintf(destination, 5, "sw%i", dest);
//     snprintf(source, 5, "sw%i", src);
//
//     if (src == 0) snprintf(source, 5, "cont");
//     if (dest == 0) snprintf(destination, 5, "cont");
//
//     if (flag == RECEIVED) printf("Received ");
//     else printf ("Transmitted ");
//
//     // print the rest of the message
//     char cpy[MAX_PACKET_LENGTH];
//     strcpy(cpy, str);
//     switch (atoi(strtok(cpy, " "))){
//
//         case OPEN:
//             printf("(src= %s, dest= %s) ", strtok(NULL, " "), destination);
//             printf("[OPEN]:\n");
//             printf("       (port0= cont, port1= %s, ", strtok(NULL, " "));
//             printf("port2= %s, ", strtok(NULL, " "));
//             printf("port3= %s", strtok(NULL, " "));
//             printf("-%s)\n", strtok(NULL, " "));
//             break;
//
//         case ACK:
//             printf("(src= %s, dest= %s) ", source, destination);
//             printf("[ACK]\n");
//             break;
//
//         case QUERY:
//             printf("(src= %s, dest= %s) ", strtok(NULL, " "), destination);
//             printf("[QUERY]:  header= (destIP= %s)\n",
//                     strtok(NULL, " "));
//             break;
//
//         case ADD:
//             printf("(src= %s, dest= %s) ", source, destination);
//             printf("[ADD]:\n"
//                     "       (srcIP= 0-1000, destIP= %s", strtok(NULL, " "));
//             printf("-%s", strtok(NULL, " "));
//             printf(", action= %s",
//                     (atoi(strtok(NULL, " ")) == FORWARD) ? "FORWARD" : "DROP");
//             printf(":%s, pri= 4, pktCount= 0)\n", strtok(NULL, " "));
//             break;
//
//         case RELAY:
//             printf("(src= %s, dest= %s) ",  source, destination);
//             printf("[RELAY]\n"
//                     "       (src= %s", strtok(NULL, " "));
//             printf(", dest= %s)\n", strtok(NULL, " "));
//             break;
//     }
// }
