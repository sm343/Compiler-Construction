#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "../lexical_analyzer/lexer_types.h"

typedef struct Node {
    stentry_t* key;
    struct Node* next;
} node_t;

node_t* make_node(stentry_t* key);
node_t* insert_tail_node(node_t* head, stentry_t* key);
void delete_tail_node(node_t* head);
void delete_list(node_t* head);

#endif
