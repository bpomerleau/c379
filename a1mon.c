#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

bool volatile wait = false;

void sig_handler(int signal){
    wait = false;
}


int main(int argc, char *argv[]){

    sigset_t signal_mask;
    int target_pid;
    int interval;
    int counter = 0;
    FILE *in_stream;

    struct rlimit new_limit = {1,1};
    struct tms tbuf_before, tbuf_after;
    clock_t wall_before, wall_after;
    if(setrlimit(RLIMIT_CPU, &new_limit)==-1)
        perror("setrlimit(RLIMIT_CPU)");

    if (argc == 2){
        target_pid = atoi(argv[1]);
        interval = 3;
    } else if (argc == 3){
        target_pid = atoi(argv[1]);
        interval = atoi(argv[2]);
    } else if (argc > 3) {
        printf("Too many arguments\n");
        return 0;
    } else {
        printf("Too few arguements\n");
        return 0;
    }

    // make signal mask
    if (sigemptyset(&signal_mask) == -1)
        perror("sigemptyset");
    if (sigaddset(&signal_mask, SIGALRM) == -1)
        perror("sigaddset(SIGALRM)");

    struct sigaction sig_struct = { .sa_handler = sig_handler, .sa_mask = signal_mask };

    //timer sigaction
    if (sigaction(SIGALRM, &sig_struct, NULL) == -1)
        perror("sigaction(SIGALRM)");

    //itimer init
    struct timeval timer_struct = { .tv_sec = interval };
    const struct itimerval timer = { .it_interval = timer_struct, .it_value = timer_struct};

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
        perror("setitimer");


    char *line;
    size_t n = 100;
    size_t nread;
    line = (char *) malloc(n);

    while(true){
        while(wait){}
        wait = true;
        counter++;
        printf("a1mon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:",
        counter, getpid(), target_pid, interval);
        in_stream = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r");
        while((nread = getline(&line, &n, in_stream)) != -1){
            printf("%s",line);
            
        }
        if ( pclose(in_stream) == -1)
            perror("pclose");
    }

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    free(line);
}
