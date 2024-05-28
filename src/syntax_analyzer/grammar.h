#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "../lexical_analyzer/lexer.h"

#define PROGRAM 57
#define MAINFUNCTION 58
#define OTHERFUNCTIONS 59
#define FUNCTION 60
#define INPUT_PAR 61
#define OUTPUT_PAR 62
#define PARAMETER_LIST 63
#define DATATYPE 64
#define PRIMITIVEDATATYPE 65
#define CONSTRUCTEDDATATYPE 66
#define REMAINING_LIST 67
#define STMTS 68
#define TYPEDEFINITIONS 69
#define ACTUALORREDEFINED 70
#define TYPEDEFINITION 71
#define FIELDDEFINITIONS 72
#define FIELDDEFINITION 73
#define FIELDTYPE 74
#define MOREFIELDS 75
#define DECLARATIONS 76
#define DECLARATION 77
#define GLOBAL_OR_NOT 78
#define OTHERSTMTS 79
#define STMT 80
#define ASSIGNMENTSTMT 81
#define SINGLEORRECID 82
#define CONSTRUCTEDVARIABLE 83
#define ONEEXPANSION 84
#define MOREEXPANSIONS 85
#define FUNCALLSTMT 86
#define OUTPUTPARAMETERS 87
#define INPUTPARAMETERS 88
#define ITERATIVESTMT 89
#define CONDITIONALSTMT 90
#define ELSEPART 91
#define IOSTMT 92
#define ARITHMETICEXPRESSION 93
#define EXPPRIME 94
#define TERM 95
#define TERMPRIME 96
#define FACTOR 97
#define HIGHPRECEDENCEOPERATORS 98
#define LOWPRECEDENCEOPERATORS 99
#define BOOLEANEXPRESSION 100
#define VAR 101
#define LOGICALOP 102
#define RELATIONALOP 103
#define RETURNSTMT 104
#define OPTIONALRETURN 105
#define IDLIST 106
#define MORE_IDS 107
#define DEFINETYPESTMT 108
#define A 109
#define OPTION_SINGLE_CONSTRUCTED 110
#define BOTTOMMARKER 111

#define NUM_TERMINALS 57
#define NUM_NONTERMINALS 54

static char *non_terminals[] = {"<program>",
                                "<mainFunction>",
                                "<otherFunctions>",
                                "<function>",
                                "<input_par>",
                                "<output_par>",
                                "<parameter_list>",
                                "<dataType>",
                                "<primitiveDatatype>",
                                "<constructedDatatype>",
                                "<remaining_list>",
                                "<stmts>",
                                "<typeDefinitions>",
                                "<actualOrRedefined>",
                                "<typeDefinition>",
                                "<fieldDefinitions>",
                                "<fieldDefinition>",
                                "<fieldType>",
                                "<moreFields>",
                                "<declarations>",
                                "<declaration>",
                                "<global_or_not>",
                                "<otherStmts>",
                                "<stmt>",
                                "<assignmentStmt>",
                                "<singleOrRecId>",
                                "<constructedVariable>",
                                "<oneExpansion>",
                                "<moreExpansions>",
                                "<funCallStmt>",
                                "<outputParameters>",
                                "<inputParameters>",
                                "<iterativeStmt>",
                                "<conditionalStmt>",
                                "<elsePart>",
                                "<ioStmt>",
                                "<arithmeticExpression>",
                                "<expPrime>",
                                "<term>",
                                "<termPrime>",
                                "<factor>",
                                "<highPrecedenceOperators>",
                                "<lowPrecedenceOperators>",
                                "<booleanExpression>",
                                "<var>",
                                "<logicalOp>",
                                "<relationalOp>",
                                "<returnStmt>",
                                "<optionalReturn>",
                                "<idList>",
                                "<more_ids>",
                                "<definetypestmt>",
                                "<a>",
                                "<option_single_constructed>"};

typedef struct Production {
    int num_right;
    int *right;
} prod_t;

typedef struct Nonterminal {
    int num_prod;
    prod_t **productions;
} nt_t;

typedef struct Grammar {
    nt_t **nonterminals;
} gram_t;

typedef struct Set {
    int term[59];
} set_t;

gram_t *create_grammar();
nt_t *add_nonterminal(gram_t *gram, int nt);
prod_t *create_production();
void add_production(nt_t *nonterm, prod_t *prod);
void add_right(prod_t *prod, int right, int nt);
void populate_productions(gram_t *gram);
void clear_grammar(gram_t *gram);
void compute_first(set_t **first_sets, gram_t *gram, nt_t *nonterm,
                   int nt_index);
set_t **compute_first_sets(gram_t *gram);
void compute_follow(set_t **follow_sets, set_t **first_sets, gram_t *gram,
                    nt_t *nonterm, int nt_index);
set_t **compute_follow_sets(gram_t *gram, set_t **first_sets);
void clear_sets(set_t **set);

#endif
