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
#include <stdio.h>
#include <string.h>

#define BACKGROUND 1
#define NOT_BACKGROUND 0
#define EXIT 0
#define RUNNING 1
#define TERMINATING 2
#define RUNNING_STATUS -1
#define DEFAULT_EXIT_STATUS 0

void load_info(int , int );
void check_running();
void print_info(void);
void execute_tokens(char **, int );

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
    else if (strcmp(tokens[0], "wait") == 0){
        wait_for(tokens[1]);
        return;
    }
    else if (strcmp(tokens[0], "terminate") == 0){
        terminate_for(tokens[1]);
        return;
    }
    else if (strcmp(tokens[num_tokens-2], "&") == 0){
        set_background(num_tokens, tokens); // arrays are passed by address! no need to use &
        execute_tokens(tokens, BACKGROUND);
        return;
    }
    else if (check_chain(num_tokens,tokens) != 0){
        int count = check_chain(num_tokens,tokens);
        handle_chain(num_tokens, tokens);
    }
    else{
        execute_tokens(tokens, NOT_BACKGROUND);
        return;
    }
}

void execute_tokens(char **tokens, int background){
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
    if (background == NOT_BACKGROUND){
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

void set_background(size_t num_tokens, char **tokens){
    tokens = realloc(tokens, (num_tokens - 1) * sizeof(char *));
    num_tokens -= 1;
    tokens[num_tokens-1] = NULL;
}


void my_quit(void) {
    for (int i=0; i<num_processes; i++){
        if (pid_status[i][1] == RUNNING){
            kill(pid_status[i][0], SIGTERM);
        }
    }
    int count =0;
    while (count < num_processes){
        if (waitpid(pid_status[count % num_processes][0], NULL, WNOHANG ) == 0){
            count = 0;
        }
        else{count ++;}
    }
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
        if (pid_status[i][1] == RUNNING | pid_status[i][1] == TERMINATING) {
            int *status_ptr;            
            if(waitpid(pid_status[i][0], &status_ptr, WNOHANG ) != 0){
                pid_status[i][1] = EXIT;
                pid_status[i][2] = DEFAULT_EXIT_STATUS; // this for now
            }
        }

    }
}

void print_info(void) {
    char *status = malloc(256); 
    char *exit_status = malloc(256);
    for (int i=0; i<num_processes; i++){    
        int pid = pid_status[i][1];
        if (pid == RUNNING){
            strcpy(status, "Running"); // this is useful for reallocating strings eh
            strcpy(exit_status, "");
        }
        else if (pid == TERMINATING){
            strcpy(status, "Terminating"); // this is useful for reallocating strings eh
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

void wait_for(char *pid_str){
    int pid;
    sscanf(pid_str, "%d", &pid);
    int status_ptr;
    // check if pid is valid
    for (int i = 0; i < num_processes; i++){
        if (pid_status[i][0] == pid & pid_status[i][1] == RUNNING){
            waitpid(pid, &status_ptr,WCONTINUED);
        }
    }
    return;
}

void terminate_for(char *pid_str){
    int pid;
    sscanf(pid_str, "%d", &pid);
    for (int i = 0; i < num_processes; i++){
        if (pid_status[i][0] == pid & pid_status[i][1] == RUNNING){
            kill(pid,SIGTERM);
            pid_status[i][1] = TERMINATING;
        }
    }
    return;
}

int check_chain(size_t num_tokens,char **tokens){
    int count = 0;
    for (int i = 0; i < num_tokens - 1; i++){ // last element is NULL
        if (strcmp(tokens[i], "&&") == 0){
            count++;
        }
    }
    return count;
}

void handle_chain(size_t num_tokens,char **tokens){
    char **sub_tokens = malloc((num_tokens) * sizeof(char *));
    for (int i = 0; i < num_tokens; i++){
        sub_tokens[i] = malloc(256);
    }
    int i = 0;
    int j = 0;
    while(i < num_tokens){
        if (!tokens[i]){
            sub_tokens[j] = NULL;
            execute_tokens(sub_tokens, NOT_BACKGROUND);
            j = 0;
        }
        else if (strcmp(tokens[i], "&&") == 0) {
            sub_tokens[j] = NULL;
            execute_tokens(sub_tokens, NOT_BACKGROUND);
            j = 0;
        }
        else {
            strcpy(sub_tokens[j], tokens[i]); 
            j++;
        }
        i++;
    }

    for (int i = 0; i < num_tokens; i++){
        free(sub_tokens[i]);
    }
    free(sub_tokens);
}