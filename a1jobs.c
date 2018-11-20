/**
* a1jobs.c
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* main program
*
*/

#include "a1jobs.h"

int main(int argc, char *argv[]){

    struct rlimit new_limit = {1,1};
    struct tms tbuf_before, tbuf_after;
    clock_t wall_before, wall_after;

    setup(&new_limit, &tbuf_before, &wall_before);

    JOB myjobs[MAXJOBS];
    int numjobs = 0;
    char in_str[100];
    char in_str_cpy[100];
    char *out_str[6];

    while(true){
        printf("a1jobs[%i]: ",getpid());
        fgets(in_str, 100, stdin);
        strtok(in_str, "\n"); //replace final \n char with \0

        if (strcmp(in_str, "list") == 0){
            print_jobs(myjobs, numjobs);
        }

        else if (strncmp(in_str, "run", 3) == 0 ) {
            if (numjobs >= MAXJOBS){
                printf("Job limit (%d) reached.\n", MAXJOBS);
                continue;
            }

            strcpy(in_str_cpy, in_str+4);
            if ( (out_str[0] = strtok(in_str," ")) != NULL){
                int i;
                for ( i = 1; i < 6; i++){
                    if ( (out_str[i] = strtok(NULL, " \n")) == NULL){
                        break;
                    }
                } //exit condition: i == size of array;

                if (i < 2) {
                    printf("Usage: run command [args(up to 4)] .\n");
                    continue;
                }
                pid_t retval;
                if ( (retval = fork()) == -1){
                    perror("fork");
                    continue;
                }
                if (retval == 0){ //child process
                    freopen("/dev/null","w",stdout);
                    run_command(out_str, i);
                }else { //parent process
                    myjobs[numjobs].index = numjobs;
                    myjobs[numjobs].pid = retval;
                    strcpy(myjobs[numjobs].command, in_str_cpy);
                    myjobs[i].state = running;
                    numjobs++;
                }
            }//if ( (out_str[0] = strtok(in_str," \n")) != NULL)

        }else if (strncmp(in_str, "suspend ", 8) == 0){
            strtok(in_str, " ");
            char *index_str = strtok(NULL, " ");
            int index = atoi(index_str);
            if (index >= 0 && index < numjobs && myjobs[index].state != terminated){
                kill(myjobs[index].pid,SIGSTOP);
                myjobs[index].state = suspended;
                printf("suspended %d\n", index);
            }

        }else if (strncmp(in_str, "resume ", 7) == 0){
            strtok(in_str, " ");
            char *index_str = strtok(NULL, " ");
            int index = atoi(index_str);
            if (index >= 0 && index < numjobs && myjobs[index].state != terminated){
                kill(myjobs[index].pid,SIGCONT);
                myjobs[index].state = running;
                printf("resumed %d\n", index);
            }

        }else if (strncmp(in_str, "terminate ", 10) == 0){
            strtok(in_str, " ");
            char *index_str = strtok(NULL, " ");
            int index = atoi(index_str);
            if (index >= 0 && index < numjobs && myjobs[index].state != terminated){
                kill(myjobs[index].pid,SIGKILL);
                myjobs[index].state = terminated;
                printf("terminated %d\n", index);
            }

        }else if (strcmp(in_str, "exit") == 0){
            printf("exiting\n");
            for (int i = 0; i < numjobs; i++){
                if (myjobs[i].state != terminated){
                    kill(myjobs[i].pid,SIGKILL);
                    myjobs[i].state = terminated;
                    printf("terminating %d...\n", myjobs[i].pid);
                }
            }
            break;
        }else if (strcmp(in_str, "quit") == 0){
            printf("quitting...\n");
            break;
        }else printf("unknown command.\n");
    }//while
    if ( (wall_after = times(&tbuf_after)) == -1){
        perror("times");
        return 0;
    }
    printf("real:        %ju sec.\n"
    "user:       %ju sec.\n"
    "sys:        %ju sec.\n"
    "child user: %ju sec.\n"
    "child sys:  %ju sec.\n",
    (wall_after - wall_before),
    (tbuf_after.tms_utime),
    (tbuf_after.tms_stime),
    (tbuf_after.tms_cutime),
    (tbuf_after.tms_cstime));
    printf("done\n");
    return 0;
}//main

/*--- function definitions -------------------------------------------------*/

/**
* set up CPU limit and get first time reading
*
*
*/
void setup(const struct rlimit *new_limit, struct tms *tms_buf, clock_t *wall_before){
    if(setrlimit(RLIMIT_CPU, new_limit) ==-1){
        perror("error on setrlimit()");
        exit(EXIT_FAILURE);
    }//if
    if ( (*wall_before = times(tms_buf)) == -1){
        perror("setup: times");
        exit(EXIT_FAILURE);
    }
}

/**
*  print a list of jobs the non-terminated admitted jobs
*
*
*/
void print_jobs(JOB jobs[], int num_jobs){
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].state != terminated){
            printf("%d: (pid=%7d, cmd= %s)\n",
            jobs[i].index,
            jobs[i].pid,
            jobs[i].command);
        }

    } //for
}

/**
* run the given command and arguments (up to 4)
*
* Parameters:
*   char *out_str[]: array of string pointers,
*        first of which is the word run, second is the command,
*        and 2-6 are the arguments
*   int i: number of string pointers in out_str[]
*
* Returns:
*
*/
void run_command(char  *out_str[], int i){
    switch(i) {
        case 2:
        execlp(out_str[1], out_str[1], NULL);
        break;

        case 3:
        execlp(out_str[1], out_str[1], out_str[2],NULL);

        break;

        case 4:
        execlp(out_str[1], out_str[1], out_str[2], out_str[3],NULL);
        break;

        case 5:
        execlp(out_str[1], out_str[1], out_str[2], out_str[3], out_str[4],NULL);
        break;

        case 6:
        execlp(out_str[1], out_str[1], out_str[2], out_str[3], out_str[4], out_str[5],NULL);
        break;

        default:
        printf("too many arguments. (Limit of 4)\n"); //should never print
    }
    exit(EXIT_FAILURE);
}
