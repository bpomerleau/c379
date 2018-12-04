/**
* io_cntrlr.c
*
* developed for CMPUT379 Assignment 4
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*/

#include "io_cntrlr.h"
#include <string.h>

int open_file(FILE **file_stream, char *filename) {
    if ((*file_stream = fopen(filename, "r")) == NULL) {perror("Opening file"); return 0;}
    else return 1;
}

void get_line(char *line, FILE *file_stream, bool *eof){
    char cpy[LINE_BUFFER];
    char *tok;
    while (fgets(line, LINE_BUFFER, file_stream) != NULL) {
        strcpy(cpy, line);
        if ((tok = strtok(cpy, " \n")) == NULL) continue;
        else if (tok[0] == '#') continue;
        else return;
    }
    *eof = true;
}

void close_file(FILE *file_stream){
    if (fclose(file_stream) < 0) perror("Closing file");
}

//eof
