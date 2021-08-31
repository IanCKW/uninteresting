/*************************************
* Lab 1 Exercise 3
* Name: ian Chan
* Student No: A0197089J
* Lab Group: 11
*************************************/
#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define ROTATE_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define MAP 6
#define PRINT_LIST 7

void print_list(list *lst);
void update_functions();
void run_instruction(list *lst, int instr, char line[]);

int main(int argc, char **argv) { // double pointer as it's a nested array
    if (argc != 2) {
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
        exit(1);
    }
    // HANDLE EMPY FILES

    // We read in the file name provided as argument
    char *fname = argv[1];

    //update_functions();

    // Rest of code logic here
    FILE *fPointer;
    fPointer = fopen(fname,"r"); // returns a pointer to the file descriptor. "r" means read only
    if (fPointer == NULL){
        fclose(fPointer);
        return;
    }
    list *lst = (list *)malloc(sizeof(list));
    lst->head = NULL;

    char line[255];
    int instr;
    while(!feof(fPointer)){ //while the file pointer is not at the end of the file
        fgets(line,255,fPointer);
        sscanf(line,"%d",&instr);
        if (strlen(line) == 0){
            break;
        }
        run_instruction(lst, instr, line);
        line[0] = '\0';
    }
    
    reset_list(lst);
    free(lst);
    fclose(fPointer);
}

void run_instruction(list *lst, int instr, char line[]) {
    int instruction, index, data, offset, map_func;
    switch (instr) {
        case PRINT_LIST:
            print_list(lst);
            break;
        case INSERT_AT:
            sscanf(line, "%d %d %d", &instruction, &index, &data);
            insert_node_at(lst, index, data);
            break;
        case DELETE_AT:
            sscanf(line, "%d %d", &instruction, &index);
            delete_node_at(lst, index);
            break;
        case ROTATE_LIST:
            sscanf(line, "%d %d", &instruction, &offset);
            rotate_list(lst, offset);
            break;
        case REVERSE_LIST:
            reverse_list(lst);
            break;
        case RESET_LIST:
            reset_list(lst);
            break;
        case SUM_LIST:
            printf("%ld\n",sum_list(lst));
            break;
        case MAP:
            sscanf(line, "%d %d", &instruction, &map_func);
            map(lst, func_list[map_func]);
    }
}

// Prints out the whole list in a single line
void print_list(list *lst) {
    if (lst->head == NULL) {
        printf("[ ]\n");
        return;
    }

    printf("[ ");
    node *curr = lst->head;
    do {
        printf("%d ", curr->data);
        curr = curr->next;
    } while (curr != lst->head);
    printf("]\n");
}
