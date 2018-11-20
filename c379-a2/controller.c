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
#include "packet.h"

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

void init_controller(int nSwitch, struct sigaction *sig_struct, pollfd *keyboard,
                            pollfd *fifo_in, int *fifo_out);
void print_controller();
void ctrl_sig_handler(int signal);
void answer_query(Packet rcv_pckt);

/*----- GLOBAL data ----------------------*/
Switch switches[MAX_NSW];
pollfd fifo_in[MAX_NSW];
int fifo_out[MAX_NSW];
int nSwitch;

/*------ GLOBAL counters ----------------*/
int open_switches;
int ack;
int query;
int add;


void start_controller(int n) {

    pollfd keyboard;

    struct sigaction sig_struct;

    open_switches = 0;
    Packet pckt_buf[PCKT_BUFFER_SIZE];

    init_controller(n, &sig_struct, &keyboard, fifo_in, fifo_out);





    while (1){

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

        // 2. poll fifos
        if (poll(fifo_in, nSwitch, 0) > 0) {
            for (int i = 0; i < nSwitch; i++) {
                if (fifo_in[i].revents & POLLIN) {
                    if (read_fromfifo(fifo_in[i].fd, input) == 0){
                        //handle various cases
                        Packet rcv_pckt;
                        if ( str2pckt(input, &rcv_pckt) == -1) continue;
                        switch (rcv_pckt.field[0]) {

                            case OPEN:

                                open_switches++;

                                switches[i].id = rcv_pckt.field[1];
                                switches[i].ports[1] = rcv_pckt.field[2];
                                switches[i].ports[2] = rcv_pckt.field[3];
                                switches[i].IPlow = rcv_pckt.field[4];
                                switches[i].IPhigh = rcv_pckt.field[5];
                                //send ACK
                                char str[MAX_PACKET_LENGTH] = "\0";
                                snprintf(str, MAX_PACKET_LENGTH, "%i", ACK);
                                write_tofifo(fifo_out[i], str);
                                ack++;

                                if (open_switches == nSwitch) {
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

                                query++;
                                if (open_switches < nSwitch) {
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

void init_controller(int n, struct sigaction *sig_struct, pollfd *keyboard,
                        pollfd *fifo_in, int *fifo_out)
{

    //initialize SIGUSR1 signal handling
    sigset_t signal_mask;
    if (sigemptyset(&signal_mask) < 0)
    perror("Error on sigemptyset");

    sig_struct->sa_handler = ctrl_sig_handler;
    sig_struct->sa_mask = signal_mask;

    nSwitch = n;

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
    printf("Switch information (nSwitch=%i):\n", nSwitch);
    for (int i = 0; i < nSwitch; i++){
        printf("[sw%i] port1= %i, port2= %i, port3= %u-%u\n",
        switches[i].id, switches[i].ports[1], switches[i].ports[2], switches[i].IPlow, switches[i].IPhigh);
    }
    printf("\n");
    printf("Packet Stats:\n"
        "Received:    OPEN:%i, QUERY:%i\n"
        "Transmitted: ACK:%i, ADD:%i\n\n",
        open_switches,query,ack,add);
}

void answer_query(Packet rcv_pckt){
    int destIP = rcv_pckt.field[2];
    int i = rcv_pckt.field[1] - 1;

    int target_switch = -1;
    for (int j = 0; j < nSwitch; j++){
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
    write_tofifo(fifo_out[i], snd_str);
    add++;
}






//eof
