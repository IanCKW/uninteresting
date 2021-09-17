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
#include <unistd.h>
#include <fcntl.h>

#define BACKGROUND 1
#define NOT_BACKGROUND 0
#define EXIT 0
#define RUNNING 1
#define TERMINATING 2
#define RUNNING_STATUS -1
#define DEFAULT_EXIT_STATUS 0
#define EXECUTED 0
#define NOT_EXECUTED -1
#define EXECUTED_FAILURE -2



void load_info(int , int );
void check_running();
void print_info(void);
void execute_tokens(char **, int );

int pid_status[50][3];
int num_processes = 0;
int execute_status = EXECUTED;


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
        set_background(num_tokens, tokens);
        execute_tokens(tokens, BACKGROUND);
        return;
    }
    else if (check_chain(num_tokens, tokens) > 0){
        handle_chain(num_tokens, tokens);
    }
    else{
        execute_tokens(tokens, NOT_BACKGROUND);
        return;
    }
}

void execute_tokens(char **tokens, int background){
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    if(!check_valid_program(tokens)) return;
    if(!check_in_file(tokens)) return;
    handle_write_errors(tokens);
    handle_write_output(tokens);
    int pid = fork_and_exec(tokens);
    execute_status = EXECUTED;
    load_process(pid, background);

    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    if(background == BACKGROUND) {printf("Child[%i] in background \n", pid);}
    return;
}

int check_valid_program(char **tokens){
    if (access(tokens[0], X_OK) != 0) {
        printf("%s not found \n", tokens[0]);
        execute_status = NOT_EXECUTED;
        return 0;
    }
    return 1;
}

int check_in_file(char **tokens){
    if (!tokens[1] || !tokens[2]) return 1;
    if (strcmp(tokens[1],"<") ==0 && access(tokens[2],F_OK) != 0){
        printf("%s does not exist \n", tokens[2]);
        return 0;
    }
    return 1;
}

void handle_file_input(char **tokens){
    int count = 0;
    int i =0;
    while(tokens[i] != NULL){
        if (strcmp(tokens[i],"<")==0) count = i;
        i++;
    }
    if (count > 0){
        int f = open( tokens[2], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        if (f == -1) { 
            perror ("open failed");
            close(f);
            return;
        }
        if( dup2(f, STDIN_FILENO) == -1) perror("dup2 failed");
        close(f);
        tokens[1] = NULL;
    }
}

void handle_write_output(char **tokens){
    int count = 0;
    int i = 0;
    while(tokens[i] != NULL){
        if(strcmp(tokens[i],">")==0) count = i;
        i++;
    }
    if (count > 0){
        int f = open(tokens[count+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (f == -1){
            perror("creat failed");
            close(f);
            return;
        }
        if( dup2(f, STDOUT_FILENO) == -1) perror("dup2 failed");
        tokens[count] = NULL;
        close(f);
    }
}

void handle_write_errors(char **tokens){
    int count = 0;
    int i =0;
    while(tokens[i] != NULL){
        if (strcmp(tokens[i],"2>")==0) count = i;
        i++;
    }
    if (count > 0){
        int f = open(tokens[count+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (f == -1){
            perror("creat failed");
            close(f);
            return;
        }
        if (dup2(f, STDERR_FILENO) == -1) perror("dup2 failed");
        tokens[count] = NULL;
        close(f);
    }
}



int fork_and_exec(char ** tokens){
    
    int pid = fork();
    if (pid == -1){
        printf("Fork Error \n");
        exit(-1);
    }
    else if (pid == 0){
        handle_file_input(tokens);
        int executed = execvp(tokens[0], &tokens[0]);
        exit(0);
    }
    return pid;
}

void load_process(int pid, int background){
    if (background == NOT_BACKGROUND){
        int exit_status;
        wait(&exit_status);
        if (WIFEXITED(exit_status))load_info(pid, WEXITSTATUS(exit_status));         
        else load_info(pid, 0); 
        if (exit_status != 0 ) execute_status = EXECUTED_FAILURE;
    }
    else{
        load_info(pid, RUNNING_STATUS);
    }
}

void set_background(int num_tokens, char **tokens){
    tokens = realloc(tokens, (num_tokens - 1) * sizeof(char *));
    tokens[num_tokens-2] = NULL;
}


void my_quit(void) {
    for (int i=0; i<num_processes; i++){
        if (pid_status[i][1] == RUNNING){
            kill(pid_status[i][0], SIGTERM);
        }
    }
    int count =0;
    while (count < num_processes){
        while(waitpid(pid_status[count][0], NULL, WNOHANG ) == 0);
        count ++;
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
    char *status = malloc(60); 
    char *exit_status = malloc(60);
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

int check_chain(int num_tokens,char **tokens){
    int count = 0;
    for (int i = 0; i < num_tokens - 1; i++){ // last element is NULL
        if (strcmp(tokens[i], "&&") == 0){
            count++;
        }
    }
    return count;
}

void handle_chain(int num_tokens,char **tokens){
    char **sub_tokens = malloc(num_tokens * sizeof(tokens[0]));
    for (int i = 0; i < num_tokens; i++){
        sub_tokens[i] = malloc(num_tokens * sizeof(tokens[0]));
    }

    int i = 0;
    int j = 0;
    while(i < num_tokens){
        while(j < num_tokens){
            if(tokens[i] == NULL || strcmp(tokens[i],"&&") == 0) {
                sub_tokens[j] = NULL;
                break;
            }
            strcpy(sub_tokens[j], tokens[i]);
            j++;
            i++;
        }
        if (execute_status == EXECUTED_FAILURE) printf("%s failed \n", sub_tokens[0]);
        if (execute_status != EXECUTED) break;
        
        for (int k = 0; k < num_tokens; k++){
            free(sub_tokens[k]);
            sub_tokens[k] = malloc(num_tokens * sizeof(tokens[0]));
        }
        j = 0;
        i++;
    }

    for (int i = 0; i < num_tokens; i++){
        free(sub_tokens[i]);
    }
    free(sub_tokens);
}

// fPointer = fopen(fname,"r"); // returns a pointer to the file descriptor. "r" means read only
//     if (fPointer == NULL){
//         fclose(fPointer);
//         return;
//     }

// int fw=open("chinaisbetter.txt", O_APPEND|O_WRONLY);
// in child process
// dup2(fileno(someopenfile), STDIN_FILENO);
// STDIN_FILENO
// STDOUT_FILENO
// STDERR_FILENO

//O_RDONLY, O_WRONLY, or O_RDWR.  These request opening the
//      file read-only, write-only, or read/write, respectively

// if (dup2(in, 0) == -1) {
//     perror("dup2 failed");
//     exit(1);
// }

// use execvp
//creat()
    //    A call to creat() is equivalent to calling open() with flags
    //    equal to O_CREAT|O_WRONLY|O_TRUNC.