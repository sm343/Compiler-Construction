#include "transition_diagram.h"

#include "lexer_types.h"

state_t* create_state(int retract, bool exit, bool line_increment,
                      bool is_final) {
    state_t* state = (state_t*)malloc(sizeof(struct State));
    state->transitions.size = 0;
    state->transitions.capacity = 1;
    state->transitions.symbol =
        (char**)calloc(sizeof(char*), state->transitions.capacity);
    state->transitions.next_state =
        (int*)calloc(sizeof(int), state->transitions.capacity);
    state->retract = retract;
    state->exit = exit;
    state->line_increment = line_increment;
    state->is_final = is_final;
    return state;
}

void add_transition(state_t* state, char symbol[], int next_state) {
    if (state->transitions.capacity < state->transitions.size + 1) {
        state->transitions.symbol =
            (char**)realloc(state->transitions.symbol,
                            (state->transitions.capacity * 2) * sizeof(char*));
        state->transitions.next_state =
            (int*)realloc(state->transitions.next_state,
                          (state->transitions.capacity * 2) * sizeof(int));
        state->transitions.capacity *= 2;
    }
    state->transitions.size++;
    state->transitions.symbol[state->transitions.size - 1] =
        (char*)malloc(2 * sizeof(char));
    state->transitions.symbol[state->transitions.size - 1][0] = symbol[0];
    state->transitions.symbol[state->transitions.size - 1][1] = symbol[1];
    state->transitions.next_state[state->transitions.size - 1] = next_state;
}

state_t** create_transition_diagram() {
    // Create states
    state_t** td = (state_t**)malloc(NUM_STATES * sizeof(state_t*));
    for (int i = 0; i < NUM_STATES; i++) {
        int retract = 0;
        bool exit = false, line_increment = false, is_final = false;

        if (i == 62) exit = true;
        if (i == 34 || i == 61) line_increment = true;
        if (i == 5 || i == 9 || i == 18 || i == 22 || i == 28 || i == 36 ||
            i == 42 || i == 44 || i == 51 || i == 60 || i == 63)
            retract++;
        if (i == 60 || i == 63) retract++;
        if (i == 4 || i == 5 || i == 6 || i == 7 || i == 9 || i == 10 ||
            i == 17 || i == 18 || i == 22 || i == 25 || i == 28 || i == 31 ||
            i == 32 || i == 34 || i == 36 || i == 37 || i == 38 || i == 42 ||
            i == 44 || i == 45 || i == 47 || i == 49 || i == 51 || i == 52 ||
            i == 53 || i == 54 || i == 55 || i == 56 || i == 57 || i == 58 ||
            i == 59 || i == 60 || i == 61 || i == 62 || i == 63)
            is_final = true;

        td[i] = create_state(retract, exit, line_increment, is_final);
    }

    // Return Values
    td[4]->token = TK_ASSIGNOP;
    td[5]->token = TK_LT;
    td[6]->token = TK_LE;
    td[7]->token = TK_DOT;
    td[9]->token = TK_NUM;
    td[10]->token = TK_MINUS;
    td[17]->token = TK_RNUM;
    td[18]->token = TK_RNUM;
    td[22]->token = TK_FUNID;
    td[25]->token = TK_OR;
    td[28]->token = TK_RUID;
    td[31]->token = TK_AND;
    td[32]->token = TK_DIV;
    td[34]->token = TK_COMMENT;
    td[36]->token = -1;
    td[37]->token = TK_CL;
    td[38]->token = TK_OP;
    td[42]->token = TK_ID;
    td[44]->token = TK_FIELDID;
    td[45]->token = TK_PLUS;
    td[47]->token = TK_NE;
    td[49]->token = TK_EQ;
    td[51]->token = TK_GT;
    td[52]->token = TK_GE;
    td[53]->token = TK_COLON;
    td[54]->token = TK_SEM;
    td[55]->token = TK_NOT;
    td[56]->token = TK_MUL;
    td[57]->token = TK_COMMA;
    td[58]->token = TK_SQR;
    td[59]->token = TK_SQL;
    td[60]->token = TK_LT;
    td[61]->token = -1;
    td[62]->token = -1;
    td[63]->token = TK_NUM;

    // Create transitions
    add_transition(td[0], "<<", 1);
    add_transition(td[0], "..", 7);
    add_transition(td[0], "09", 8);
    add_transition(td[0], "--", 10);
    add_transition(td[0], "__", 19);
    add_transition(td[0], "@@", 23);
    add_transition(td[0], "##", 26);
    add_transition(td[0], "**", 56);
    add_transition(td[0], "[[", 59);
    add_transition(td[0], "&&", 29);
    add_transition(td[0], "]]", 58);
    add_transition(td[0], ",,", 57);
    add_transition(td[0], "//", 32);
    add_transition(td[0], "%%", 33);
    add_transition(td[0], "))", 37);
    add_transition(td[0], "  ", 35);
    add_transition(td[0], "\t\t", 35);
    add_transition(td[0], "((", 38);
    add_transition(td[0], "bd", 39);
    add_transition(td[0], "\n\n", 61);
    add_transition(td[0], "\r\r", 61);
    add_transition(td[0], "az", 43);
    add_transition(td[0], "++", 45);
    add_transition(td[0], "!!", 46);
    add_transition(td[0], "==", 48);
    add_transition(td[0], ">>", 50);
    add_transition(td[0], "::", 53);
    add_transition(td[0], ";;", 54);
    add_transition(td[0], "~~", 55);

    add_transition(td[1], "--", 2);
    add_transition(td[1], "==", 6);
    add_transition(td[1], "\0\x7f", 5);

    add_transition(td[2], "--", 3);
    add_transition(td[2], "\0\x7f", 60);

    add_transition(td[3], "--", 4);

    add_transition(td[8], "09", 8);
    add_transition(td[8], "..", 11);
    add_transition(td[8], "\0\x7f", 9);

    add_transition(td[11], "09", 12);
    add_transition(td[11], "\0\x7f", 63);

    add_transition(td[12], "09", 13);

    add_transition(td[13], "EE", 14);
    add_transition(td[13], "\0\x7f", 18);

    add_transition(td[14], "++", 15);
    add_transition(td[14], "--", 15);
    add_transition(td[14], "09", 16);

    add_transition(td[15], "09", 16);

    add_transition(td[16], "09", 17);

    add_transition(td[19], "az", 20);
    add_transition(td[19], "AZ", 20);

    add_transition(td[20], "az", 20);
    add_transition(td[20], "AZ", 20);
    add_transition(td[20], "09", 21);
    add_transition(td[20], "\0\x7f", 22);

    add_transition(td[21], "09", 21);
    add_transition(td[21], "\0\x7f", 22);

    add_transition(td[23], "@@", 24);

    add_transition(td[24], "@@", 25);

    add_transition(td[26], "az", 27);

    add_transition(td[27], "az", 27);
    add_transition(td[27], "\0\x7f", 28);

    add_transition(td[29], "&&", 30);

    add_transition(td[30], "&&", 31);

    add_transition(td[33], "\n\n", 34);
    add_transition(td[33], "\r\r", 34);
    add_transition(td[33], "\0\x7f", 33);

    add_transition(td[35], "  ", 35);
    add_transition(td[35], "\t\t", 35);
    add_transition(td[35], "\0\x7f", 36);

    add_transition(td[39], "27", 40);
    add_transition(td[39], "az", 43);
    add_transition(td[39], "\0\x7f", 44);

    add_transition(td[40], "27", 41);
    add_transition(td[40], "bd", 40);
    add_transition(td[40], "\0\x7f", 42);

    add_transition(td[41], "27", 41);
    add_transition(td[41], "\0\x7f", 42);

    add_transition(td[43], "az", 43);
    add_transition(td[43], "\0\x7f", 44);

    add_transition(td[46], "==", 47);

    add_transition(td[48], "==", 49);

    add_transition(td[50], "==", 52);
    add_transition(td[50], "\0\x7f", 51);

    return td;
}

int get_next_state(int curr_state, char symbol, state_t** td) {
    state_t* state = td[curr_state];
    transitions_t transitions = state->transitions;
    int num_tr = transitions.size;

    for (int i = 0; i < num_tr; i++) {
        char* transition = transitions.symbol[i];
        if (symbol >= transition[0] && symbol <= transition[1]) {
            return transitions.next_state[i];
        }
    }

    return -1;
}

void clear_transition_diagram(state_t** td) {
    for (int i = 0; i < NUM_STATES; i++) {
        state_t* state = td[i];
        int size = state->transitions.size;
        if (size > 0) {
            for (int j = 0; j < size; j++) {
                free(state->transitions.symbol[j]);
            }
            free(state->transitions.symbol);
            free(state->transitions.next_state);
        }
        free(state);
    }
    free(td);
}
