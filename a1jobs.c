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

#define MAXJOBS 32

struct jobs {
    int index;
    int pid;
    char command[100];
    int state;
};

enum pstate{running, suspended, terminated};

void setup(const struct rlimit *new_limit, struct tms *tms_buf);

void delay(int secs)
{
    clock_t start_time = clock();

    // looping until required time is achieved
    while (clock() < start_time + secs*CLOCKS_PER_SEC){}
    return;
}


int main(int argc, char *argv[]){
    struct rlimit new_limit = {1,1};
    struct tms tbuf_before, tbuf_after;
    clock_t wall_before, wall_after;
    if(setrlimit(RLIMIT_CPU, &new_limit)==-1){
        perror("error on setrlimit()");
        return 0;
    }//if
    if ( (wall_before = times(&tbuf_before)) == -1){
        perror("erro on times() function call");
        return 0;
    }//if
    // printf("CLOCKS_PER_SEC: %ju\n", CLOCKS_PER_SEC);

    struct jobs myjobs[MAXJOBS];
    int numjobs = 0;
    char in_str[100];
    char in_str_cpy[100];
    char *out_str[6];
    int i;
    // printf("I'm still here! %d times.\n",counter++);
    while(true){
        printf("a1jobs[%i]: ",getpid());
        fgets(in_str, 100, stdin);
        strtok(in_str, "\n"); //replace final \n char with \0

        if (strcmp(in_str, "list") == 0){
            for (int i = 0; i < numjobs; i++) {
                if (myjobs[i].state != terminated){
                    printf("%d: (pid=%7d, cmd= %s)\n",
                    myjobs[i].index,
                    myjobs[i].pid,
                    myjobs[i].command);
                }

            } //for
        } //if (strcmp(in_str, "list") == 0)
        else if (strncmp(in_str, "run", 3) == 0 ) {
            if (numjobs >= MAXJOBS){
                printf("Job limit (%d) reached.\n", MAXJOBS);
                continue;
            }

            strcpy(in_str_cpy, in_str+4);
            if ( (out_str[0] = strtok(in_str," ")) != NULL){
                for ( i = 1; i < 6; i++){
                    if ( (out_str[i] = strtok(NULL, " \n")) == NULL){
                        break;
                    }
                } //exit condition: i == size of array;

                if (i < 2) {
                    printf("You have to tell me what to run.\n");
                    continue;
                }
                int retval;
                if ( (retval = fork()) == -1){
                    printf("error on fork");
                    continue;
                }
                if (retval == 0){ //child process
                    freopen("/dev/null","w",stdout);
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
            int susp_index = atoi(index_str);
            if (susp_index >= 0 && susp_index < numjobs && myjobs[susp_index].state != terminated){
                kill(myjobs[susp_index].pid,SIGSTOP);
                myjobs[susp_index].state = suspended;
                printf("suspended %d\n", susp_index);
            }

        }else if (strncmp(in_str, "resume ", 7) == 0){
            strtok(in_str, " ");
            char *index_str = strtok(NULL, " ");
            int susp_index = atoi(index_str);
            if (susp_index >= 0 && susp_index < numjobs && myjobs[susp_index].state != terminated){
                kill(myjobs[susp_index].pid,SIGCONT);
                myjobs[susp_index].state = running;
                printf("resumed %d\n", susp_index);
            }

        }else if (strncmp(in_str, "terminate ", 10) == 0){
            strtok(in_str, " ");
            char *index_str = strtok(NULL, " ");
            int susp_index = atoi(index_str);
            if (susp_index >= 0 && susp_index < numjobs && myjobs[susp_index].state != terminated){
                kill(myjobs[susp_index].pid,SIGKILL);
                myjobs[susp_index].state = terminated;
                printf("terminated %d\n", susp_index);
            }

        }else if (strcmp(in_str, "exit") == 0){
            printf("exiting\n");
            for (int i = 0; i < numjobs; i++){
                if (myjobs[i].state != terminated){
                    kill(myjobs[i].pid,SIGKILL);
                    myjobs[i].state = terminated;
                    printf("terminated %d\n", i);
                }
            }
            break;
        }else if (strcmp(in_str, "quit") == 0){
            printf("quitting\n");
            break;
        }else printf("unknown command.\n");
    }//while
    if ( (wall_after = times(&tbuf_after)) == -1){
        perror("error on times() function call");
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
    // for (long int i = 0; i<10000000000; i++);
    printf("done\n");
    return 0;
}//main
