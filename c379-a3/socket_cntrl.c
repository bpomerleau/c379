
#include "socket_cntrl.h"

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

        if (connect(controller->fd, p->ai_addr, p->ai_addrlen) == -1) {
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

int read_from_socket(int sockfd, char *str){
    return 0;
}

int write_to_socket(int sockfd, char *str){
    return 0;
}
