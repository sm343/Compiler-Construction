#include "LinkedList.h"

#include <stdio.h>
#include <stdlib.h>

node_t* make_node(stentry_t* key) {
    node_t* temp = (node_t*)malloc(sizeof(node_t));
    if (temp == NULL) {
        printf("Mem Alloc failed");
    }
    temp->key = key;
    temp->next = NULL;
    return temp;
}

node_t* insert_tail_node(node_t* head, stentry_t* key) {
    node_t* tail = make_node(key);

    if (head == NULL) {
        head = tail;
        return tail;
    }
    node_t* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = tail;
    return tail;
}

void delete_tail_node(node_t* head) {
    if (head == NULL) {
        return;
    }
    if (head->next == NULL) {
        free(head);
        head = NULL;
        return;
    }
    node_t* temp = head;
    while (temp->next->next != NULL) {
        temp = temp->next;
    }
    free(temp->next);
    temp->next = NULL;
}

void delete_list(node_t* head) {
    if (head == NULL) {
        return;
    }

    node_t* temp = head;
    while (temp != NULL) {
        node_t* next = temp->next;
        free(temp->key->lexeme);
        free(temp->key);
        free(temp);
        temp = next;
    }
}
