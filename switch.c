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
void init_switch(struct sigaction *sig_struct, const char *trafficfile, pollfd *keyboard);
bool match_rule(unsigned int destIP, int *actiontype, int *actionval);
void perform_action(char *pckt, int actiontype, int actionval);
void query_controller(unsigned int destIP);
void outstanding_jobs();

Switch self;

int admitted;

void start_switch(int num,
    const char *trafficfile,
    int port1,
    int port2,
    unsigned int IPlow,
    unsigned int IPhigh)
    {

    self.num=num;
    self.IPlow=IPlow;
    self.IPhigh=IPhigh;
    self.ports[0]=0;
    self.ports[1]=port1;
    self.ports[2]=port2;

    print_switch();

    struct sigaction sig_struct;
    pollfd keyboard;

    init_switch(&sig_struct, trafficfile, &keyboard);


    //if not EOF: read single line from TrafficFile
    //ignore empty lines, comments lines (starting with #), and lines for other switches
    //if line specifies current switch, admit packet
    //attempt to match admitted packet to flow table
    //success: perform action specified
    //else: QUERY controller
    bool endoffile = 0;
    while (true){
        if (!endoffile){
            char input[LINE_BUFFER];
            if (fgets(input, LINE_BUFFER, self.traffic_stream) == NULL)
            endoffile = true;
            else {
                char copy[LINE_BUFFER];
                char *tok;
                strcpy(copy, input);
                char *frwd_pckt;// = copy;
                printf("File line: %s", input);
                if ((tok = strtok(input, " \n")) == NULL) continue;
                else if (tok[0] == '#') continue;
                else if (tok[2] == self.num + 0x30) {
                    printf("Admitted from trafficFile: %s\n", copy);
                    admitted++;
                    tok = strtok(NULL, " ");
                    frwd_pckt = copy + (tok - input);
                    tok = strtok(NULL, " ");
                    int actiontype, actionval;
                    if (match_rule( (unsigned int) atoi(tok), &actiontype, &actionval) ){
                        perform_action(frwd_pckt, actiontype, actionval);
                    } else {
                        query_controller((unsigned int) atoi(tok));
                    }


                }
            }
        }

        //poll keyboard
        //if list: list
        //if exit: list and exit
        int num_ready = poll(&keyboard, 1, 0);
        if (num_ready > 0) {
            ssize_t n_read;
            char input[LINE_BUFFER];
            if ((n_read = read(STDIN_FILENO, input, LINE_BUFFER)) < 0){} //TODO: EOF or error, deal with either
            else if (n_read == 5){
                if (strncmp(input, "list", 4) == 0) print_switch();
                else if (strncmp(input, "exit", 4) == 0) {print_switch(); exit(EXIT_SUCCESS);}
            }
        } else if (num_ready < 0) perror("reading from keyboard");
        //poll incoming FIFOs (3 ports)
        //if avail, read
        //if ADD: add info to flow table
        //attempt to match any outstanding packets to flow table and perform action
        //if RELAY:
        //attempt to match packet to flow table
        //success: perform action
        //else: QUERY controller
        num_ready = poll(self.fds_rd, 3, 0);
        if (num_ready > 0) {
            for (int i = 0; i < 3; i++) {
                if (self.fds_rd[i].revents & POLLIN) {
                    char input[LINE_BUFFER];
                    ssize_t n_read;
                    if ((n_read = read(self.fds_rd[i].fd, input, LINE_BUFFER)) <= 0){perror("maybe");} //TODO: EOF or error, deal with either
                    else {
                        //handle various cases
                        printf("Received: %s\n", input);
                        char copy[LINE_BUFFER];
                        strcpy(copy, input);
                        char *type = strtok(input, " ");
                        //update switch info, respond with ACK
                        if (strcmp(type, "ADD") == 0){
                            self.flow_table[self.n_rules].srcIP_lo = 0;
                            self.flow_table[self.n_rules].srcIP_hi = MAX_IP;
                            self.flow_table[self.n_rules].destIP_lo = (unsigned int) atoi(strtok(NULL, " "));
                            self.flow_table[self.n_rules].destIP_hi = (unsigned int) atoi(strtok(NULL, " "));
                            self.flow_table[self.n_rules].actionType = atoi(strtok(NULL, " "));
                            self.flow_table[self.n_rules].actionVal = atoi(strtok(NULL, " "));
                            self.flow_table[self.n_rules].actionVal = atoi(strtok(NULL, " "));

                            outstanding_jobs();

                        }
                        else if (strcmp(type, "RELAY") == 0){
                            char *frwd_pckt;
                            char *tok;
                            frwd_pckt = copy + (strtok(NULL, " ") - input);
                            tok = strtok(NULL, " ");
                            int actiontype, actionval;
                            if (match_rule( (unsigned int) atoi(tok), &actiontype, &actionval) ){
                                perform_action(frwd_pckt, actiontype, actionval);
                            } else {
                                query_controller((unsigned int) atoi(tok));
                            }

                        }
                    }
                }
            }
        }
    }
}

void init_switch(struct sigaction *sig_struct, const char *trafficfile, pollfd *keyboard){
    //initialize SIGUSR1 signal handling
    sigset_t signal_mask;
    if (sigemptyset(&signal_mask) < 0)
    perror("Error on sigemptyset");

    sig_struct->sa_handler = sw_sig_handler;
    sig_struct->sa_mask = signal_mask;

    if (sigaction(SIGUSR1, sig_struct, NULL) < 0)
    perror("Error on sigaction(SIGUSR1)");

    FILE *traffic_stream;
    if ((traffic_stream = fopen(trafficfile, "r")) == NULL)
    perror("opening trafficFile");
    self.traffic_stream = traffic_stream;

    admitted = 0;

    //open all FIFOs for read and write (prevent blocking)
    char fifo[FILENAME_BUFFER];
    for (int i = 0; i<3; i++){
        if (self.ports[i] == -1) continue;
        get_fifo_name(fifo, self.ports[i], self.num);
        if ((self.fds_rd[i].fd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
        perror(fifo);
        self.fds_rd[i].events = POLLIN;
        get_fifo_name(fifo, self.num, self.ports[i]);
        if ((self.fds_wr[i] = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
        perror(fifo);
    }

    keyboard->fd = STDIN_FILENO;
    keyboard->events = POLLIN;

    //initialize first rule in flow table
    self.flow_table[0] = (Rule) { .srcIP_lo= 0,
        .srcIP_hi= MAX_IP,
        .destIP_lo= self.IPlow,
        .destIP_hi= self.IPhigh,
        .actionType= FORWARD,
        .actionVal= 3,
        .pri= MIN_PRI,
        .pktCount= 0};
    self.n_rules = 1;


    //handshake: send OPEN to controller
    char packet[MAX_PACKET_LENGTH];
    snprintf(packet, MAX_PACKET_LENGTH,
        "OPEN %i %i %i %u %u",
        self.num, self.ports[1],
        self.ports[2], self.IPlow, self.IPhigh);
        printf("Transmitting: %s\n", packet);
        if (write(self.fds_wr[0], packet, strlen(packet)) == -1) perror("write to fifo");
        // wait for ACK
        char input[LINE_BUFFER];
        size_t n = LINE_BUFFER;
        ssize_t n_read;
        poll(&self.fds_rd[0], 1, -1); //hang forever waiting for ACK
        if ((n_read = read(self.fds_rd[0].fd, input, n)) <= 0) perror("problem with read"); //problems
        if (strcmp(input, "ACK") != 0){
            printf("Received: %s\nNot ACK\n", input);
            exit(EXIT_FAILURE);}//problems
        else printf("Received: %s\n", input);


}

void sw_sig_handler(int signal){
    print_switch();
}

void print_switch() {

    printf("Packet Stats:\n"
        "Received:    ADMIT:%i, ACK:%i, ADDRULE:%i, RELAYIN:%i\n"
        "Transmitted: OPEN:%i, QUERY:%i, RELAYOUT:%i\n\n\n\n",
        admitted,1,self.n_rules-1,0,1,0,0);
}


bool match_rule(unsigned int destIP, int *actiontype, int *actionval){
int curr_priority = MIN_PRI + 1;
int matched = -1;
for (int i = 0; i < self.n_rules; i++){
    if (destIP > self.flow_table[i].destIP_lo
        && destIP < self.flow_table[i].destIP_hi
        && self.flow_table[i].pri < curr_priority) {
            matched = i;
            curr_priority = self.flow_table[i].pri;
            *actiontype = self.flow_table[i].actionType;
            *actionval = self.flow_table[i].actionVal;

        }
    }
    if (matched >= 0) {
        self.flow_table[matched].pktCount += 1;
        printf("We matched a packet!\n");
        return true;
    }else return false;
}

void perform_action(char *pckt, int actiontype, int actionval){
    char output_pckt[MAX_PACKET_LENGTH];
    if (actiontype == FORWARD) {
        snprintf(output_pckt, MAX_PACKET_LENGTH,
            "RELAY %s", pckt);
            printf("Transmitting: %s\n", output_pckt);
            if (actionval < 3){
                printf(" to port %i\n", actionval);
                if (write(self.fds_wr[actionval], output_pckt, strlen(output_pckt)) == -1) perror("write to fifo");
        }} //nothing to do for DROP
    }

void query_controller(unsigned int destIP){
    char packet[MAX_PACKET_LENGTH];
    snprintf(packet, MAX_PACKET_LENGTH,
        "QUERY %u",destIP);
        printf("Transmitting: %s\n", packet);
        if (write(self.fds_wr[0], packet, strlen(packet)) == -1) perror("write to fifo");
}


void  outstanding_jobs(){
    printf("do outstanding jobs\n");
}
