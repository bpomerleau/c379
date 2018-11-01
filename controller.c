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
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <poll.h>

#include <unistd.h>

// typedef struct Switches {
//     Switch *switches;
//     struct pollfd *fifo_in;
//     struct pollfd *fifo_out;
// } Switches;

void init_controller(int nSwitch,
                    struct sigaction *sig_struct,
                    pollfd *keyboard,
                    pollfd *fifo_in,
                    int *fifo_out
                );

void controller_list();
void ctrl_sig_handler(int signal);

void start_controller(int nSwitch) {

    Switch switches[nSwitch];
    pollfd keyboard;
    pollfd fifo_in[nSwitch];
    int fifo_out[nSwitch];
    struct sigaction sig_struct;

    init_controller(nSwitch, &sig_struct, &keyboard, fifo_in, fifo_out);

    int num_ready;
    char input[LINE_BUFFER];
    size_t n = LINE_BUFFER;
    ssize_t n_read;

    char output_packet[MAX_PACKET_LENGTH];


    while (1){
        // 1. poll keyboard
        num_ready = poll(&keyboard, 1, 0);
        if (num_ready > 0) {
            if ((n_read = read(STDIN_FILENO, input, n)) < 0){} //TODO: EOF or error, deal with either
            else if (n_read == 5){
                if (strncmp(input, "list", 4) == 0) controller_list();
                else if (strncmp(input, "exit", 4) == 0) {controller_list(); exit(EXIT_SUCCESS);}
            }
        } else if (num_ready < 0) perror("reading from keyboard");

        // 2. poll fifos
        num_ready = poll(fifo_in, nSwitch, 0);
        if (num_ready > 0) {
            for (int i = 0; i < nSwitch; i++) {
                if (fifo_in[i].revents & POLLIN) {
                    if ((n_read = read(fifo_in[i].fd, input, n)) <= 0){} //TODO: EOF or error, deal with either
                    else {
                        //handle various cases
                            char *type = strtok(input, " ");
                            //update switch info, respond with ACK
                                if (strcmp(type, "OPEN") == 0){
                                    int sw_num = atoi(strtok(NULL, " "));
                                    switches[sw_num].ports[1] = atoi(strtok(NULL, " "));
                                    switches[sw_num].ports[2] = atoi(strtok(NULL, " "));
                                    switches[sw_num].IPlow = (unsigned int) atoi(strtok(NULL, " "));
                                    switches[sw_num].IPhigh = (unsigned int) atoi(strtok(NULL, " "));

                                    printf("%i %i %i %u %u\n",
                                    sw_num, switches[sw_num].ports[1],
                                    switches[sw_num].ports[2], switches[sw_num].IPlow, switches[sw_num].IPhigh);

                                    snprintf(output_packet, MAX_PACKET_LENGTH, "ACK");
                                    printf("%s", output_packet);
                                    if (write(fifo_out[sw_num-1], output_packet, strlen(output_packet)) == -1) perror("write to fifo");


                                }
                                else if (strcmp(type, "QUERY") == 0){

                                }
                                //determine destination port
                                    //reply with ADD rule that will get it closer to destination
                                    //if no desitination port, replay with rule to drop
                    }
                }
            }
        }

    }

}

void init_controller(int nSwitch,
                    struct sigaction *sig_struct,
                    pollfd *keyboard,
                    pollfd *fifo_in,
                    int *fifo_out) {

    //initialize SIGUSR1 signal handling
    sigset_t signal_mask;
    if (sigemptyset(&signal_mask) < 0)
        perror("Error on sigemptyset");

    sig_struct->sa_handler = ctrl_sig_handler;
    sig_struct->sa_mask = signal_mask;

    if (sigaction(SIGUSR1, sig_struct, NULL) < 0)
        perror("Error on sigaction(SIGUSR1)");

    //open all read and write fifos for both read/write (to avoid blocking)
    char fifo[FILENAME_BUFFER];

    for (int i = 1; i <= nSwitch; i++ ) {
        get_fifo_name(fifo, i, 0);
        if ((fifo_in[i-1].fd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
            perror(fifo);
        get_fifo_name(fifo, 0, i);
        if ((fifo_out[i-1] = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
            perror(fifo);
    }

    //setup pollfds

    keyboard->fd = STDIN_FILENO;
    keyboard->events = POLLIN;

    for (int i = 0; i < nSwitch; i++) {
        fifo_in[i].events = POLLIN;
    }

    //TODO: only complete intialisation after all switches connected



}


void ctrl_sig_handler(int signal){
		controller_list();
}

void controller_list() {
    printf("Here's a list of shit n shit");
}
