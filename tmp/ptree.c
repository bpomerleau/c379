/**
* ptree.c
*
* developed for CMPUT379 Assignment 1
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
* struct and functions for keeping track of a process and its descendent
* process tree
*
*/


#include "ptree.h"

/**
* initializes values and allocates space on heap for TREE
*
* Parameters:
*    pid_t root_pid: root of tree
*
* Returns:
*   TREE struct with values initialized
*/
TREE init_tree(pid_t root_pid){
    TREE tree;
    tree.size = 1;
    tree.buf_size = MAX_TREE_BUF_SIZE;
    tree.buf = (pid_t *) malloc(sizeof(pid_t)*tree.buf_size);
    tree.buf[0] = root_pid;
    return tree;
}

/**
* add a node to the tree
*
* Parameters:
*   TREE *tree: the tree struct to add a node to
*   pid_t pid: value of additional node
*
* Returns:
*
*/
void add_node(TREE *tree, pid_t pid){

    if (tree->size == tree->buf_size){
        tree->buf_size *= 2;
        tree->buf = (pid_t *) realloc(tree->buf, tree->buf_size);
    }
    tree->buf[tree->size++] = pid;

}

/**
* checks if a certain node is in the tree
*
* Parameters:
*    TREE tree: tree to inspect
*   pid_t pid: pid to search for
*
* Returns:
*   bool: true if node is in tree, false otherwise
*/
bool is_node(TREE *tree, pid_t pid){
    for (int i = 0; i < tree->size; i++){
        if (pid == tree->buf[i]) return true;
    }
    return false;
}

/**
* kills all of the processes tracked in the tree
*
* Parameters:
*    TREE tree: tree of processes to kill
*
* Returns:
*   int: 0 if all success, -1 if any one failed
*/
int kill_tree(TREE * tree){
    int ret_code = 0;
    for (int i = 1; i < tree->size; i++){
        printf("Terminating process[%d]...\n",tree->buf[i]);
        if (kill(tree->buf[i], SIGKILL) == -1){
            perror("killtree: kill(pid,SIGKILL)");
            ret_code = -1;
        }
    }
    free(tree->buf);
    return ret_code;
}

/**
* free the memory on the heap where pids are stored
* do this before deallocation space for tree struct
*
* Parameters:
*    TREE tree: tree whose memory we want to free
*
* Returns:
*
*/
void delete_tree(TREE *tree){
    free(tree->buf);
}

/**
* print list of processes in tree
*
* Parameters:
*    TREE tree: tree whose memory we want to free
*
* Returns:
*
*/
void print_tree(TREE *tree){
    printf("Monitored processes:\n");
    for (int i = 0; i < tree->size; i++){
        printf("---%d\n",tree->buf[i]);
    }
}
