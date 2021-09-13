/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"


void my_init(void) {
    // Initialize what you need here
}

void my_process_command(size_t num_tokens, char **tokens) {
    int result = fork();
    if (pid == -1){
        printf("Fork Error \n");
        exit(-1);
    }
    else if (result == 0){
        execv(tokens[0], &tokens[0]);
    }
    wait(NULL);
}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command
}
