#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#define EPSILON -1
#define TK_ASSIGNOP 0
#define TK_COMMENT 1
#define TK_FIELDID 2
#define TK_ID 3
#define TK_NUM 4
#define TK_RNUM 5
#define TK_FUNID 6
#define TK_RUID 7
#define TK_WITH 8
#define TK_PARAMETERS 9
#define TK_END 10
#define TK_WHILE 11
#define TK_UNION 12
#define TK_ENDUNION 13
#define TK_DEFINETYPE 14
#define TK_AS 15
#define TK_TYPE 16
#define TK_MAIN 17
#define TK_GLOBAL 18
#define TK_PARAMETER 19
#define TK_LIST 20
#define TK_SQL 21
#define TK_SQR 22
#define TK_INPUT 23
#define TK_OUTPUT 24
#define TK_INT 25
#define TK_REAL 26
#define TK_COMMA 27
#define TK_SEM 28
#define TK_COLON 29
#define TK_DOT 30
#define TK_ENDWHILE 31
#define TK_OP 32
#define TK_CL 33
#define TK_IF 34
#define TK_THEN 35
#define TK_ENDIF 36
#define TK_READ 37
#define TK_WRITE 38
#define TK_RETURN 39
#define TK_PLUS 40
#define TK_MINUS 41
#define TK_MUL 42
#define TK_DIV 43
#define TK_CALL 44
#define TK_RECORD 45
#define TK_ENDRECORD 46
#define TK_ELSE 47
#define TK_AND 48
#define TK_OR 49
#define TK_NOT 50
#define TK_LT 51
#define TK_LE 52
#define TK_EQ 53
#define TK_GT 54
#define TK_GE 55
#define TK_NE 56

static char *token_str[] = {
    "TK_ASSIGNOP", "TK_COMMENT",   "TK_FIELDID", "TK_ID",       "TK_NUM",
    "TK_RNUM",     "TK_FUNID",     "TK_RUID",    "TK_WITH",     "TK_PARAMETERS",
    "TK_END",      "TK_WHILE",     "TK_UNION",   "TK_ENDUNION", "TK_DEFINETYPE",
    "TK_AS",       "TK_TYPE",      "TK_MAIN",    "TK_GLOBAL",   "TK_PARAMETER",
    "TK_LIST",     "TK_SQL",       "TK_SQR",     "TK_INPUT",    "TK_OUTPUT",
    "TK_INT",      "TK_REAL",      "TK_COMMA",   "TK_SEM",      "TK_COLON",
    "TK_DOT",      "TK_ENDWHILE",  "TK_OP",      "TK_CL",       "TK_IF",
    "TK_THEN",     "TK_ENDIF",     "TK_READ",    "TK_WRITE",    "TK_RETURN",
    "TK_PLUS",     "TK_MINUS",     "TK_MUL",     "TK_DIV",      "TK_CALL",
    "TK_RECORD",   "TK_ENDRECORD", "TK_ELSE",    "TK_AND",      "TK_OR",
    "TK_NOT",      "TK_LT",        "TK_LE",      "TK_EQ",       "TK_GT",
    "TK_GE",       "TK_NE",
};

typedef struct SymbolTableEntry {
    char *lexeme;
    int token_type;
} stentry_t;

typedef struct TokenInfo {
    int token_type;
    union {
        stentry_t *stentry;
        double rnum_val;
        unsigned long num_val;
    } info;
    int line_no;
    char *lexeme;
} tokeninfo_t;

#endif  // !COMPILER_TYPES_H
