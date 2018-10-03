#include "a1mon.h"
#include "ptree.h"

bool volatile wait = false;

void sig_handler(int signal){
    wait = false;
}

int main(int argc, char *argv[]){

    sigset_t signal_mask;
    pid_t target_pid;
    int interval;
    int counter = 0;
    FILE *in_stream;

    struct rlimit new_limit = {1,1};
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
    pid_t pid;
    pid_t ppid;
    char state;
    // CAUTION: TREE STRUCT TRACKS DYNAMICALLY ALLOCATED MEMORY; SEE "ptree.h"
    TREE old_tree;
    TREE new_tree;
    bool target_found = 0;

    while(true){
        while(wait){}
        sigprocmask(SIG_BLOCK, &signal_mask, NULL);
        wait = true;
        counter++;
        printf("a1mon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:",
        counter, getpid(), target_pid, interval);
        new_tree = init_tree(target_pid);
        in_stream = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r");
        while((nread = getline(&line, &n, in_stream)) != -1){
            printf("%s",line);
            /*assumption: since entries are sorted by start time, parent will
                          appear before any child. */
            strtok(line, " ");  //user field
            pid = atoi(strtok(NULL, " "));
            ppid = atoi(strtok(NULL, " "));
            state = strtok(NULL, " ")[0];
            if (!target_found && pid == target_pid){
                    if (state == 'Z'){
                        //head process is terminated, kill tree and quit
                        kill_tree(&old_tree);
                        return 0;
                    }
                    target_found = true;
                    continue;
            } else if (target_found){
                if(is_node(&new_tree, ppid)) add_node(&new_tree, pid);
            }
        }
        if (!target_found){
            // no target_pid process discovered, kill all and quit.
            kill_tree(&old_tree);
            return 0;
        } else {
            delete_tree(&old_tree);
            old_tree = new_tree;
            print_tree(&old_tree);
            target_found = false;
        }
        if ( pclose(in_stream) == -1)
            perror("pclose");
        sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
    }
    free(line);
}
