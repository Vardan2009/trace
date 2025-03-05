#include "trace_basic/lexer.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "shell.h"

basic_token_t basic_tokens[BASIC_MAX_TOKENS];
int basic_tokens_len = 0;

int is_ident(char chr) { return (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z'); }
int is_digit(char chr) { return (chr >= '0' && chr <= '9'); }
int is_operator(char chr) {
    return chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '%' || chr == ';';
}

char *srcptr;
char *endptr;

void read_ident(char buffer[BASIC_MAX_TOKEN_LEN]) {
    int i = 0;
    while (is_ident(*srcptr) && i < BASIC_MAX_TOKEN_LEN - 1 && srcptr < endptr) {
        buffer[i++] = *(srcptr++);
    }
    buffer[i] = '\0';
}

void read_string(char buffer[BASIC_MAX_TOKEN_LEN]) {
    ++srcptr;
    int i = 0;
    while (*srcptr != '"' && *srcptr != '\0' && i < BASIC_MAX_TOKEN_LEN - 1 && srcptr < endptr) {
        buffer[i++] = *(srcptr++);
    }
    buffer[i] = '\0';
    if (*srcptr == '"') ++srcptr;
}

void read_number(char buffer[BASIC_MAX_TOKEN_LEN]) {
    int i = 0;
    while ((is_digit(*srcptr) || *srcptr == '.') && i < BASIC_MAX_TOKEN_LEN - 1 && srcptr < endptr) {
        buffer[i++] = *(srcptr++);
    }
    buffer[i] = '\0';
}

void skip_comment() {
    if (*srcptr == '\'' || strncmp(srcptr, "REM ", 4) == 0) {
        while (*srcptr != '\n' && *srcptr != '\0' && srcptr < endptr) {
            ++srcptr;
        }
    }
}

void basic_lex(const char *srcbuf, int srcl) {
    if (!srcbuf) {
        print_err("BASIC: Lexer received NULL pointer");
    }

    basic_tokens_len = 0;
    memset(basic_tokens, 0, sizeof(basic_tokens));

    srcptr = (char *)srcbuf;
    endptr = srcptr + srcl;

    while (*srcptr) {
        char chr = *srcptr;

        if (chr == ' ' || chr == '\n' || chr == '\t') {
            ++srcptr;
            continue;
        } 
        else if (is_ident(chr)) {
            basic_token_t ident_token = { .type = IDENT, .value = "" };
            read_ident(ident_token.value);
            basic_tokens[basic_tokens_len++] = ident_token;
            continue;
        } 
        else if (is_digit(chr)) {
            basic_token_t number_token = { .type = NUMBER, .value = "" };
            read_number(number_token.value);
            basic_tokens[basic_tokens_len++] = number_token;
            continue;
        } 
        else if (chr == '$') {
            ++srcptr;
            basic_token_t var_token = { .type = VARIABLE, .value = "" };
            read_ident(var_token.value);
            basic_tokens[basic_tokens_len++] = var_token;
            continue;
        } 
        else if (is_operator(chr)) {
            basic_token_t op_token = { .type = OPERATOR, .value = { chr, '\0' } };
            ++srcptr;
            basic_tokens[basic_tokens_len++] = op_token;
            continue;
        } 
        else if (chr == '(') {
            basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = LPAREN, .value = "(" };
            ++srcptr;
            continue;
        } 
        else if (chr == ')') {
            basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = RPAREN, .value = ")" };
            ++srcptr;
            continue;
        } 
        else if (chr == '"') {
            basic_token_t str_token = { .type = STRING, .value = "" };
            read_string(str_token.value);
            basic_tokens[basic_tokens_len++] = str_token;
            continue;
        } 
        else if (chr == '\'') {
            skip_comment();
            continue;
        } 
        else if ((chr == '<' || chr == '>') && (*(srcptr + 1) == '=' || (chr == '<' && *(srcptr + 1) == '>'))) {
            basic_token_t op_token = { .type = OPERATOR, .value = { chr, *(srcptr + 1), '\0' } };
            srcptr += 2;
            basic_tokens[basic_tokens_len++] = op_token;
            continue;
        } 
        else {
            print_err("BASIC: Unexpected Symbol `%c`, skipping...", chr);
            ++srcptr;
            continue;
        }
    }
}