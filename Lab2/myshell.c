/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */
#include <sys/types.h>
#include <signal.h>
#include "myshell.h"
#include <errno.h>
#include <sys/wait.h>

#define EXIT 0
#define RUNNING 1
#define RUNNING_STATUS -1
#define DEFAULT_EXIT_STATUS 0

//void load_info(void);
//void print_info(void);

int pid_status[50][3];
int num_processes = 0;


void my_init(void) {
    
}

void my_process_command(size_t num_tokens, char **tokens) {
    if (strcmp(tokens[0], "info") == 0){
        check_running();
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
            return;
        }
        exit(0);
    }
    
    if (isBackground == 0){
        int exit_status;
        wait(&exit_status);
        if (WIFEXITED(exit_status)) { load_info(pid, WEXITSTATUS(exit_status));}            
        else { load_info(pid, 0); }
    }
    else{
        printf("Child[%i] in background \n", pid);
        load_info(pid, RUNNING_STATUS);
    }
}

void my_quit(void) {
    printf("Goodbye!\n");
    exit(0);
    
}

void load_info(int pid, int status) {
    pid_status[num_processes][0] = pid;
    if (status != RUNNING_STATUS){
        pid_status[num_processes][1] = EXIT;
        pid_status[num_processes][2] = status;
    }
    else{
        pid_status[num_processes][1] = RUNNING;
        pid_status[num_processes][2] = RUNNING;
    }
    num_processes += 1;
}

void check_running(){
    for (int i=0; i < num_processes; i++){
        if (pid_status[i][1] == RUNNING) {
            int *status_ptr;            
            if(waitpid(pid_status[i][0], &status_ptr, WNOHANG ) != 0){
                pid_status[i][1] = EXIT;
                pid_status[i][2] = status_ptr;
            }
        }
    }
}

void print_info(void) {
    char *status = malloc(256); 
    char *exit_status = malloc(256);
    for (int i=0; i<num_processes; i++){    
        if (pid_status[i][1] == RUNNING){
            strcpy(status, "Running"); // this is useful for reallocating strings eh
            strcpy(exit_status, "");
        }
        else{
            strcpy(status, "Exited");
            sprintf(exit_status, "%d", pid_status[i][2]);
        }
        printf("[%i] %s %s\n", pid_status[i][0], status, exit_status);
    }
    free(status);
    free(exit_status);

}

