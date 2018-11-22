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

#include "io_cntrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netdb.h>
#include <errno.h>

enum PRINT_FLAG {RECEIVED, TRANSMITTED};


int get_fifo_name(char * str, int from, int to){
    snprintf(str, FILENAME_BUFFER, "fifo-%i-%i", from, to);
    return 0;
}

void write_to_fifo(int fd, char *str, int src, int dest) {
    if (write(fd, str, strlen(str)+1) == -1) perror("write error on fifo");
    print_message(str, src, dest, TRANSMITTED);
    return;
}

int read_from_fifo(int fd, char *buf, int src, int dest){
    char c = '0';
    int n = 0;
    while (c != '\0') {
        if (read(fd, &c, 1) <= 0) return -1;//EOF or error, don't use buf;
        buf[n++] = c;
    }
    print_message(buf, src, dest, RECEIVED);
    return 0;
}

void setup_server(int *sockfd, char *port){
    // code for a server waiting for connections
    // adapted from https://beej.us/guide/bgnet/html/multi/getaddrinfoman.html

    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
                perror("socket");
                continue;
            }

            if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(*sockfd);
                perror("bind");
                continue;
            }

            break; // if we get here, we must have connected successfully
        }

        if (p == NULL) {
            // looped off the end of the list with no successful bind
            fprintf(stderr, "failed to bind socket\n");
            exit(2);
        }

        freeaddrinfo(servinfo); // all done with this structure
    }

void setup_client(int *sockfd, char *server_address, char *port){
    // adapted from https://beej.us/guide/bgnet/html/multi/getaddrinfoman.html
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server_address, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(*sockfd);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no connection
        fprintf(stderr, "failed to connect\n");
        exit(2);
    }

    freeaddrinfo(servinfo); // all done with this structure

}

int read_from_socket(int sockfd, char *str, int src, int dest){
    int retval = recv(sockfd, str, MAX_PACKET_LENGTH, MSG_DONTWAIT);
    if (retval > 0){
        print_message(str, src, dest, RECEIVED);
    }
    else if (retval < 0){
        if (errno == SIGPIPE) printf("Lost connection to sw%i.\n", src);
    }
    return retval;
}

void write_to_socket(int sockfd, char *str, int src, int dest){
    if (send(sockfd, str, strlen(str)+1, MSG_DONTWAIT) == -1) {
        if (errno == SIGPIPE) printf("Lost connection to sw%i.\n", dest);
        return;
    }
    print_message(str, src, dest, TRANSMITTED);
    return;
}

void print_message(char *str, int src, int dest, int flag){
    char source[5], destination[5];

    snprintf(destination, 5, "sw%i", dest);
    snprintf(source, 5, "sw%i", src);

    if (src == 0) snprintf(source, 5, "cont");
    if (dest == 0) snprintf(destination, 5, "cont");

    if (flag == RECEIVED) printf("Received ");
    else printf ("Transmitted ");

    // print the rest of the message
    char cpy[MAX_PACKET_LENGTH];
    strcpy(cpy, str);
    switch (atoi(strtok(cpy, " "))){

        case OPEN:
            printf("(src= %s, dest= %s) ", strtok(NULL, " "), destination);
            printf("[OPEN]:\n");
            printf("       (port0= cont, port1= %s, ", strtok(NULL, " "));
            printf("port2= %s, ", strtok(NULL, " "));
            printf("port3= %s", strtok(NULL, " "));
            printf("-%s)\n", strtok(NULL, " "));
            break;

        case ACK:
            printf("(src= %s, dest= %s) ", source, destination);
            printf("[ACK]\n");
            break;

        case QUERY:
            printf("(src= %s, dest= %s) ", strtok(NULL, " "), destination);
            printf("[QUERY]:  header= (destIP= %s)\n",
                    strtok(NULL, " "));
            break;

        case ADD:
            printf("(src= %s, dest= %s) ", source, destination);
            printf("[ADD]:\n"
                    "       (srcIP= 0-1000, destIP= %s", strtok(NULL, " "));
            printf("-%s", strtok(NULL, " "));
            printf(", action= %s",
                    (atoi(strtok(NULL, " ")) == FORWARD) ? "FORWARD" : "DROP");
            printf(":%s, pri= 4, pktCount= 0)\n", strtok(NULL, " "));
            break;

        case RELAY:
            printf("(src= %s, dest= %s) ",  source, destination);
            printf("[RELAY]\n"
                    "       (src= %s", strtok(NULL, " "));
            printf(", dest= %s)\n", strtok(NULL, " "));
            break;
    }
}
