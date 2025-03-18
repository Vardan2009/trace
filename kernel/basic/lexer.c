#include "basic/lexer.h"

#include "lib/stdio.h"
#include "lib/stdlib.h"
#include "lib/string.h"

bool alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool digit(char c) {
    return c >= '0' && c <= '9';
}

int basic_tokenize(char **ptr, basic_token_t *tokens) {
    if (strlen(*ptr) == 0) return 0;
    // printf("Tokenizing `%s`\n", *ptr);
    int token_count = 0;
    while (**ptr != 0 && **ptr != ',') {
        if (**ptr == ' ' || **ptr == '\t') {
            ++(*ptr);
        } else if (**ptr == ',')
            break;
        else if (**ptr == '"') {
            ++(*ptr);
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));

            int buflen = 0;
            while (**ptr != '"' && **ptr != 0) buffer[buflen++] = *((*ptr)++);
            buffer[buflen] = 0;
            if (**ptr == '"') ++(*ptr);
            int idx = token_count++;
            tokens[idx].type = TOK_STR;
            tokens[idx].value.is_string = true;
            strncpy(tokens[idx].value.sval, buffer, 64);
            tokens[idx].value.sval[63] = 0;
        } else if (alpha(**ptr)) {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            int buflen = 0;
            while (alpha(**ptr) || digit(**ptr)) buffer[buflen++] = *((*ptr)++);
            int idx = token_count++;
            tokens[idx].type = TOK_VAR;
            tokens[idx].value.is_string = true;
            strncpy(tokens[idx].value.sval, buffer, 64);
            tokens[idx].value.sval[63] = 0;
        } else if (digit(**ptr)) {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));

            int buflen = 0;
            while (digit(**ptr)) buffer[buflen++] = *((*ptr)++);
            int idx = token_count++;
            tokens[idx].type = TOK_NUM;
            tokens[idx].value.fval = stof(buffer);
        } else if (**ptr == '+') {
            tokens[token_count++].type = TOK_PLUS;
            ++(*ptr);
        } else if (**ptr == '-') {
            tokens[token_count++].type = TOK_MINUS;
            ++(*ptr);
        } else if (**ptr == '*') {
            tokens[token_count++].type = TOK_MUL;
            ++(*ptr);
        } else if (**ptr == '/') {
            tokens[token_count++].type = TOK_DIV;
            ++(*ptr);
        }
        else if (**ptr == '%') {
            tokens[token_count++].type = TOK_MOD;
            ++(*ptr);
        } 
        else if (**ptr == '<') {
            tokens[token_count++].type = TOK_LT;
            ++(*ptr);
        } else if (**ptr == '>') {
            tokens[token_count++].type = TOK_GT;
            ++(*ptr);
        } else if (**ptr == '=') {
            tokens[token_count++].type = TOK_EQ;
            ++(*ptr);
        } else if (**ptr == '~') {
            tokens[token_count++].type = TOK_NEQ;
            ++(*ptr);
        } else if (**ptr == '(') {
            tokens[token_count++].type = TOK_LPAREN;
            ++(*ptr);
        } else if (**ptr == ')') {
            tokens[token_count++].type = TOK_RPAREN;
            ++(*ptr);
        } else {
            printf("Invalid character `%c`\n", **ptr);
            return -1;
        }
    }
    return token_count;
}
