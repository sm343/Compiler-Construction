#include "lexer.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "../helper.h"
#include "lexer_types.h"
#include "transition_diagram.h"

int populate_twin_buffers(int begin, int forward, char* buffer, int* fptr,
                          int* prev_buf, bool is_swap) {
    int active = begin / BUF_SIZE;
    int next = forward / BUF_SIZE;
    if (next == *prev_buf || next == active) {
        return -1;
    }
    if (is_swap) {
        return -2;
    }
    *prev_buf = next;
    bzero(buffer + (BUF_SIZE * next), BUF_SIZE);
    return read(*fptr, buffer + (BUF_SIZE * next), BUF_SIZE);
}

void print_lexical_op(int opfptr, tokeninfo_t* tk_info) {
    printf("Line No. %d\t|", tk_info->line_no);
    if (tk_info->token_type < -1) {
        printf(ANSI_COLOR_RED "  LEXICAL ERROR! Pattern <%s>:",
               tk_info->lexeme);
        switch (tk_info->token_type) {
            case -2:
                printf(" Invalid lexeme\n" ANSI_COLOR_RESET);
                break;
            case -3:
                printf(
                    " Variable Identifier length exceeds max length of 20 "
                    "characters\n" ANSI_COLOR_RESET);
                break;
            case -4:
                printf(
                    " Function Identifier length exceeds max length of 30 "
                    "characters\n" ANSI_COLOR_RESET);
                break;
            default:
                printf("Internal Error. Bye" ANSI_COLOR_RESET);
                exit(-1);
        }
        return;
    }
    printf("  Token %-20s|", token_str[tk_info->token_type]);
    printf("  Lexeme '%s'\n", tk_info->lexeme);
}

tokeninfo_t get_next_token(char* ip_filename, ht_t* symbol_table,
                           char* op_filename) {
    static unsigned int begin = 0;
    static unsigned int forward = 0;
    static int res_read;
    static bool is_lexer_init = false;
    static bool is_end = false;
    static char* buffer = NULL;
    static int ip_fptr = -1;
    static int op_fptr = -1;
    static state_t** td = NULL;
    static int curr_state = 0;
    static int line_number = 1;
    static int prev_buf = 0;
    tokeninfo_t ret_token = {0, {0}};
    bool is_swap = false;
    if (!is_lexer_init) {
        buffer = (char*)calloc(TBUF_SIZE, sizeof(char));
        is_lexer_init = true;
        fclose(fopen(op_filename, "w"));
        ip_fptr = open(ip_filename, O_RDONLY);
        op_fptr = open(op_filename, O_RDWR | O_CREAT, 0666);
        td = create_transition_diagram();
        res_read = read(ip_fptr, buffer, BUF_SIZE);
    }

    if (res_read < BUF_SIZE && forward % BUF_SIZE >= res_read) {
        is_end = true;
    }

    if (is_end) {
        if (buffer != NULL) free(buffer);
        if (ip_fptr != -1) close(ip_fptr);
        if (op_fptr != -1) close(op_fptr);
        if (td != NULL) clear_transition_diagram(td);
        buffer = NULL;
        ip_fptr = -1;
        op_fptr = -1;
        td = NULL;
        ret_token.token_type = -1;
        return ret_token;
    }

start_parsing:
    while (!td[curr_state]->is_final) {
        char curr_char = buffer[forward];

        if (curr_char == '\0') {
            is_end = true;
            free(buffer);
            close(ip_fptr);
            close(op_fptr);
            clear_transition_diagram(td);
            buffer = NULL;
            ip_fptr = -1;
            op_fptr = -1;
            td = NULL;
            ret_token.token_type = -1;
            return ret_token;
        }

        curr_state = get_next_state(curr_state, curr_char, td);

        if (curr_state == -1) {
            int val_len = forward - begin + 1;
            if (val_len < 0) {
                val_len = TBUF_SIZE - begin + forward + 1;
            } else if (val_len == 1) {
                val_len++;
                forward += 1;
            }
            forward = forward % TBUF_SIZE;
            int tmp = populate_twin_buffers(begin, forward, buffer, &ip_fptr,
                                            &prev_buf, is_swap);
            if (tmp == -2) {
                printf(ANSI_COLOR_RED
                       "Lexeme greater that 1024 characters exists at line "
                       "number: %d\n" ANSI_COLOR_RESET,
                       line_number);
                exit(-1);
            }
            if (tmp != -1) {
                res_read = tmp;
                is_swap = true;
            }
            char value[val_len];
            value[val_len - 1] = '\0';
            for (int i = 0; i < val_len - 1; i++) {
                value[i] = buffer[begin];
                begin = (begin + 1) % TBUF_SIZE;
            }
            begin = forward;
            curr_state = 0;
            ret_token.token_type = -2;
            ret_token.line_no = line_number;
            ret_token.lexeme = (char*)malloc(sizeof(char) * val_len);
            strcpy(ret_token.lexeme, value);
            print_lexical_op(op_fptr, &ret_token);
            return ret_token;
        } else if (!td[curr_state]->is_final) {
            forward = (forward + 1) % TBUF_SIZE;
            int tmp = populate_twin_buffers(begin, forward, buffer, &ip_fptr,
                                            &prev_buf, is_swap);
            if (tmp == -2) {
                printf(ANSI_COLOR_RED
                       "Lexeme greater that 1024 characters exists at line "
                       "number: %d\n" ANSI_COLOR_RESET,
                       line_number);
                exit(-1);
            }
            if (tmp != -1) {
                res_read = tmp;
                is_swap = true;
            }
        }
    }

    int token = td[curr_state]->token;
    forward -= td[curr_state]->retract - 1;
    int val_len = forward - begin + 1;

    if (val_len < 0) {
        val_len = TBUF_SIZE - begin + forward + 1;
    }

    forward = forward % TBUF_SIZE;

    int tmp = populate_twin_buffers(begin, forward, buffer, &ip_fptr, &prev_buf,
                                    is_swap);
    if (tmp == -2) {
        printf(ANSI_COLOR_RED
               "Lexeme greater that 1024 characters exists at line "
               "number: %d\n" ANSI_COLOR_RESET,
               line_number);
        exit(-1);
    }

    if (tmp != -1) {
        res_read = tmp;
        is_swap = true;
    }

    char value[val_len];
    value[val_len - 1] = '\0';
    for (int i = 0; i < val_len - 1; i++) {
        value[i] = buffer[begin];
        begin = (begin + 1) % TBUF_SIZE;
    }
    if (td[curr_state]->line_increment) line_number++;

    if (token >= 0 && token != TK_COMMENT) {
        ret_token.token_type = token;
        switch (token) {
            case TK_NUM:
                ret_token.info.num_val = atol(value);
                break;
            case TK_RNUM:
                ret_token.info.rnum_val = atof(value);
                break;
            case TK_FUNID:
            case TK_FIELDID:
            case TK_ID:
            case TK_RUID: {
                if (token == TK_ID && val_len > 20) {
                    ret_token.token_type = -3;
                    break;
                } else if (token == TK_FUNID && val_len > 30) {
                    ret_token.token_type = -4;
                    break;
                }
                char* val_heap = (char*)calloc(val_len, sizeof(char));
                strcpy(val_heap, value);
                stentry_t* entry = (stentry_t*)calloc(1, sizeof(stentry_t));
                entry->lexeme = val_heap;
                entry->token_type = token;
                stentry_t* existing_entry = insert_entry(symbol_table, entry);
                if (existing_entry != NULL) {
                    free(entry);
                    entry = existing_entry;
                }
                ret_token.token_type = entry->token_type;
                ret_token.info.stentry = entry;
                break;
            }
        }
    } else {
        curr_state = 0;
        goto start_parsing;
    }

    if (!td[curr_state]->exit)
        curr_state = 0;
    else {
        is_end = true;
        free(buffer);
        close(ip_fptr);
        clear_transition_diagram(td);
        buffer = NULL;
        ip_fptr = -1;
        td = NULL;
    }

    ret_token.line_no = line_number;
    ret_token.lexeme = (char*)malloc(sizeof(char) * val_len);
    strcpy(ret_token.lexeme, value);
    print_lexical_op(op_fptr, &ret_token);
    return ret_token;
}

void remove_comments(char* ipfile, char* opfile) {
    bool is_comment = false;
    bool is_new_line = true;
    FILE* fin = fopen(ipfile, "r");
    FILE* fout = fopen(opfile, "w+");
    char c = fgetc(fin);

    while (c != EOF) {
        if (c == '%') {
            is_comment = true;
            if (!is_new_line) {
                fputc('\n', fout);
            }
        }

        if (!is_comment) fputc(c, fout);

        if (c == '\n') {
            is_comment = false;
            is_new_line = true;
        } else {
            is_new_line = false;
        }

        c = fgetc(fin);
    }
}
