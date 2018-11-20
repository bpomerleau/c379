/**
* ptree.h
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* struct and functions for keeping track of a process and its descendent
* process tree
*
*/

#ifndef PTREE_H
#define PTREE_H

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_TREE_BUF_SIZE 100


typedef struct ptree {
    pid_t *buf;
    int size;
    int buf_size;
} TREE;

TREE init_tree(pid_t root_pid);
void add_node(TREE * tree, pid_t pid);
bool is_node(TREE *tree, pid_t pid);
int kill_tree(TREE *tree);
void delete_tree(TREE *tree);
void print_tree(TREE *tree);

#endif
