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

int main(int argc, char *argv[]){
    char in_str[100];
    char *out_str[6];
    int i = 0;
    while(true){
        printf("input: ");
        fgets(in_str, 100, stdin);
        strtok(in_str, " ");
        char *index = strtok(NULL, " ");
        printf("%s", index);
        printf("%d\n", atoi(index));
        // out_str[i] = strtok(in_str, " ");
        // printf("%s\n%s\n",in_str, out_str[i]);
        // for(i = 1; i < 6; i++){
        //     out_str[i] = strtok(NULL, " ");
        //     printf("%s\n%s\n",in_str, out_str[i]);
        // }
    }
}
