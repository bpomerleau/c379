/**
* controller.c
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "controller.h"
#include "fifo_cntrl.h"
#include "socket_cntrl.h"
#include "packet.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>

typedef struct Counters{
    int nSwitch;
    int connected_switches;
    int open_switches;
    int ack;
    int query;
    int add;
} Counters;

void init_controller(int n, pollfd *keyboard, pollfd *l_sock, char *port);
void print_controller();
void ctrl_sig_handler(int signal);
void answer_query(Packet rcv_pckt);

/* ---- global data ------------------*/
static Counters counters;
static Switch switches[MAX_NSW];
static pollfd sockets[MAX_NSW];



void start_controller(int nSwitch, char *port) {

    pollfd listen_sock;
    pollfd keyboard;
    Packet pckt_buf[PCKT_BUFFER_SIZE];

    init_controller(nSwitch, &keyboard, &listen_sock, port);


    while (1){

        // i. poll listening socket
        if (poll(&listen_sock, 1, 0) > 0) {
            if ( (sockets[counters.connected_switches].fd = accept(listen_sock.fd, NULL, NULL)) < 0)
                perror("Accept connection error");
            sockets[counters.connected_switches++].events = POLLIN;
        }

        // 1. poll keyboard
        char input[LINE_BUFFER];
        ssize_t n_read;
        if (poll(&keyboard, 1, 0) > 0) {
            if ((n_read = read(STDIN_FILENO, input, LINE_BUFFER)) <= 0){} //TODO: EOF or error, deal with either
            else if (n_read == 5){ //only interested in "list" and "exit"
                if (strncmp(input, "list", 4) == 0) print_controller();
                else if (strncmp(input, "exit", 4) == 0) {print_controller(); exit(EXIT_SUCCESS);}
            }
        }

        // 2. poll sockets
        if (poll(sockets, nSwitch, 0) > 0) {
            for (int i = 0; i < nSwitch; i++) {
                if (sockets[i].revents & POLLIN) {
                    if (read_from_socket(sockets[i].fd, input) == 0){
                        //handle various cases
                        Packet rcv_pckt;
                        if ( str2pckt(input, &rcv_pckt) == -1) continue;
                        switch (rcv_pckt.field[0]) {

                            case OPEN:

                                counters.open_switches++;

                                switches[i].id = rcv_pckt.field[1];
                                switches[i].ports[0] = rcv_pckt.field[2];
                                switches[i].ports[1] = rcv_pckt.field[3];
                                switches[i].IPlow = rcv_pckt.field[4];
                                switches[i].IPhigh = rcv_pckt.field[5];
                                //send ACK
                                char str[MAX_PACKET_LENGTH] = "\0";
                                snprintf(str, MAX_PACKET_LENGTH, "%i", ACK);
                                write_to_socket(sockets[i].fd, str);
                                counters.ack++;

                                if (counters.open_switches == nSwitch) {
                                    Packet backlog_pckt;
                                    while ( pckt_buf_op(RM, pckt_buf, &backlog_pckt) == 0 ){
                                        char str[MAX_PACKET_LENGTH] = "\0";
                                        pckt2str(backlog_pckt, str);
                                        printf("doing_backlog:\n %s\n", str);
                                        answer_query(backlog_pckt);
                                    }
                                }

                                break;

                            case QUERY:

                                counters.query++;
                                if (counters.open_switches < nSwitch) {
                                    pckt_buf_op(STORE, pckt_buf, &rcv_pckt);
                                    continue;
                                }
                                answer_query(rcv_pckt);
                                break;

                            default:
                                    printf("Poop, something's fooey!");
                        }
                    }
                }
            }
        }
    }
}

void init_controller(int n, pollfd *keyboard, pollfd *l_sock, char *port)
{
    // initialise global Counters
    counters.nSwitch = n;
    counters.connected_switches = 0;
    counters.open_switches=0;
    counters.ack = 0;
    counters.query = 0;
    counters.add = 0;
    //open sockets for each switch
    setup_server(&(l_sock->fd), port);

    if (listen(l_sock->fd, n) < 0) perror("Listen error");

    // set up keyboard polling
    keyboard->fd = STDIN_FILENO;
    keyboard->events = POLLIN;

        //TODO: only complete intialisation after all switches connected

}

void print_controller() {
    printf("Switch information (nSwitch=%i):\n", counters.nSwitch);
    for (int i = 0; i < counters.nSwitch; i++){
        printf("[sw%i] port1= %i, port2= %i, port3= %u-%u\n",
        switches[i].id, switches[i].ports[0], switches[i].ports[1], switches[i].IPlow, switches[i].IPhigh);
    }
    printf("\n");
    printf("Packet Stats:\n"
        "Received:    OPEN:%i, QUERY:%i\n"
        "Transmitted: ACK:%i, ADD:%i\n\n",
        counters.open_switches,counters.query,counters.ack,counters.add);
}

void answer_query(Packet rcv_pckt){
    int destIP = rcv_pckt.field[2];
    int i = rcv_pckt.field[1] - 1;

    int target_switch = -1;
    for (int j = 0; j < counters.nSwitch; j++){
        if (destIP >= switches[j].IPlow && destIP <= switches[j].IPhigh) {
            target_switch = j;
        }
    }
    int range_low = (target_switch >= 0) ?
                    switches[target_switch].IPlow : destIP;
    int range_high = (target_switch >= 0) ?
                    switches[target_switch].IPhigh : destIP;
    int action = (target_switch >= 0) ? FORWARD : DROP;
    int port;
    if (action == FORWARD) port = (target_switch < i) ? 1 : 2;
    else port = 0;

    Packet snd_pckt = gen_packet(6, ADD, range_low, range_high, action, port, MIN_PRI);
    char snd_str[MAX_PACKET_LENGTH] = "\0";
    pckt2str(snd_pckt, snd_str);
    write_to_socket(sockets[i].fd, snd_str);
    counters.add++;
}






//eof
