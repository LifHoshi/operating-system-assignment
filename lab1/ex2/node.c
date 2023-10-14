/*************************************
* Lab 1 Exercise 2
* Name:Wang Lifu
* Student No:A0245880X
* Lab Group: 06
*************************************/

#include "node.h"
#include <stdlib.h>
#include <stdio.h>


// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit 


// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
  node *cur_node = lst -> head;
  node *new_node = (node*)malloc(sizeof(node));
  new_node -> data = data;

  if (index == 0) {
    new_node -> next = cur_node;
    lst -> head = new_node;
    return;
  }

  for(int i = 0; i < index - 1; i++) {
    cur_node = cur_node -> next;
  }
  
  if (cur_node -> next != NULL) {
    new_node -> next = cur_node -> next;
    cur_node -> next = new_node;
  } else {
    new_node -> next = NULL;
    cur_node -> next = new_node;
  }

  return;

}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
  node *cur_node = lst -> head;
  if (index == 0) {
    lst -> head = cur_node -> next;
    free(cur_node);
    return;
  }

  for (int i = 0; i < index - 1; i++) {
    cur_node = cur_node -> next;
  }

  if (cur_node -> next != NULL) {
    node *del_node = cur_node -> next;
    cur_node -> next = del_node -> next;
    free(del_node);

  } else {
    node *del_node = cur_node -> next;
    cur_node -> next = NULL;
    free(del_node);
  }


 return;
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
//Printing of the index is already handled in ex2.c
int search_list(list *lst, int element) {
    node *cur_node = lst -> head;
    int count = 0;

    if (cur_node == NULL) {
      return -2;
    }
     
    while(cur_node != NULL) {
      if (cur_node -> data == element) {
        return count;
      } else {
        cur_node = cur_node -> next;
        count += 1;
      }
    }

    return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {
  node *cur_node = lst -> head;
  node *next_node = NULL;
  node *pre_node = NULL;
  
  while (cur_node != NULL) {
    next_node = cur_node -> next;
    cur_node -> next = pre_node;
    pre_node = cur_node;
    cur_node = next_node;
  }
  
  lst -> head = pre_node;
  return;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
  node *cur_node = lst -> head;
  node *temp_node = NULL;

  while(cur_node != NULL) {
    temp_node = cur_node;
    cur_node = cur_node -> next;
    free(temp_node);
  }

  lst -> head = NULL;
return;
}
