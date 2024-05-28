#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "../hash_table/hash_table.h"
#include "lexer_types.h"

#define BUF_SIZE 512
#define TBUF_SIZE (BUF_SIZE * 2)

tokeninfo_t get_next_token(char* ip_filename, ht_t* symbol_table,
                           char* op_filename);
void remove_comments(char* fin, char* fout);

#endif  // !LEXER_H
