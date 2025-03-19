#include "basic/basic.h"

#include "basic/defs.h"
#include "basic/lexer.h"
#include "basic/parser.h"
#include "lib/fs.h"
#include "lib/malloc.h"
#include "lib/path.h"
#include "lib/rand.h"
#include "lib/stdio.h"
#include "lib/stdlib.h"
#include "lib/string.h"
#include "timer.h"

#define BASIC_MAX_STMTS 512
#define BASIC_MAX_VARS 128

basic_stmt_t code[BASIC_MAX_STMTS];
char code_str[BASIC_MAX_STMTS][128];

basic_var_t vars[BASIC_MAX_VARS];
int varcount = 0;

void set_var(const char *name, basic_value_t val) {
    if (strcmp(name, "RND") == 0) {
        printf("BASIC: RND is not modifiable\n");
        return;
    }

    if (strcmp(name, "TIMER") == 0) {
        printf("BASIC: TIMER is not modifiable\n");
        return;
    }

    for (int i = 0; i < varcount; ++i)
        if (strcmp(name, vars[i].name) == 0) {
            vars[i].val = val;
            return;
        }
    int idx = varcount++;
    strncpy(vars[idx].name, name, 64);
    vars[idx].val = val;
}

int get_var(const char *name, basic_value_t *out) {
    if (strcmp(name, "RND") == 0) {
        *out = (basic_value_t){false, rand()};
        return 0;
    }

    if (strcmp(name, "TIMER") == 0) {
        *out = (basic_value_t){false, ticks};
        return 0;
    }

    for (int i = 0; i < varcount; ++i)
        if (strcmp(name, vars[i].name) == 0) {
            *out = vars[i].val;
            return 0;
        }
    printf("Variable `%s` undefined\n", name);
    return 1;
}

long int extract_line_num(const char *str, char **remainingStr) {
    long int value = strtol(str, remainingStr, 10);
    if (*remainingStr == str) {
        *remainingStr = (char *)str;
        return -1;
    }
    while (**remainingStr == ' ' || **remainingStr == '\t') ++(*remainingStr);
    return value;
}

basic_stmt_type_t extract_stmt_type(char **str, int rln, int *exitcode) {
    *exitcode = 0;
    if (strncmp(*str, "PRINT", 5) == 0) {
        *str += 5;
        return PRINT;
    } else if (strncmp(*str, "IF", 2) == 0) {
        *str += 2;
        return IF;
    } else if (strncmp(*str, "GOTO", 4) == 0) {
        *str += 4;
        return GOTO;
    } else if (strncmp(*str, "INPUT", 5) == 0) {
        *str += 5;
        return INPUT;
    } else if (strncmp(*str, "LET", 3) == 0) {
        *str += 3;
        return LET;
    } else if (strncmp(*str, "DIM", 3) == 0) {
        *str += 3;
        return DIM;
    } else if (strncmp(*str, "RANDOMIZE", 9) == 0) {
        *str += 9;
        return SRAND;
    } else if (strncmp(*str, "CLEAR", 5) == 0) {
        *str += 5;
        return CLEAR;
    } else if (strncmp(*str, "END", 3) == 0) {
        *str += 3;
        return END;
    } else {
        if (rln != -1)
            printf("Invalid Statement `%s` on line %d\n", *str, rln);
        else
            printf("Invalid Statement `%s`\n", *str);
        *exitcode = 1;
        return 0;
    }
}

void print_ast(basic_ast_node_t *ast, int indent) {
    for (int i = 0; i < indent; ++i) printf("---");
    if (ast->value.is_string)
        printf("NODE %d `%s`\n", ast->type, ast->value.sval);
    else
        printf("NODE %d %f\n", ast->type, ast->value.fval);
    for (int i = 0; i < ast->childrenln; ++i)
        print_ast(ast->children[i], indent + 1);
}

basic_token_t tokens[BASIC_MAX_TOKEN_COUNT];

basic_ast_node_t *extract_param(char **params, int *exitcode) {
    if (**params == ',') ++(*params);
    if (**params == 0) return NULL;

    while (**params == ' ' || **params == '\t') ++(*params);

    memset(tokens, 0, sizeof(basic_token_t) * BASIC_MAX_TOKEN_COUNT);

    int sz = basic_tokenize(params, tokens);

    if (sz == -1) return NULL;

    // for (int i = 0; i < sz; ++i) {
    //    if (tokens[i].value.is_string)
    //        printf("TOKEN %d %s\n", tokens[i].type, tokens[i].value.sval);
    //    else
    //        printf("TOKEN %d %f\n", tokens[i].type, tokens[i].value.fval);
    // }

    int ec = 0;
    basic_ast_node_t *node = basic_parse(tokens, sz, &ec);
    if (ec) {
        *exitcode = 1;
        return NULL;
    }

    // print_ast(node, 0);

    while (**params == ' ' || **params == '\t') ++(*params);

    return node;
}

int visit_node(basic_ast_node_t *node, basic_value_t *out) {
    switch (node->type) {
        case BINOP:
            switch (node->value.sval[0]) {
                case '+': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;

                    if (lval.is_string || rval.is_string) {
                        out->is_string = true;
                        strcpy(out->sval, lval.sval);
                        strcat(out->sval, rval.sval);
                    } else {
                        out->is_string = false;
                        out->fval = lval.fval + rval.fval;
                    }
                    return 0;
                }
                case '-': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf("BASIC: - not supported on strings\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = lval.fval - rval.fval;
                    return 0;
                }
                case '*': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf(
                            "BASIC: * not supported on "
                            "strings\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = lval.fval * rval.fval;
                    return 0;
                }
                case '/': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf("BASIC: / not supported on strings\n");
                        return 1;
                    }
                    if (rval.fval == 0) {
                        printf("BASIC: Zero division error\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = lval.fval / rval.fval;
                    return 0;
                }
                case '%': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf("BASIC: %% not supported on strings\n");
                        return 1;
                    }
                    if (rval.fval == 0) {
                        printf("BASIC: Zero division error\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = fmod(lval.fval, rval.fval);

                    return 0;
                }
                case '=': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;

                    if (lval.is_string && rval.is_string) {
                        out->is_string = 0;
                        out->fval = strcmp(lval.sval, rval.sval) == 0;
                    } else if (!lval.is_string && !rval.is_string) {
                        out->is_string = 0;
                        out->fval = lval.fval == rval.fval;
                    } else {
                        out->is_string = 0;
                        out->fval = 0;
                    }
                    return 0;
                }
                case '~': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;

                    if (lval.is_string && rval.is_string) {
                        out->is_string = 0;
                        out->fval = strcmp(lval.sval, rval.sval) != 0;
                    } else if (!lval.is_string && !rval.is_string) {
                        out->is_string = 0;
                        out->fval = lval.fval != rval.fval;
                    } else {
                        out->is_string = 0;
                        out->fval = 1;
                    }
                    return 0;
                }
                case '<': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf(
                            "BASIC: < not supported on "
                            "strings\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = lval.fval < rval.fval;
                    return 0;
                }
                case '>': {
                    basic_value_t lval, rval;
                    if (visit_node(node->children[0], &lval)) return 1;
                    if (visit_node(node->children[1], &rval)) return 1;
                    if (lval.is_string || rval.is_string) {
                        printf(
                            "Error: > not supported on "
                            "strings!\n");
                        return 1;
                    }
                    out->is_string = 0;
                    out->fval = lval.fval > rval.fval;
                    return 0;
                }
                default: return 1;
            }
            break;
        case NUM:
        case STR: *out = node->value; return 0;
        case VAR: return get_var(node->value.sval, out);
        case VAR_DIM: {
            basic_value_t val;
            basic_value_t idx;

            memset(&val, 0, sizeof(basic_value_t));
            memset(&idx, 0, sizeof(basic_value_t));

            if (visit_node(node->children[0], &idx)) return 1;
            if (get_var(node->value.sval, &val)) return 1;

            if (!val.is_arr && !val.is_string) {
                printf("BASIC: %s is not DIM or STRING\n", node->value.sval);
                return 1;
            }

            if (val.is_string) {
                if ((int)idx.fval >= strlen(val.sval)) {
                    printf("BASIC: String %s out of bounds!\n",
                           node->value.sval);
                    return 1;
                }

                *out = (basic_value_t){false, val.sval[(int)idx.fval]};
            } else {
                if ((int)idx.fval >= val.arr_len) {
                    printf("BASIC: Array %s out of bounds!\n",
                           node->value.sval);
                    return 1;
                }

                *out = val.arr[(int)idx.fval];
            }
            return 0;
        }
        default: return 1;
    }
}

void basic_rec_free(basic_ast_node_t *node) {
    if (node) return;
    for (int i = 0; i < node->childrenln; ++i)
        basic_rec_free(node->children[i]);
    free(node);
}

void basic_free_stmt(int i) {
    if (code[i].type == NONE) return;
    for (int j = 0; j < code[i].paramln; ++j) basic_rec_free(code[i].params[j]);
}

void basic_free_code() {
    for (int i = 0; i < BASIC_MAX_STMTS; ++i) basic_free_stmt(i);

    for (int i = 0; i < varcount; ++i)
        if (vars[i].val.is_arr) free(vars[i].val.arr);
}

void exec_loaded_basic() {
    memset(vars, 0, sizeof(vars));
    varcount = 0;

    for (int i = 0; i < BASIC_MAX_STMTS; ++i) {
        switch (code[i].type) {
            case NONE: continue;
            case PRINT:
                for (int j = 0; j < code[i].paramln; ++j) {
                    basic_value_t val;
                    if (visit_node(code[i].params[j], &val)) return;
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
                    return;
                }
                basic_value_t condition, jmpcode;
                visit_node(code[i].params[0], &condition);
                visit_node(code[i].params[1], &jmpcode);
                if (condition.fval != 0) i = (int)jmpcode.fval - 1;
                break;
            }
            case GOTO: {
                if (code[i].paramln != 1) {
                    printf("BASIC: GOTO takes one INT parameter\n");
                    return;
                }
                if (code[i].params[0]->value.is_string) {
                    printf("BASIC: GOTO takes one INT parameter\n");
                    return;
                }
                basic_value_t jmpcode;
                visit_node(code[i].params[0], &jmpcode);
                i = (int)jmpcode.fval - 1;
                break;
            }
            case INPUT: {
                if (code[i].paramln != 2) {
                    printf("BASIC: INPUT takes two parameters, VAR, TYPE\n");
                    return;
                }

                char inbuf[512];
                scanl(inbuf, 512);

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
            case LET: {
                basic_value_t val;
                memset(&val, 0, sizeof(basic_value_t));

                if (visit_node(code[i].params[1], &val)) return;
                if (code[i].params[0]->type == VAR_DIM) {
                    basic_value_t arr;
                    basic_value_t idx;
                    if (get_var(code[i].params[0]->value.sval, &arr)) return;
                    if (visit_node(code[i].params[0]->children[0], &idx))
                        return;
                    arr.arr[(int)idx.fval] = val;
                    set_var(code[i].params[0]->value.sval, arr);
                } else if (code[i].params[0]->type == VAR) {
                    set_var(code[i].params[0]->value.sval, val);
                }
                break;
            }
            case DIM: {
                const char *dim_name = code[i].params[0]->value.sval;

                basic_value_t dim_size;

                if (visit_node(code[i].params[0]->children[0], &dim_size))
                    return;

                basic_value_t dim_val;
                dim_val.is_arr = true;
                dim_val.is_string = false;
                dim_val.arr =
                    malloc(sizeof(basic_value_t) * (int)dim_size.fval);
                dim_val.arr_len = (int)dim_size.fval;

                set_var(dim_name, dim_val);
                break;
            }
            case SRAND: {
                basic_value_t seed;
                if (visit_node(code[i].params[0], &seed)) return;
                if (seed.is_string ||
                    ((!seed.is_string) && ((int)seed.fval) != seed.fval)) {
                    printf("BASIC: RANDOMIZE takes INT");
                    return;
                }
                srand((int)seed.fval);
                break;
            }
            case CLEAR: {
                if (code[i].paramln != 0) {
                    printf("BASIC: CLEAR takes no parameters\n");
                    return;
                }
                clear_screen();
                break;
            }
            case END: {
                if (code[i].paramln != 0) {
                    printf("BASIC: END takes no parameters\n");
                    return;
                }
                return;
                break;
            }
        }
    }

    basic_free_code();
}

int process_line(char *ln, int *rln, int *stmtlen) {
    while (*ln == ' ' || *ln == '\t') ++ln;

    char *pln;
    long int line_num = extract_line_num(ln, &pln);

    if (stmtlen) ++(*stmtlen);

    if (line_num == -1) {
        if (rln)
            printf("Missing line number on line %d\n", *rln);
        else
            printf("Missing line number\n");
        return 1;
    }

    basic_free_stmt(line_num);
    code[line_num] = (basic_stmt_t){0};
    strncpy(code_str[line_num], ln, 128);

    if (strncmp(pln, "REM ", 4) == 0) {
        if (rln) ++(*rln);
        return 0;
    }

    int exitcode = 0;
    basic_stmt_type_t type =
        extract_stmt_type(&pln, rln ? *rln : -1, &exitcode);
    if (exitcode) return 1;

    while (*pln == ' ' || *pln == '\t') ++pln;

    code[line_num].type = type;

    basic_ast_node_t *param;

    while ((param = extract_param(&pln, &exitcode))) {
        if (exitcode) return 1;
        code[line_num].params[code[line_num].paramln++] = param;
    }

    if (rln) ++(*rln);
    return 0;
}

void save_basic_to_file(const char *relpath) {
    char path[256];
    memset(path, 0, 256);
    relative_to_user_pwd(relpath, path);

    char buffer[60000];
    memset(buffer, 0, 60000);
    for (int i = 0; i < BASIC_MAX_STMTS; ++i)
        if (strlen(code_str[i]) > 0) {
            strcat(buffer, code_str[i]);
            strcat(buffer, "\n");
        }

    write_file(path, buffer);
}

void load_basic_from_file(const char *relpath) {
    char path[256];
    memset(path, 0, 256);
    relative_to_user_pwd(relpath, path);

    static const int buflen = 60000;
    char buffer[buflen];
    int bytesread = read_file(path, (uint8_t *)(&buffer[0]), buflen);
    if (bytesread == -1) return;
    buffer[bytesread] = '\0';

    memset(code, 0, sizeof(code));
    memset(code_str, 0, sizeof(code_str));

    int stmtlen = 0;
    char *ln = strtok((char *)buffer, "\n");
    int rln = 1;

    while (ln) {
        if (process_line(ln, &rln, &stmtlen)) return;
        ln = strtok(NULL, "\n");
    }
}

void run_basic(char *src) {
    memset(code, 0, sizeof(code));
    memset(code_str, 0, sizeof(code_str));

    int stmtlen = 0;
    char *ln = strtok((char *)src, "\n");
    int rln = 1;

    while (ln) {
        if (process_line(ln, &rln, &stmtlen)) return;
        ln = strtok(NULL, "\n");
    }

    exec_loaded_basic();
    basic_free_code();
}

void list_basic_code() {
    for (int i = 0; i < BASIC_MAX_STMTS; ++i)
        if (strlen(code_str[i]) > 0) printf("%s\n", code_str[i]);
}

void basic_shell() {
    printf("TRACE BASIC\nSimple BASIC Interpreter for TRACE\n\n");
    memset(code, 0, sizeof(code));
    memset(code_str, 0, sizeof(code_str));

    char cmdbuf[512];
    memset(cmdbuf, 0, sizeof(cmdbuf));

    while (true) {
        printf("[BASIC] ");
        scanl(cmdbuf, 512);

        if (strcmp(cmdbuf, "EXIT") == 0)
            break;
        else if (strcmp(cmdbuf, "RUN") == 0)
            exec_loaded_basic();
        else if (strcmp(cmdbuf, "CLEAR") == 0) {
            memset(code, 0, sizeof(code));
            memset(code_str, 0, sizeof(code_str));
        } else if (strcmp(cmdbuf, "LIST") == 0)
            list_basic_code();
        else if (strncmp(cmdbuf, "LOAD ", 5) == 0)
            load_basic_from_file(cmdbuf + 5);
        else if (strncmp(cmdbuf, "SAVE ", 5) == 0)
            save_basic_to_file(cmdbuf + 5);
        else
            process_line(cmdbuf, NULL, NULL);
    }
    basic_free_code();
}
