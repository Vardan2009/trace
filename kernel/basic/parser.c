#include "basic/defs.h"
#include "lib/malloc.h"
#include "lib/stdio.h"
#include "lib/string.h"

basic_ast_node_t *basic_expr(basic_token_t *tokens, int tokens_sz,
                             int *tokenptr, int *exitcode);

basic_ast_node_t *basic_factor(basic_token_t *tokens, int tokens_sz,
                               int *tokenptr, int *exitcode) {
    switch (tokens[*tokenptr].type) {
        case TOK_NUM: {
            basic_token_t t = tokens[(*tokenptr)++];
            basic_ast_node_t *res = malloc(sizeof(basic_ast_node_t));
            res->type = NUM;
            res->childrenln = 0;
            res->value = t.value;
            return res;
        }
        case TOK_STR: {
            basic_token_t t = tokens[(*tokenptr)++];
            basic_ast_node_t *res = malloc(sizeof(basic_ast_node_t));
            res->type = STR;
            res->childrenln = 0;
            res->value = t.value;
            return res;
        }
        case TOK_VAR: {
            basic_token_t *t = &tokens[(*tokenptr)++];
            basic_ast_node_t *res = malloc(sizeof(basic_ast_node_t));
            res->type = VAR;
            res->childrenln = 0;
            res->value.is_string = true;
            strncpy(res->value.sval, t->value.sval, 64);
            return res;
        }
        case TOK_LPAREN: {
            ++(*tokenptr);
            basic_ast_node_t *res =
                basic_expr(tokens, tokens_sz, tokenptr, exitcode);
            ++(*tokenptr);
            return res;
        }
        default: {
            printf("Invalid factor of type %d\n", tokens[*tokenptr].type);
            *exitcode = 1;
        }
    }
    return NULL;
}

basic_ast_node_t *basic_term(basic_token_t *tokens, int tokens_sz,
                             int *tokenptr, int *exitcode) {
    basic_ast_node_t *n = basic_factor(tokens, tokens_sz, tokenptr, exitcode);
    while (true) {
        if (tokens[*tokenptr].type == TOK_MUL ||
            tokens[*tokenptr].type == TOK_DIV ||
            tokens[*tokenptr].type == TOK_EQ ||
            tokens[*tokenptr].type == TOK_NEQ ||
            tokens[*tokenptr].type == TOK_LT ||
            tokens[*tokenptr].type == TOK_GT) {
            basic_token_type_t op = tokens[(*tokenptr)++].type;
            basic_ast_node_t *r =
                basic_factor(tokens, tokens_sz, tokenptr, exitcode);
            basic_ast_node_t *l = n;
            n = malloc(sizeof(basic_ast_node_t));
            n->type = BINOP;
            n->value.is_string = true;
            if (op == TOK_MUL)
                n->value.sval[0] = '*';
            else if (op == TOK_DIV)
                n->value.sval[0] = '/';
            else if (op == TOK_EQ)
                n->value.sval[0] = '=';
            else if (op == TOK_NEQ)
                n->value.sval[0] = '~';
            else if (op == TOK_LT)
                n->value.sval[0] = '<';
            else if (op == TOK_GT)
                n->value.sval[0] = '>';
            n->value.sval[1] = 0;
            n->childrenln = 2;
            n->children[0] = l;
            n->children[1] = r;
        } else
            break;
    }
    return n;
}

basic_ast_node_t *basic_expr(basic_token_t *tokens, int tokens_sz,
                             int *tokenptr, int *exitcode) {
    basic_ast_node_t *n = basic_term(tokens, tokens_sz, tokenptr, exitcode);
    while (true) {
        if (tokens[*tokenptr].type == TOK_PLUS ||
            tokens[*tokenptr].type == TOK_MINUS) {
            basic_token_type_t op = tokens[(*tokenptr)++].type;
            basic_ast_node_t *r =
                basic_term(tokens, tokens_sz, tokenptr, exitcode);
            basic_ast_node_t *l = n;
            n = malloc(sizeof(basic_ast_node_t));
            n->type = BINOP;
            n->value.is_string = true;
            if (op == TOK_PLUS)
                n->value.sval[0] = '+';
            else
                n->value.sval[0] = '-';
            n->value.sval[1] = 0;
            n->childrenln = 2;
            n->children[0] = l;
            n->children[1] = r;
        } else
            break;
    }

    return n;
}

basic_ast_node_t *basic_parse(basic_token_t *tokens, int tokens_sz,
                              int *exitcode) {
    int tokenptr = 0;
    return basic_expr(tokens, tokens_sz, &tokenptr, exitcode);
}
