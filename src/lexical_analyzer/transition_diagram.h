#ifndef TRANSITION_DIAGRAM_H
#define TRANSITION_DIAGRAM_H

#include <stdbool.h>
#include <stdlib.h>

#define NUM_STATES 64

typedef struct Transitions transitions_t;
typedef struct State state_t;

struct Transitions {
    int capacity;
    int size;
    char** symbol;
    int* next_state;
};

struct State {
    transitions_t transitions;
    int retract;
    int token;
    bool exit;
    bool is_final;
    bool line_increment;
};

state_t* create_state(int retract, bool exit, bool line_increment,
                      bool is_final);
void add_transition(state_t* state, char symbol[], int next_state);
state_t** create_transition_diagram();
int get_next_state(int curr_state, char symbol, state_t** td);
void clear_transition_diagram(state_t** td);

#endif
