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

// Inserts a new node with data value at index (counting from head starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
    node *current = (*lst).head;
    for (int i = 0; i < index - 1; i++)
    {
        current = (*current).next;
    }
    node *previous = &current;
    node *replaced = (*previous).next;
    node *next = (*replaced).next;

    node *replacement;
    replacement = malloc(2);
    (*replacement).data = data;
    (*previous).next = &replacement;
    (*replacement).next = &next;

    printf("hello there");
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
}

// Rotates list by the given offset.
// Note: offset is guarenteed to be non-negative.
void rotate_list(list *lst, int offset) {
}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
}
