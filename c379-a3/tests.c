#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>



void sig_handler(int signum){
    printf("handled\n");
}

void sig_enact(){
    sigset_t mask;
    sigemptyset(&mask);
    struct sigaction new = {
        .sa_handler = sig_handler,
        .sa_mask = mask,
        .sa_flags = 0
    };
    sigaction(SIGUSR1, &new, NULL);
}
int main(int argc, char *argv[]){
    sig_enact();
    while(1){}
}
