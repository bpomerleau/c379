/**
* switch.c
*
* developed for CMPUT379 Assignment 2
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "switch.h"
#include "packet.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>
#include <stdbool.h>

void sw_sig_handler(int signal);
void print_switch();
void init_switch(struct sigaction *sig_struct, const char *trafficfile,
                    FILE *traffic_stream, pollfd *keyboard);
bool match_rule( int destIP, int *actiontype, int *actionval);
void perform_action(Packet pckt, int actiontype, int actionval);
void query_controller(Packet pckt);
void relay(Packet pckt);
void do_backlog();

/* --GLOBAL data structs-----------*/
static Switch self;
static pollfd fds_rd[3];
static int fds_wr[3];
static Rule flow_table[100];
static int n_rules;
static Packet backlog[PCKT_BUFFER_SIZE];

/* GLOBAL counters --------------*/
static int admit;
static int relayin;
static int query;
static int relayout;

void start_switch(int id, const char *trafficfile, int port1, int port2,
                        int IPlow, int IPhigh, char* serv, char* port)
{

    self.id=id;
    self.IPlow=IPlow;
    self.IPhigh=IPhigh;
    self.ports[0]=0;
    self.ports[1]=port1;
    self.ports[2]=port2;

    struct sigaction sig_struct;
    pollfd keyboard;
    pollfd controller;
    FILE *traffic_stream;
    if ((traffic_stream = fopen(trafficfile, "r")) == NULL) perror("opening trafficFile");


    init_switch(&sig_struct, trafficfile, serv, port, traffic_stream, &keyboard, &controller);


    bool endoffile = 0;
    while (true){

        // 1. if not EOF: read single line from TrafficFile
        if (!endoffile){
            char input[LINE_BUFFER];
            if (fgets(input, LINE_BUFFER, traffic_stream) == NULL)
                endoffile = true;
            else {
                // printf("File line: %s", input);
                char copy[LINE_BUFFER];
                char *tok;
                strcpy(copy, input);
                if ((tok = strtok(input, " \n")) == NULL) continue;
                else if (tok[0] == '#') continue;
                else if (tok[2] == self.id + 0x30) {
                    // printf("Admitted from trafficFile: %s", copy);
                    admit++;
                    Packet rcv_pckt;
                    str2pckt(copy, &rcv_pckt);
                    int actiontype, actionval;
                    if (match_rule( rcv_pckt.field[2], &actiontype, &actionval) ){
                        perform_action(rcv_pckt, actiontype, actionval);
                    } else {
                        query_controller(rcv_pckt);
                    }


                }
            }
        }

        // 2. poll keyboard
        if (poll(&keyboard, 1, 0) > 0) {
            ssize_t n_read;
            char input[LINE_BUFFER] = "\0";
            if ((n_read = read(STDIN_FILENO, input, LINE_BUFFER)) < 0){} //TODO: EOF or error, deal with either
            else if (n_read == 5){
                //if list: list
                if (strncmp(input, "list", 4) == 0) print_switch();
                //if exit: list and exit
                else if (strncmp(input, "exit", 4) == 0) {print_switch(); exit(EXIT_SUCCESS);}
            }
        }

        // 3. poll controller socket
        // 4. poll incoming FIFOs (3 ports)
        if (poll(fds_rd, 3, 0) > 0) {
            for (int i = 0; i < 3; i++) {
                if (fds_rd[i].revents & POLLIN) {
                    char input[LINE_BUFFER] = "\0";
                    if (read_fromfifo(fds_rd[i].fd, input) == 0){
                        //handle various cases
                        Packet rcv_pckt;
                        str2pckt(input, &rcv_pckt);

                        switch (rcv_pckt.field[0]) {
                            case ADD: ;
                                //update switch info, respond with ACK
                                flow_table[n_rules].srcIP_lo = 0;
                                flow_table[n_rules].srcIP_hi = MAX_IP;
                                flow_table[n_rules].destIP_lo = rcv_pckt.field[1];
                                flow_table[n_rules].destIP_hi = rcv_pckt.field[2];
                                flow_table[n_rules].actionType = rcv_pckt.field[3];
                                flow_table[n_rules].actionVal = rcv_pckt.field[4];
                                flow_table[n_rules++].pri = rcv_pckt.field[5];

                                do_backlog();

                                break;

                            case RELAY: ;
                                relayin++;
                                relay(rcv_pckt);

                                break;

                        }
                    }
                }
            }
        }
    }
}

void init_switch(struct sigaction *sig_struct, const char *trafficfile, char *serv,
                    char *port, FILE *traffic_stream, pollfd *keyboard, pollfd *controller){
    //initialize SIGUSR1 signal handling
    sigset_t signal_mask;
    if (sigemptyset(&signal_mask) < 0)
    perror("Error on sigemptyset");

    sig_struct->sa_handler = sw_sig_handler;
    sig_struct->sa_mask = signal_mask;

    if (sigaction(SIGUSR1, sig_struct, NULL) < 0)
    perror("Error on sigaction(SIGUSR1)");

    //initialise globals
    admit = 0;
    relayin = 0;
    query = 0;
    relayout = 0;

    // open controller socket and connect to server
    setup_client(controller->fd, serv, port);
    //open all FIFOs for read and write (prevent blocking)
    char fifo[FILENAME_BUFFER];
    for (int i = 0; i<3; i++){
        if (self.ports[i] == -1) continue;
        get_fifo_name(fifo, self.ports[i], self.id);
        if ((fds_rd[i].fd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
        perror(fifo);
        fds_rd[i].events = POLLIN;
        get_fifo_name(fifo, self.id, self.ports[i]);
        if ((fds_wr[i] = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
        perror(fifo);
    }
    controller->events = POLLIN;

    keyboard->fd = STDIN_FILENO;
    keyboard->events = POLLIN;

    //initialize first rule in flow table
    flow_table[0] = (Rule) { .srcIP_lo= 0,
        .srcIP_hi= MAX_IP,
        .destIP_lo= self.IPlow,
        .destIP_hi= self.IPhigh,
        .actionType= FORWARD,
        .actionVal= 3,
        .pri= MIN_PRI,
        .pktCount= 0};
    n_rules = 1;


    //handshake: send OPEN to controller
    char packet[MAX_PACKET_LENGTH];
    snprintf(packet, MAX_PACKET_LENGTH,
        "%i %i %i %i %i %i",
        OPEN, self.id, self.ports[1],
        self.ports[2], self.IPlow, self.IPhigh);
        write_to_socket(controller->fd, packet);
        // wait for ACK
        char input[LINE_BUFFER];
        poll(controller, 1, -1); //hang forever waiting for ACK
        if (read_from_socket(controller->fd, input) == 0) {
            Packet rcv_pckt;
            str2pckt(input, &rcv_pckt);
            if (rcv_pckt.field[0] != ACK){
                printf("Received: %s\nNot ACK\n", input);
                exit(EXIT_FAILURE);}//problems
        }


}

void sw_sig_handler(int signal){
    print_switch();
}

void print_switch() {
    printf("Flow Table:\n");
    for (int i = 0; i < n_rules; i++){
        printf("[%i] (srcIP= %i-%i, destIP= %i-%i, action= %s:%i, pri=%i, pktCount= %i)\n",
                i, flow_table[i].srcIP_lo, flow_table[i].srcIP_hi,
                flow_table[i].destIP_lo, flow_table[i].destIP_hi,
                (flow_table[i].actionType == FORWARD) ? "FORWARD" : "DROP",
                flow_table[i].actionVal, flow_table[i].pri, flow_table[i].pktCount);
    }
    printf("\n");
    printf("Packet Stats:\n"
        "Received:    ADMIT:%i, ACK:%i, ADDRULE:%i, RELAYIN:%i\n"
        "Transmitted: OPEN:%i, QUERY:%i, RELAYOUT:%i\n\n",
        admit,1,n_rules-1,relayin,1,query,relayout);
}


bool match_rule( int destIP, int *actiontype, int *actionval){
int curr_priority = MIN_PRI + 1;
int matched = -1;
for (int i = 0; i < n_rules; i++){
    if (destIP >= flow_table[i].destIP_lo
        && destIP <= flow_table[i].destIP_hi
        && flow_table[i].pri < curr_priority) {
            matched = i;
            curr_priority = flow_table[i].pri;
            *actiontype = flow_table[i].actionType;
            *actionval = flow_table[i].actionVal;

        }
    }
    if (matched >= 0) {
        flow_table[matched].pktCount += 1;
        // printf("We matched a packet!\n");
        return true;
    }else return false;
}

void perform_action(Packet pckt, int actiontype, int actionval){
    char str_out[MAX_PACKET_LENGTH];
    if (actiontype == FORWARD) {
        snprintf(str_out, MAX_PACKET_LENGTH,
            "%i %i %i", RELAY, pckt.field[1], pckt.field[2]);

        if (actionval < 3){
            write_tofifo(fds_wr[actionval], str_out);
            relayout++;
        }
    } //nothing to do for DROP
}



void query_controller(Packet pckt){
    if (n_rules - 1 == query) { //no pending query
        char str[MAX_PACKET_LENGTH];
        snprintf(str, MAX_PACKET_LENGTH,
            "%i %i %i", QUERY, self.id, pckt.field[2]);
        write_tofifo(fds_wr[0], str);
        query++;
    }
    pckt_buf_op(STORE, backlog, &pckt);
}



void relay(Packet pckt){
    int actiontype, actionval;
    if (match_rule(pckt.field[2], &actiontype, &actionval) ){
        perform_action(pckt, actiontype, actionval);
    } else {
        query_controller(pckt);
    }
}

void do_backlog() {
    Packet backlog_pckt;
    int ttl = PCKT_BUFFER_SIZE;
    while ( pckt_buf_op(RM, backlog, &backlog_pckt) == 0  && ttl ){
        relay(backlog_pckt);
        ttl--;
    }
}
