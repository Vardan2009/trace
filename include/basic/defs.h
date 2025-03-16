#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>

#define BASIC_MAX_PARAM_COUNT 10

typedef struct {
    bool is_string;
    float fval;
    char sval[64];
} basic_value_t;

typedef enum { BINOP, NUM, STR, VAR } basic_ast_node_type_t;

typedef struct basic_ast_node {
    basic_ast_node_type_t type;
    unsigned int childrenln;

    basic_value_t value;

    struct basic_ast_node *children[2];
} basic_ast_node_t;

typedef enum { NONE, PRINT, IF, GOTO, INPUT, LET } basic_stmt_type_t;

typedef struct {
    basic_stmt_type_t type;
    unsigned int paramln;
    basic_ast_node_t *params[BASIC_MAX_PARAM_COUNT];
} basic_stmt_t;

#define BASIC_MAX_TOKEN_COUNT 1024

typedef enum {
    TOK_NUM,
    TOK_STR,
    TOK_VAR,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_GT,
    TOK_LPAREN,
    TOK_RPAREN
} basic_token_type_t;

typedef struct {
    basic_token_type_t type;
    basic_value_t value;
} basic_token_t;

typedef struct {
    char name[64];
    basic_value_t val;
} basic_var_t;

void basic_shell();

#endif  // DEFS_H
