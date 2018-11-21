/**
* main.c
*
* developed for CMPUT379 Assignment 2 - a2sdn
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    if (argc == 4 && strcmp(argv[1], "cont") == 0) {
        // TODO match pattern with regex
        if (atoi(argv[2]) > 0
            && atoi(argv[2]) <= MAX_NSW
            && atoi(argv[3]) <= MAX_PORT)
                start_controller(atoi(argv[2]), argv[3]);

    }

    else if (argc == 8) {
        // TODO match pattern with regex
        // extract fields:
        int sw_num = atoi(&argv[1][2]);
        char *trafficfile = argv[2];
        int port1_num = (strcmp(argv[3],"null") == 0) ? -1 : atoi(&argv[3][2]);
        int port2_num = (strcmp(argv[4],"null") == 0) ? -1 : atoi(&argv[4][2]);
        int IPlow = atoi(strtok(argv[5], "-"));
        int IPhigh = atoi(strtok(NULL, "-"));
        char *serv = argv[6];
        char *port = argv[7];

        start_switch(sw_num, trafficfile, port1_num, port2_num, IPlow, IPhigh, serv, port);//TODO fix this
    }
    else printf("You done messed up.\nUsage:\nI'll write this later.\n");
}
