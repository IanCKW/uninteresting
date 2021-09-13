/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */
#include <sys/types.h>
#include <signal.h>
#include "myshell.h"
void load_info(void);
void print_info(void);

int pids[50];
char exit_status[50][10];
char current_status[50][10];
int num_processes = 0;

void my_init(void) {
    
}

void my_process_command(size_t num_tokens, char **tokens) {
    if (strcmp(tokens[0], "info"){
        print_info();
        return;
    }
    int isBackground = 0;
    if (tokens[num_tokens-2][0] == '&') { // '' for char!
        isBackground = 1;
        tokens = realloc(tokens, (num_tokens - 1) * sizeof(char *));
        num_tokens -= 1;
        tokens[num_tokens-1] = NULL;
    }

    int pid = fork();
    
    if (pid == -1){
        printf("Fork Error \n");
        exit(-1);
    }
    else if (pid == 0){
        int executed = execv(tokens[0], &tokens[0]);
        if (executed == -1){
            printf("%s not found \n", tokens[0]);
            exit(0);
        }
    }
    
    if (isBackground == 0){
        int exit_status;
        wait(&exit_status);
        load_info(pid, exit_status);
    }
    else{
        printf("Child[%i] in background \n", pid);
    }
    exit(0);
}

void my_quit(void) {
    printf("Goodbye!\n");
    char** exit_cmd = {"exit", "0", NULL};
    execl(exit_cmd[0], &exit_cmd[0]);
}

void load_info(int pid, status) {
    pids[num_processes] = pid;
    num_processes += 1;
}

void print_info(void) {
    
}
