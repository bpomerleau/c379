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
void print_controller();
void ctrl_sig_handler(int signal);




void start_controller(int nSwitch) {

    Switch switches[nSwitch];
    pollfd keyboard;
    pollfd fifo_in[nSwitch];
    int fifo_out[nSwitch];
    struct sigaction sig_struct;

    init_controller(nSwitch, &sig_struct, &keyboard, fifo_in, fifo_out);


    char output_packet[MAX_PACKET_LENGTH];


    while (1){
        // 1. poll keyboard
        char input[LINE_BUFFER];
        ssize_t n_read;
        int num_ready = poll(&keyboard, 1, 0);
        if (num_ready > 0) {
            if ((n_read = read(STDIN_FILENO, input, LINE_BUFFER)) < 0){} //TODO: EOF or error, deal with either
            else if (n_read == 5){
                if (strncmp(input, "list", 4) == 0) print_controller();
                else if (strncmp(input, "exit", 4) == 0) {print_controller(); exit(EXIT_SUCCESS);}
            }
        } else if (num_ready < 0) perror("reading from keyboard");

        // 2. poll fifos
        num_ready = poll(fifo_in, nSwitch, 0);
        if (num_ready > 0) {
            for (int i = 0; i < nSwitch; i++) {
                if (fifo_in[i].revents & POLLIN) {
                    if ((n_read = read(fifo_in[i].fd, input, LINE_BUFFER)) <= 0){} //TODO: EOF or error, deal with either
                    else {
                        //handle various cases
                        printf("Received: %s\n", input);
                        char *type = strtok(input, " ");
                        //update switch info, respond with ACK
                        if (strcmp(type, "OPEN") == 0){
                            int sw_num = atoi(strtok(NULL, " "));
                            switches[sw_num].ports[1] = atoi(strtok(NULL, " "));
                            switches[sw_num].ports[2] = atoi(strtok(NULL, " "));
                            switches[sw_num].IPlow = (unsigned int) atoi(strtok(NULL, " "));
                            switches[sw_num].IPhigh = (unsigned int) atoi(strtok(NULL, " "));

                            snprintf(output_packet, MAX_PACKET_LENGTH, "ACK");
                            printf("Transmitting: %s\n", output_packet);
                            if (write(fifo_out[i], output_packet, 3) == -1) perror("write to fifo");
                            scanf("press any key");


                        }
                        else if (strcmp(type, "QUERY") == 0){
                            unsigned int destIP = (unsigned int) atoi(strtok(NULL, " "));
                            int target_switch = -1;
                            for (int j = 0; j < nSwitch; j++){
                                if (destIP > switches[i].IPlow && destIP < switches[i].IPhigh) {
                                    target_switch = j;
                                }
                            }
                            if (target_switch > 0){
                                char pckt[MAX_PACKET_LENGTH];
                                snprintf(pckt, MAX_PACKET_LENGTH,
                                    "ADD %u %u %i %i %i",
                                    switches[target_switch].IPlow,
                                    switches[target_switch].IPhigh,
                                    FORWARD,
                                    (target_switch < i) ? 1 : 2,
                                    MIN_PRI
                                );
                                printf("Transmitting: %s\n", pckt);
                                if (write(fifo_out[i], pckt, strlen(pckt)) == -1) perror("write to fifo");
                            } else {
                                char pckt[MAX_PACKET_LENGTH];
                                snprintf(pckt, MAX_PACKET_LENGTH,
                                    "ADD %u %u %i %i %i",
                                    destIP,
                                    destIP,
                                    DROP,
                                    0,
                                    MIN_PRI
                                );
                                printf("Transmitting: %s\n", pckt);
                                if (write(fifo_out[i], pckt, strlen(pckt)) == -1) perror("write to fifo");

                            }
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
        print_controller();
    }

    void print_controller() {
        printf("Switch information:\n");
        // for (int i = 0; i < nSwitch; i++){
        //     "[sw%i] port1= %i, port2= %i, port3= %u-%u\n\n\n",
        //     switches[i].num, switches[i].ports[1], switches[i].ports[2], switches[i].IPlow, switches[i].IPhigh);
        // }
    }
