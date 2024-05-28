#include "grammar.h"

#include <stdlib.h>

#include "../lexical_analyzer/lexer_types.h"

bool visited[NUM_NONTERMINALS] = {0};
int occur[NUM_NONTERMINALS][NUM_NONTERMINALS] = {{0}};

gram_t *create_grammar() {
    gram_t *gram = (gram_t *)malloc(sizeof(gram_t));
    gram->nonterminals = (nt_t **)calloc(NUM_NONTERMINALS, sizeof(nt_t *));

    populate_productions(gram);
    return gram;
}

void clear_grammar(gram_t *gram) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        for (int j = 0; j < gram->nonterminals[i]->num_prod; j++) {
            free(gram->nonterminals[i]->productions[j]->right);
            free(gram->nonterminals[i]->productions[j]);
        }
        free(gram->nonterminals[i]->productions);
        free(gram->nonterminals[i]);
    }
    free(gram->nonterminals);
    free(gram);
}

set_t **compute_first_sets(gram_t *gram) {
    set_t **first_sets = (set_t **)calloc(NUM_NONTERMINALS, sizeof(set_t *));
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        first_sets[i] = (set_t *)malloc(sizeof(set_t));
    }

    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        compute_first(first_sets, gram, gram->nonterminals[i], i);
    }
    return first_sets;
}

void compute_first(set_t **first_sets, gram_t *gram, nt_t *nonterm,
                   int nt_index) {
    if (visited[nt_index]) {
        return;
    }

    visited[nt_index] = true;
    int num_prod = nonterm->num_prod;
    for (int i = 0; i < num_prod; i++) {
        for (int k = 0; k < nonterm->productions[i]->num_right; k++) {
            int symb = nonterm->productions[i]->right[k];
            if (symb <= NUM_TERMINALS) {
                first_sets[nt_index]->term[symb + 1] = 1;
                break;
            } else {
                compute_first(first_sets, gram,
                              gram->nonterminals[symb - NUM_TERMINALS],
                              symb - NUM_TERMINALS);
                set_t *firstN = first_sets[symb - NUM_TERMINALS];
                for (int j = 0; j <= NUM_TERMINALS; j++) {
                    if (firstN->term[j] == 1) first_sets[nt_index]->term[j] = 1;
                }
                // not epsilon
                if (firstN->term[0] == 0) {
                    break;
                } else {
                    first_sets[nt_index]->term[0] = 0;
                }
            }
        }
    }
}

set_t **compute_follow_sets(gram_t *gram, set_t **first_sets) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        visited[i] = false;
    }
    set_t **follow_sets = (set_t **)calloc(NUM_NONTERMINALS, sizeof(set_t *));

    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        follow_sets[i] = (set_t *)malloc(sizeof(set_t));
    }

    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        compute_follow(follow_sets, first_sets, gram, gram->nonterminals[i], i);
    }

    // for(int i=0; i<NUM_NONTERMINALS; i++) {
    //     printf("Follow set for %d: ", i);
    //     for(int j=0; j<=NUM_TERMINALS+1; j++) {
    //         if(follow_sets[i]->term[j] == 1) {
    //             if(j == 0) {
    //                 printf("EPSILON ");
    //             }
    //             else if(j == NUM_TERMINALS+1) {
    //                 printf("$ ");
    //             }
    //             else {
    //                 printf("%s ", token_str[j-1]);
    //             }
    //         }
    //     }
    //     printf("\n");
    // }
    return follow_sets;
}

void compute_follow(set_t **follow_sets, set_t **first_sets, gram_t *gram,
                    nt_t *nonterm, int nt_index) {
    if (visited[nt_index]) {
        return;
    }

    visited[nt_index] = true;
    int num_prod = nonterm->num_prod;
    if (nt_index == 0) {
        // bottom marker
        follow_sets[nt_index]->term[NUM_TERMINALS + 1] = 1;
    }

    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        if (occur[nt_index][i] == 1) {
            int num_prod = gram->nonterminals[i]->num_prod;
            for (int j = 0; j < num_prod; j++) {
                int num_right =
                    gram->nonterminals[i]->productions[j]->num_right;
                bool flag = false;
                for (int k = 0; k < num_right; k++) {
                    if (gram->nonterminals[i]->productions[j]->right[k] ==
                        nt_index + NUM_TERMINALS) {
                        flag = true;
                        continue;
                    }
                    if (flag) {
                        if (gram->nonterminals[i]->productions[j]->right[k] <
                            NUM_TERMINALS) {
                            follow_sets[nt_index]->term[gram->nonterminals[i]
                                                            ->productions[j]
                                                            ->right[k] +
                                                        1] = 1;
                            break;
                        } else {
                            set_t *firstN = first_sets[gram->nonterminals[i]
                                                           ->productions[j]
                                                           ->right[k] -
                                                       NUM_TERMINALS];
                            for (int l = 0; l <= NUM_TERMINALS + 1; l++) {
                                if (firstN->term[l] == 1) {
                                    follow_sets[nt_index]->term[l] = 1;
                                }
                            }
                            // epsilon in first(nonterminal)
                            if (firstN->term[0] == 1) {
                                if (k != num_right - 1)
                                    follow_sets[nt_index]->term[0] = 0;
                            }
                            // not epsilon
                            else {
                                flag = false;
                                break;
                            }
                        }
                    }
                }

                if (gram->nonterminals[i]
                        ->productions[j]
                        ->right[num_right - 1] == nt_index + NUM_TERMINALS) {
                    follow_sets[nt_index]->term[0] = 0;
                    compute_follow(follow_sets, first_sets, gram,
                                   gram->nonterminals[i], i);
                    for (int l = 0; l <= NUM_TERMINALS + 1; l++) {
                        if (follow_sets[i]->term[l] == 1) {
                            follow_sets[nt_index]->term[l] = 1;
                        }
                    }
                }
            }

            if (follow_sets[nt_index]->term[0] == 1) {
                follow_sets[nt_index]->term[0] = 0;
                compute_follow(follow_sets, first_sets, gram,
                               gram->nonterminals[i], i);
                for (int l = 0; l <= NUM_TERMINALS + 1; l++) {
                    if (follow_sets[i]->term[l] == 1) {
                        follow_sets[nt_index]->term[l] = 1;
                    }
                }
            }
        }
    }
}

void clear_sets(set_t **sets) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        free(sets[i]);
    }
    free(sets);
}

nt_t *add_nonterminal(gram_t *gram, int nt) {
    gram->nonterminals[nt - NUM_TERMINALS] = (nt_t *)malloc(sizeof(nt_t));
    gram->nonterminals[nt - NUM_TERMINALS]->num_prod = 0;
    return gram->nonterminals[nt - NUM_TERMINALS];
}

prod_t *create_production() {
    prod_t *prod = (prod_t *)malloc(sizeof(prod_t));
    prod->num_right = 0;
    return prod;
}

void add_production(nt_t *nonterm, prod_t *prod) {
    nonterm->num_prod++;
    nonterm->productions = (prod_t **)realloc(
        nonterm->productions, nonterm->num_prod * sizeof(prod_t *));
    nonterm->productions[nonterm->num_prod - 1] = prod;
}

void add_right(prod_t *prod, int right, int nt) {
    prod->num_right++;
    prod->right = (int *)realloc(prod->right, prod->num_right * sizeof(int));
    prod->right[prod->num_right - 1] = right;
    if (right >= NUM_TERMINALS) {
        occur[right - NUM_TERMINALS][nt - NUM_TERMINALS] = 1;
    }
}

void populate_productions(gram_t *gram) {
    nt_t *nonterm = add_nonterminal(gram, PROGRAM);
    prod_t *prod = create_production();
    add_right(prod, OTHERFUNCTIONS, PROGRAM);
    add_right(prod, MAINFUNCTION, PROGRAM);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, MAINFUNCTION);
    prod = create_production();
    add_right(prod, TK_MAIN, MAINFUNCTION);
    add_right(prod, STMTS, MAINFUNCTION);
    add_right(prod, TK_END, MAINFUNCTION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OTHERFUNCTIONS);
    prod = create_production();
    add_right(prod, FUNCTION, OTHERFUNCTIONS);
    add_right(prod, OTHERFUNCTIONS, OTHERFUNCTIONS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OTHERFUNCTIONS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FUNCTION);
    prod = create_production();
    add_right(prod, TK_FUNID, FUNCTION);
    add_right(prod, INPUT_PAR, FUNCTION);
    add_right(prod, OUTPUT_PAR, FUNCTION);
    add_right(prod, TK_SEM, FUNCTION);
    add_right(prod, STMTS, FUNCTION);
    add_right(prod, TK_END, FUNCTION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, INPUT_PAR);
    prod = create_production();
    add_right(prod, TK_INPUT, INPUT_PAR);
    add_right(prod, TK_PARAMETER, INPUT_PAR);
    add_right(prod, TK_LIST, INPUT_PAR);
    add_right(prod, TK_SQL, INPUT_PAR);
    add_right(prod, PARAMETER_LIST, INPUT_PAR);
    add_right(prod, TK_SQR, INPUT_PAR);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OUTPUT_PAR);
    prod = create_production();
    add_right(prod, TK_OUTPUT, OUTPUT_PAR);
    add_right(prod, TK_PARAMETER, OUTPUT_PAR);
    add_right(prod, TK_LIST, OUTPUT_PAR);
    add_right(prod, TK_SQL, OUTPUT_PAR);
    add_right(prod, PARAMETER_LIST, OUTPUT_PAR);
    add_right(prod, TK_SQR, OUTPUT_PAR);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OUTPUT_PAR);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, PARAMETER_LIST);
    prod = create_production();
    add_right(prod, DATATYPE, PARAMETER_LIST);
    add_right(prod, TK_ID, PARAMETER_LIST);
    add_right(prod, REMAINING_LIST, PARAMETER_LIST);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, DATATYPE);
    prod = create_production();
    add_right(prod, PRIMITIVEDATATYPE, DATATYPE);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, CONSTRUCTEDDATATYPE, DATATYPE);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, PRIMITIVEDATATYPE);
    prod = create_production();
    add_right(prod, TK_INT, PRIMITIVEDATATYPE);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_REAL, PRIMITIVEDATATYPE);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, CONSTRUCTEDDATATYPE);
    prod = create_production();
    add_right(prod, TK_RECORD, CONSTRUCTEDDATATYPE);
    add_right(prod, TK_RUID, CONSTRUCTEDDATATYPE);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_UNION, CONSTRUCTEDDATATYPE);
    add_right(prod, TK_RUID, CONSTRUCTEDDATATYPE);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_RUID, CONSTRUCTEDDATATYPE);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, REMAINING_LIST);
    prod = create_production();
    add_right(prod, TK_COMMA, REMAINING_LIST);
    add_right(prod, PARAMETER_LIST, REMAINING_LIST);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, REMAINING_LIST);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, STMTS);
    prod = create_production();
    add_right(prod, TYPEDEFINITIONS, STMTS);
    add_right(prod, DECLARATIONS, STMTS);
    add_right(prod, OTHERSTMTS, STMTS);
    add_right(prod, RETURNSTMT, STMTS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, TYPEDEFINITIONS);
    prod = create_production();
    add_right(prod, ACTUALORREDEFINED, TYPEDEFINITIONS);
    add_right(prod, TYPEDEFINITIONS, TYPEDEFINITIONS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, TYPEDEFINITIONS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, ACTUALORREDEFINED);
    prod = create_production();
    add_right(prod, TYPEDEFINITION, ACTUALORREDEFINED);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, DEFINETYPESTMT, ACTUALORREDEFINED);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, TYPEDEFINITION);
    prod = create_production();
    add_right(prod, TK_RECORD, TYPEDEFINITION);
    add_right(prod, TK_RUID, TYPEDEFINITION);
    add_right(prod, FIELDDEFINITIONS, TYPEDEFINITION);
    add_right(prod, TK_ENDRECORD, TYPEDEFINITION);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_UNION, TYPEDEFINITION);
    add_right(prod, TK_RUID, TYPEDEFINITION);
    add_right(prod, FIELDDEFINITIONS, TYPEDEFINITION);
    add_right(prod, TK_ENDUNION, TYPEDEFINITION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FIELDDEFINITIONS);
    prod = create_production();
    add_right(prod, FIELDDEFINITION, FIELDDEFINITIONS);
    add_right(prod, FIELDDEFINITION, FIELDDEFINITIONS);
    add_right(prod, MOREFIELDS, FIELDDEFINITIONS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FIELDDEFINITION);
    prod = create_production();
    add_right(prod, TK_TYPE, FIELDDEFINITION);
    add_right(prod, FIELDTYPE, FIELDDEFINITION);
    add_right(prod, TK_COLON, FIELDDEFINITION);
    add_right(prod, TK_FIELDID, FIELDDEFINITION);
    add_right(prod, TK_SEM, FIELDDEFINITION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FIELDTYPE);
    prod = create_production();
    add_right(prod, PRIMITIVEDATATYPE, FIELDTYPE);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, CONSTRUCTEDDATATYPE, FIELDTYPE);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, MOREFIELDS);
    prod = create_production();
    add_right(prod, FIELDDEFINITION, MOREFIELDS);
    add_right(prod, MOREFIELDS, MOREFIELDS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, MOREFIELDS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, DECLARATIONS);
    prod = create_production();
    add_right(prod, DECLARATION, DECLARATIONS);
    add_right(prod, DECLARATIONS, DECLARATIONS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, DECLARATIONS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, DECLARATION);
    prod = create_production();
    add_right(prod, TK_TYPE, DECLARATION);
    add_right(prod, DATATYPE, DECLARATION);
    add_right(prod, TK_COLON, DECLARATION);
    add_right(prod, TK_ID, DECLARATION);
    add_right(prod, GLOBAL_OR_NOT, DECLARATION);
    add_right(prod, TK_SEM, DECLARATION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, GLOBAL_OR_NOT);
    prod = create_production();
    add_right(prod, TK_COLON, GLOBAL_OR_NOT);
    add_right(prod, TK_GLOBAL, GLOBAL_OR_NOT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, GLOBAL_OR_NOT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OTHERSTMTS);
    prod = create_production();
    add_right(prod, STMT, OTHERSTMTS);
    add_right(prod, OTHERSTMTS, OTHERSTMTS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OTHERSTMTS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, STMT);
    prod = create_production();
    add_right(prod, ASSIGNMENTSTMT, STMT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, ITERATIVESTMT, STMT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, CONDITIONALSTMT, STMT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, IOSTMT, STMT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, FUNCALLSTMT, STMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, ASSIGNMENTSTMT);
    prod = create_production();
    add_right(prod, SINGLEORRECID, ASSIGNMENTSTMT);
    add_right(prod, TK_ASSIGNOP, ASSIGNMENTSTMT);
    add_right(prod, ARITHMETICEXPRESSION, ASSIGNMENTSTMT);
    add_right(prod, TK_SEM, ASSIGNMENTSTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, SINGLEORRECID);
    prod = create_production();
    add_right(prod, TK_ID, SINGLEORRECID);
    add_right(prod, OPTION_SINGLE_CONSTRUCTED, SINGLEORRECID);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, CONSTRUCTEDVARIABLE);

    nonterm = add_nonterminal(gram, ONEEXPANSION);
    prod = create_production();
    add_right(prod, TK_DOT, ONEEXPANSION);
    add_right(prod, TK_FIELDID, ONEEXPANSION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, MOREEXPANSIONS);
    prod = create_production();
    add_right(prod, ONEEXPANSION, MOREEXPANSIONS);
    add_right(prod, MOREEXPANSIONS, MOREEXPANSIONS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, MOREEXPANSIONS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OPTION_SINGLE_CONSTRUCTED);
    prod = create_production();
    add_right(prod, ONEEXPANSION, OPTION_SINGLE_CONSTRUCTED);
    add_right(prod, MOREEXPANSIONS, OPTION_SINGLE_CONSTRUCTED);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OPTION_SINGLE_CONSTRUCTED);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FUNCALLSTMT);
    prod = create_production();
    add_right(prod, OUTPUTPARAMETERS, FUNCALLSTMT);
    add_right(prod, TK_CALL, FUNCALLSTMT);
    add_right(prod, TK_FUNID, FUNCALLSTMT);
    add_right(prod, TK_WITH, FUNCALLSTMT);
    add_right(prod, TK_PARAMETERS, FUNCALLSTMT);
    add_right(prod, INPUTPARAMETERS, FUNCALLSTMT);
    add_right(prod, TK_SEM, FUNCALLSTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OUTPUTPARAMETERS);
    prod = create_production();
    add_right(prod, TK_SQL, OUTPUTPARAMETERS);
    add_right(prod, IDLIST, OUTPUTPARAMETERS);
    add_right(prod, TK_SQR, OUTPUTPARAMETERS);
    add_right(prod, TK_ASSIGNOP, OUTPUTPARAMETERS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OUTPUTPARAMETERS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, INPUTPARAMETERS);
    prod = create_production();
    add_right(prod, TK_SQL, INPUTPARAMETERS);
    add_right(prod, IDLIST, INPUTPARAMETERS);
    add_right(prod, TK_SQR, INPUTPARAMETERS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, ITERATIVESTMT);
    prod = create_production();
    add_right(prod, TK_WHILE, ITERATIVESTMT);
    add_right(prod, TK_OP, ITERATIVESTMT);
    add_right(prod, BOOLEANEXPRESSION, ITERATIVESTMT);
    add_right(prod, TK_CL, ITERATIVESTMT);
    add_right(prod, STMT, ITERATIVESTMT);
    add_right(prod, OTHERSTMTS, ITERATIVESTMT);
    add_right(prod, TK_ENDWHILE, ITERATIVESTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, CONDITIONALSTMT);
    prod = create_production();
    add_right(prod, TK_IF, CONDITIONALSTMT);
    add_right(prod, TK_OP, CONDITIONALSTMT);
    add_right(prod, BOOLEANEXPRESSION, CONDITIONALSTMT);
    add_right(prod, TK_CL, CONDITIONALSTMT);
    add_right(prod, TK_THEN, CONDITIONALSTMT);
    add_right(prod, STMT, CONDITIONALSTMT);
    add_right(prod, OTHERSTMTS, CONDITIONALSTMT);
    add_right(prod, ELSEPART, CONDITIONALSTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, ELSEPART);
    prod = create_production();
    add_right(prod, TK_ELSE, ELSEPART);
    add_right(prod, STMT, ELSEPART);
    add_right(prod, OTHERSTMTS, ELSEPART);
    add_right(prod, TK_ENDIF, ELSEPART);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_ENDIF, ELSEPART);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, IOSTMT);
    prod = create_production();
    add_right(prod, TK_READ, IOSTMT);
    add_right(prod, TK_OP, IOSTMT);
    add_right(prod, VAR, IOSTMT);
    add_right(prod, TK_CL, IOSTMT);
    add_right(prod, TK_SEM, IOSTMT);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_WRITE, IOSTMT);
    add_right(prod, TK_OP, IOSTMT);
    add_right(prod, VAR, IOSTMT);
    add_right(prod, TK_CL, IOSTMT);
    add_right(prod, TK_SEM, IOSTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, ARITHMETICEXPRESSION);
    prod = create_production();
    add_right(prod, TERM, ARITHMETICEXPRESSION);
    add_right(prod, EXPPRIME, ARITHMETICEXPRESSION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, EXPPRIME);
    prod = create_production();
    add_right(prod, LOWPRECEDENCEOPERATORS, EXPPRIME);
    add_right(prod, TERM, EXPPRIME);
    add_right(prod, EXPPRIME, EXPPRIME);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, EXPPRIME);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, TERM);
    prod = create_production();
    add_right(prod, FACTOR, TERM);
    add_right(prod, TERMPRIME, TERM);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, TERMPRIME);
    prod = create_production();
    add_right(prod, HIGHPRECEDENCEOPERATORS, TERMPRIME);
    add_right(prod, FACTOR, TERMPRIME);
    add_right(prod, TERMPRIME, TERMPRIME);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, TERMPRIME);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, FACTOR);
    prod = create_production();
    add_right(prod, TK_OP, FACTOR);
    add_right(prod, ARITHMETICEXPRESSION, FACTOR);
    add_right(prod, TK_CL, FACTOR);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, VAR, FACTOR);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, HIGHPRECEDENCEOPERATORS);
    prod = create_production();
    add_right(prod, TK_MUL, HIGHPRECEDENCEOPERATORS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_DIV, HIGHPRECEDENCEOPERATORS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, LOWPRECEDENCEOPERATORS);
    prod = create_production();
    add_right(prod, TK_PLUS, LOWPRECEDENCEOPERATORS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_MINUS, LOWPRECEDENCEOPERATORS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, BOOLEANEXPRESSION);
    prod = create_production();
    add_right(prod, TK_OP, BOOLEANEXPRESSION);
    add_right(prod, BOOLEANEXPRESSION, BOOLEANEXPRESSION);
    add_right(prod, TK_CL, BOOLEANEXPRESSION);
    add_right(prod, LOGICALOP, BOOLEANEXPRESSION);
    add_right(prod, TK_OP, BOOLEANEXPRESSION);
    add_right(prod, BOOLEANEXPRESSION, BOOLEANEXPRESSION);
    add_right(prod, TK_CL, BOOLEANEXPRESSION);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, VAR, BOOLEANEXPRESSION);
    add_right(prod, RELATIONALOP, BOOLEANEXPRESSION);
    add_right(prod, VAR, BOOLEANEXPRESSION);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_NOT, BOOLEANEXPRESSION);
    add_right(prod, TK_OP, BOOLEANEXPRESSION);
    add_right(prod, BOOLEANEXPRESSION, BOOLEANEXPRESSION);
    add_right(prod, TK_CL, BOOLEANEXPRESSION);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, VAR);
    prod = create_production();
    add_right(prod, SINGLEORRECID, VAR);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_NUM, VAR);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_RNUM, VAR);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, LOGICALOP);
    prod = create_production();
    add_right(prod, TK_AND, LOGICALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_OR, LOGICALOP);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, RELATIONALOP);
    prod = create_production();
    add_right(prod, TK_LT, RELATIONALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_LE, RELATIONALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_EQ, RELATIONALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_GT, RELATIONALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_GE, RELATIONALOP);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_NE, RELATIONALOP);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, RETURNSTMT);
    prod = create_production();
    add_right(prod, TK_RETURN, RETURNSTMT);
    add_right(prod, OPTIONALRETURN, RETURNSTMT);
    add_right(prod, TK_SEM, RETURNSTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, OPTIONALRETURN);
    prod = create_production();
    add_right(prod, TK_SQL, OPTIONALRETURN);
    add_right(prod, IDLIST, OPTIONALRETURN);
    add_right(prod, TK_SQR, OPTIONALRETURN);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, OPTIONALRETURN);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, IDLIST);
    prod = create_production();
    add_right(prod, TK_ID, IDLIST);
    add_right(prod, MORE_IDS, IDLIST);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, MORE_IDS);
    prod = create_production();
    add_right(prod, TK_COMMA, MORE_IDS);
    add_right(prod, IDLIST, MORE_IDS);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, EPSILON, MORE_IDS);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, DEFINETYPESTMT);
    prod = create_production();
    add_right(prod, TK_DEFINETYPE, DEFINETYPESTMT);
    add_right(prod, A, DEFINETYPESTMT);
    add_right(prod, TK_RUID, DEFINETYPESTMT);
    add_right(prod, TK_AS, DEFINETYPESTMT);
    add_right(prod, TK_RUID, DEFINETYPESTMT);
    add_production(nonterm, prod);

    nonterm = add_nonterminal(gram, A);
    prod = create_production();
    add_right(prod, TK_RECORD, A);
    add_production(nonterm, prod);
    prod = create_production();
    add_right(prod, TK_UNION, A);
    add_production(nonterm, prod);
}
