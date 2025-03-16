#include "basic/basic.h"
#include "basic/defs.h"

#include "basic/parser.h"
#include "lib/malloc.h"
#include "lib/stdio.h"
#include "lib/string.h"
#include "lib/stdlib.h"
#include "basic/lexer.h"

#define BASIC_MAX_STMTS 512
#define BASIC_MAX_VARS 128

basic_stmt_t code[BASIC_MAX_STMTS];

basic_var_t vars[BASIC_MAX_VARS];
int varcount = 0;

void set_var(const char *name, basic_value_t val) {
    for (int i = 0; i < varcount; ++i)
        if (strcmp(name, vars[i].name) == 0) {
            vars[i].val = val;
            return;
        }
    int idx = varcount++;
    strncpy(vars[idx].name, name, 64);
    vars[idx].val = val;
}

basic_value_t get_var(const char *name) {
    for (int i = 0; i < varcount; ++i)
        if (strcmp(name, vars[i].name) == 0)
            return vars[i].val;
    printf("Variable `%s` is undefined\n", name);
    exit(1);
}

long int extract_line_num(const char *str, char **remainingStr) {
    long int value = strtol(str, remainingStr, 10);
    if (*remainingStr == str) {
        *remainingStr = (char *)str;
        return -1;
    }
    while (**remainingStr == ' ' || **remainingStr == '\t')
        ++(*remainingStr);
    return value;
}

basic_stmt_type_t extract_stmt_type(char **str, int rln) {
    if (strncmp(*str, "PRINT ", 6) == 0) {
        *str += 6;
        return PRINT;
    } else if (strncmp(*str, "IF ", 3) == 0) {
        *str += 3;
        return IF;
    } else if (strncmp(*str, "GOTO ", 5) == 0) {
        *str += 5;
        return GOTO;
    } else if (strncmp(*str, "INPUT ", 6) == 0) {
        *str += 6;
        return INPUT;
    } else if (strncmp(*str, "LET ", 4) == 0) {
        *str += 4;
        return LET;
    } else {
        printf("Invalid Statement `%s` on line %d\n", *str, rln);
        exit(1);
    }
}

void print_ast(basic_ast_node_t *ast, int indent) {
    for (int i = 0; i < indent; ++i)
        printf("---");
    if (ast->value.is_string)
        printf("NODE %d `%s`\n", ast->type, ast->value.sval);
    else
        printf("NODE %d %f\n", ast->type, ast->value.fval);
    for (int i = 0; i < ast->childrenln; ++i)
        print_ast(ast->children[i], indent + 1);
}

basic_token_t tokens[BASIC_MAX_TOKEN_COUNT];


basic_ast_node_t *extract_param(char **params) {
    if (**params == ',')
        ++(*params);
    if (**params == 0)
        return NULL;

    while (**params == ' ' || **params == '\t')
        ++(*params);

    memset(tokens, 0, sizeof(basic_token_t) * BASIC_MAX_TOKEN_COUNT);

    int sz = basic_tokenize(params, tokens);

    if (sz == -1)
        return NULL;

     // for (int i = 0; i < sz; ++i) {
     //    if (tokens[i].value.is_string)
     //        printf("TOKEN %d %s\n", tokens[i].type, tokens[i].value.sval);
     //    else
     //        printf("TOKEN %d %f\n", tokens[i].type, tokens[i].value.fval);
    // } 

    basic_ast_node_t *node = basic_parse(tokens, sz);

    // print_ast(node, 0);

    while (**params == ' ' || **params == '\t')
        ++(*params);

    return node;
}

basic_value_t visit_node(basic_ast_node_t *node) {
    switch (node->type) {
    case BINOP:
        switch (node->value.sval[0]) {
        case '+':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval +
                                       visit_node(node->children[1]).fval};
        case '-':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval -
                                       visit_node(node->children[1]).fval};
        case '*':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval *
                                       visit_node(node->children[1]).fval};
        case '/':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval /
                                       visit_node(node->children[1]).fval};
        case '=':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval ==
                                       visit_node(node->children[1]).fval};
        case '~':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval !=
                                       visit_node(node->children[1]).fval};
        case '<':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval <
                                       visit_node(node->children[1]).fval};
        case '>':
            return (basic_value_t){false,
                                   visit_node(node->children[0]).fval >
                                       visit_node(node->children[1]).fval};
        }
        break;
    case NUM:
    case STR: return node->value;
    case VAR: return get_var(node->value.sval);
    }
    return (basic_value_t){0};
}

void rec_free(basic_ast_node_t *node) {
    for(int i = 0; i < node->childrenln; ++i)
        rec_free(node->children[i]);
    free(node);
}

void exec_loaded_basic() {
    for (int i = 0; i < BASIC_MAX_STMTS; ++i) {
        switch (code[i].type) {
        case NONE: continue;
        case PRINT:
            for (int j = 0; j < code[i].paramln; ++j) {
                basic_value_t val = visit_node(code[i].params[j]);
                if (val.is_string)
                    printf("%s", val.sval);
                else if (val.fval == (int)val.fval)
                    printf("%d", (int)val.fval);
                else
                    printf("%f", val.fval);
            }
            printf("\n");
            break;
        case IF: {
            if (code[i].paramln != 2) {
                printf("BASIC: IF takes two paramters, CONDITION, GOTO\n");
                exit(1);
            }
            if (visit_node(code[i].params[0]).fval != 0)
                i = visit_node(code[i].params[1]).fval - 1;
            break;
        }
        case GOTO: {
            if (code[i].paramln != 1) {
                printf("BASIC: GOTO takes one INT parameter\n");
                exit(1);
            }
            if (code[i].params[0]->value.is_string) {
                printf("BASIC: GOTO takes one INT parameter\n");
                exit(1);
            }
            i = (int)visit_node(code[i].params[0]).fval - 1;
            break;
        }
        case INPUT: {
            static char inbuf[512];
            scanl(inbuf, 512);
            inbuf[strlen(inbuf)] = 0;
            
            if (code[i].paramln != 2) {
                printf("BASIC: INPUT takes two parameters, VAR, TYPE\n");
                exit(1);
            }

            const char *varname = code[i].params[0]->value.sval;
            const char *typename = code[i].params[1]->value.sval;

            basic_value_t val;
            memset(&val, 0, sizeof(val));

            if (strcmp(typename, "STRING") == 0) {
                val.is_string = true;
                strncpy(val.sval, inbuf, 64);
            } else if (strcmp(typename, "INT") == 0) {
                val.fval = (float)stoi(inbuf);
            } else if (strcmp(typename, "DOUBLE") == 0) {
                val.fval = stof(inbuf);
            }

            set_var(varname, val);

            break;
        }
        case LET:
            set_var(code[i].params[0]->value.sval,
                    visit_node(code[i].params[1]));
            break;
        }
    }

    for (int i = 0; i < BASIC_MAX_STMTS; ++i) {
        if(code[i].type == NONE) continue;
        for(int j = 0; j < code[i].paramln; ++j)
            rec_free(code[i].params[j]);
    }
}

void run_basic(char *src) {
    varcount = 0;
    int stmtlen = 0;
    char *ln = strtok((char *)src, "\n");
    int rln = 1;
    while (ln) {
        while (*ln == ' ' || *ln == '\t')
            ++ln;

        char *pln;
        long int line_num = extract_line_num(ln, &pln);

        ++stmtlen;

        if (line_num == -1) {
            printf("Missing line number on line %d\n", rln);
            return;
        }

        code[line_num] = (basic_stmt_t){0};

        if (strncmp(pln, "REM ", 4) == 0) {
            ln = strtok(NULL, "\n");
            ++rln;
            continue;
        }

        basic_stmt_type_t type = extract_stmt_type(&pln, rln);
        while (*pln == ' ' || *pln == '\t')
            ++pln;

        code[line_num].type = type;

        basic_ast_node_t *param;

        while ((param = extract_param(&pln)))
            code[line_num].params[code[line_num].paramln++] = param;

        ++rln;
        ln = strtok(NULL, "\n");
    }

    exec_loaded_basic();
}
