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

void sw_sig_handler(int signal);
void switch_list();
void init_switch(Switch *self, struct sigaction *sig_struct);

void start_switch(int num,
                const char *trafficfile,
                int port1,
                int port2,
                unsigned int IPlow,
                unsigned int IPhigh)
{
    FILE *traffic_stream;
    if ((traffic_stream = fopen(trafficfile, "r")) == NULL)
        perror("opening trafficFile");

    Switch self = { .num=num,
                    .IPlow=IPlow,
                    .IPhigh=IPhigh,
                    .traffic_stream=traffic_stream,
                    .ports= {0, port1, port2}
                };
    printf("port1: %i\nport2: %i\n", self.ports[1], self.ports[2]);
    struct sigaction sig_struct;
    init_switch(&self, &sig_struct);
    printf("Initialisation successful.\n");
    while (1){

    }
    //if not EOF: read single line from TrafficFile
        //ignore empty lines, comments lines (starting with #), and lines for other switches
        //if line specifies current switch, admit packet
        //attempt to match admitted packet to flow table
            //success: perform action specified
            //else: QUERY controller
    //poll keyboard
        //if list: list
        //if exit: list and exit
    //poll incoming FIFOs (3 ports)
        //if avail, read
            //if ADD: add info to flow table
                //attempt to match any outstanding packets to flow table and perform action
            //if RELAY:
                //attempt to match packet to flow table
                    //success: perform action
                    //else: QUERY controller
}

void init_switch(Switch *self, struct sigaction *sig_struct){
    //initialize SIGUSR1 signal handling
    sigset_t signal_mask;
    if (sigemptyset(&signal_mask) < 0)
        perror("Error on sigemptyset");

    sig_struct->sa_handler = sw_sig_handler;
    sig_struct->sa_mask = signal_mask;

    if (sigaction(SIGUSR1, sig_struct, NULL) < 0)
        perror("Error on sigaction(SIGUSR1)");

    //open all FIFOs for read and write (prevent blocking)
    char fifo[FILENAME_BUFFER];
    for (int i = 0; i<3; i++){
        if (self->ports[i] == -1) continue;
        get_fifo_name(fifo, self->ports[i], self->num);
        if ((self->fds_rd[i].fd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
            perror(fifo);
        self->fds_rd[i].events = POLLIN;
        get_fifo_name(fifo, self->num, self->ports[i]);
        if ((self->fds_wr[i] = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
            perror(fifo);
    }

    //initialize first rule in flow table
    self->flow_table[0] = (Rule) { .srcIP_lo= 0,
                           .srcIP_hi= MAX_IP,
                           .destIP_lo= self->IPlow,
                           .destIP_hi= self->IPhigh,
                           .actionType= FORWARD,
                           .actionVal= 3,
                           .pri= MIN_PRI,
                           .pktCount= 0};


    //handshake: send OPEN to controller
    char packet[MAX_PACKET_LENGTH];
    snprintf(packet, MAX_PACKET_LENGTH,
            "OPEN %i %i %i %u %u\n",
            self->num, self->ports[1],
            self->ports[2], self->IPlow, self->IPhigh);
    printf("%s", packet);
    if (write(self->fds_wr[0], packet, strlen(packet)) == -1) perror("write to fifo");
    // wait for ACK
    char input[LINE_BUFFER];
    size_t n = LINE_BUFFER;
    ssize_t n_read;
    poll(&self->fds_rd[0], 1, -1);
    if ((n_read = read(self->fds_rd[0].fd, input, n)) <= 0) perror("problem with read"); //problems
    if (strcmp(input, "ACK") != 0){perror("didn't get an ACK"); exit(EXIT_FAILURE);}//problems



}

void sw_sig_handler(int signal){
		switch_list();
}

void switch_list() {
    printf("Here's a list of shit n stuff");
}
