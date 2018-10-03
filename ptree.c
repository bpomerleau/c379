#include "ptree.h"


TREE init_tree(pid_t root_pid){
    TREE tree;
    tree.size = 1;
    tree.buf_size = 20;
    tree.buf = (pid_t *) malloc(sizeof(pid_t)*tree.buf_size);
    tree.buf[0] = root_pid;
    return tree;
}

int add_node(TREE *tree, pid_t pid){

    if (tree->size == tree->buf_size){
        tree->buf_size *= 2;
        tree->buf = (pid_t *) realloc(tree->buf, tree->buf_size);
    }
    tree->buf[tree->size++] = pid;
    return 0;

}

bool is_node(TREE *tree, pid_t pid){
    for (int i = 0; i < tree->size; i++){
        if (pid == tree->buf[i]) return true;
    }
    return false;
}

int kill_tree(TREE * tree){
    int ret_code = 0;
    for (int i = tree->size; i > 0; i--){
        printf("Terminating process[%d]...\n",tree->buf[i]);
        if (kill(tree->buf[i], SIGKILL) == -1){
            perror("killtree: kill(pid,SIGKILL)");
            ret_code = -1;
        }
    }
    free(tree->buf);
    return ret_code;
}

void delete_tree(TREE *tree){
    free(tree->buf);
}

void print_tree(TREE *tree){
    for (int i = 0; i < tree->size; i++){
        printf("---%d\n",tree->buf[i]);
    }
}
