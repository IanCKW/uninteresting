/*************************************
* Lab 1 Exercise 2
* Name: Ian Chan Kit Wai
* Student No: A0197089J
* Lab Group: 11
*************************************/

#include "node.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_size(list *lst);
node* get_tail(list *lst, int size_of_list);
node* get_previous(list* lst, int index);
node* get_next(list* lst, int index);

// Inserts a new node with data value at index (counting from head starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
    
    node *replacement = malloc(4);
    (*replacement).next = NULL;
    (*replacement).data = data;
    int size_of_list = get_size(lst);
    
    if (index < size_of_list){
        node *next_node = get_next(lst, index);
        replacement->next = next_node;
        //printf("issue is not here index < size");
    }
    if (index == 0){
        lst->head = replacement;
    }
    if (index > 0){
        node *previous_node = get_previous(lst, index);
        previous_node->next = replacement;
        //printf("issue is not here index > 0");
    }
    size_of_list++; // the list has grown by 1
    node *tail_node_pointer = get_tail(lst, size_of_list);
    tail_node_pointer->next = lst->head;

}
//helpful commands: fopen, fscanf, fclose. Can print to stderr using fprintf is the file does not exit
// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
    if (lst->head == NULL){
        return;
    }
    node* deleted = get_next(lst, index);
    int size_of_list = get_size(lst);
    if(size_of_list == 1){
        free(lst->head);
        lst->head = NULL;
        return;
    }
    else if (index == 0){
        lst->head = deleted->next;
    }
    else { // index > 0
        node *prev = get_previous(lst, index);
        prev->next = deleted->next; // if index is at the end of the list, prev-> will change to lst.head
    }
    free(deleted);
    size_of_list--; // the list has shrunk by 1
    node *tail_node_pointer = get_tail(lst, size_of_list);
    tail_node_pointer->next = lst->head;
}

// Rotates list by the given offset.
// Note: offset is guarenteed to be non-negative.
void rotate_list(list *lst, int offset) {
    int size_of_list = get_size(lst);
    int offset_mod = offset % size_of_list;
    node* new_head = get_next(lst, offset_mod);
    lst->head = new_head;
}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {
    int size_of_list = get_size(lst);
    if (lst->head == NULL | size_of_list == 1) {
        return;
    }
    node* curr = lst->head;
    node* next = curr->next;
    node* prev = NULL;
    for (int i=0;i<size_of_list;i++){
        curr->next = prev;
        prev = curr;
        curr = next;
        next = next->next;
    }
    lst->head = prev;
    curr->next = lst->head;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
    int list_size = get_size(lst);
    for (int i=0;i<list_size;i++){
        delete_node_at(lst,0);
    }
}

int get_size(list *lst){
    if (lst->head == NULL){
        return 0;
    }
    node* current = lst->head;
    if (current == NULL){
        return 0;
    }
    int size = 1;
    while (current->next != lst->head){ // it points to the front
        size += 1;
        current = current->next;
    }
    return size;
}

node* get_tail(list *lst, int size_of_list){
    node* tail = lst->head;
    for(int i = 0; i < size_of_list - 1; i++){
        tail = tail->next;
    }
    return tail;
}

node* get_previous(list *lst, int index){
    node* current = lst->head;
    for(int i = 0; i < index-1; i++){
        current = current->next;
    }
    return current;
}

node* get_next(list *lst, int index){
    node* current = lst->head;
    for(int i = 0; i < index; i++){
        current = current->next;
    }
    return current;
}