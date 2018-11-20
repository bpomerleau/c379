/**
* a1mon.c
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/


#include "a1mon.h"
#include "ptree.h"

bool volatile gb_WAIT = false;

int main(int argc, char *argv[]){

    //set CPU time limit
    struct rlimit new_limit = {CPU_SOFT_LIMIT,CPU_HARD_LIMIT};
    if ( setrlimit(RLIMIT_CPU, &new_limit) == -1 ) {
        perror("setrlimit(RLIMIT_CPU)");
        exit(EXIT_FAILURE);
    }

    pid_t target_pid;
    int interval;

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
    sigset_t signal_mask;

    if (sigemptyset(&signal_mask) == -1) {
        perror("sigemptyset");
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&signal_mask, SIGALRM) == -1) {
        perror("sigaddset(SIGALRM)");
        exit(EXIT_FAILURE);
    }

    //set ISR
    struct sigaction sig_struct = { .sa_handler = sig_handler, .sa_mask = signal_mask };

    if (sigaction(SIGALRM, &sig_struct, NULL) == -1) {
        perror("sigaction(SIGALRM)");
        exit(EXIT_FAILURE);
    }

    //set timer
    struct timeval timer_struct = { .tv_sec = interval };
    const struct itimerval timer = { .it_interval = timer_struct, .it_value = timer_struct};

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }




    int counter = 0;
    FILE *in_stream;
    char *line;
    size_t line_buf = LINE_BUFFER_SIZE;
    size_t nread;
    line = (char *) malloc(line_buf);
    pid_t pid;
    pid_t ppid;
    char state;
    // CAUTION: TREE STRUCT TRACKS DYNAMICALLY ALLOCATED MEMORY; SEE "ptree.h"
    TREE tree = init_tree(target_pid);
    bool target_found = false;

    while(true){
        while(gb_WAIT){}
        sigprocmask(SIG_BLOCK, &signal_mask, NULL);
        gb_WAIT = true;
        counter++;
        printf("a1mon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:",
        counter, getpid(), target_pid, interval);
        in_stream = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r");
        while((nread = getline(&line, &line_buf, in_stream)) != -1){
            printf("%s",line);
            /*assumption: since entries are sorted by start time, parent will
                          appear before any child. */
            strtok(line, " ");  //user field
            pid = atoi(strtok(NULL, " "));
            ppid = atoi(strtok(NULL, " "));
            state = strtok(NULL, " ")[0];
            if (pid == target_pid){
                    if (state == 'Z'){
                        //head process is terminated, kill tree and quit
                        kill_tree(&tree);
                        break;
                    }
                    target_found = true;
                    continue;
            } else {
                if ( is_node(&tree, ppid) && !is_node(&tree, pid) ) add_node(&tree, pid);
            }
        }
        if ( pclose(in_stream) == -1) {
            perror("pclose");
            exit(EXIT_FAILURE);
        }
        if (target_found){
            print_tree(&tree);
            target_found = false;
        } else {
            // no target_pid process discovered, kill all and quit.
            kill_tree(&tree);
            break;
        }
        sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
    }
}

/**
* handles the interrupt signal
* flips global wait flag
*
* Parameters:
*
* Returns:
*
*/
void sig_handler(int signal){
    gb_WAIT = false;
}
