#include "trace_basic/lexer.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "shell.h"

int is_ident(char chr) { return (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z'); }
int is_digit(char chr) { return (chr >= '0' && chr <= '9'); }

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

void basic_lex(const char *srcbuf, int srcl, basic_token_t *basic_tokens, int *final_len) {
    if (!srcbuf) {
        print_err("BASIC: Lexer received NULL pointer");
        return;
    }

    int basic_tokens_len = 0;
    memset(basic_tokens, 0, sizeof(basic_token_t[BASIC_MAX_TOKENS]));

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
        else if (chr == '+') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = PLUS, .value = "+" };
        else if (chr == '-') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = MINUS, .value = "-" };
        else if (chr == '*') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = MUL, .value = "*" };
        else if (chr == '/') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = DIV, .value = "/" };
        else if (chr == '%') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = MOD, .value = "%" };
        else if (chr == '=') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = ASSIGN, .value = "=" };
        else if (chr == '(') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = LPAREN, .value = "(" };
        else if (chr == ')') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = RPAREN, .value = ")" };
        else if (chr == ';') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = CONCAT, .value = ";" };
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
        else if (chr == '<') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = LT, .value = "<" };
        else if (chr == '>') basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = GT, .value = ">" };
        else if (strncmp(srcptr, "<=", 2) == 0) { basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = LTE, .value = "<=" }; srcptr++; }
        else if (strncmp(srcptr, ">=", 2) == 0) { basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = GTE, .value = ">=" }; srcptr++; }
        else if (strncmp(srcptr, "<>", 2) == 0) { basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = NEQ, .value = "<>" }; srcptr++; }
        else if (strncmp(srcptr, "==", 2) == 0) { basic_tokens[basic_tokens_len++] = (basic_token_t) { .type = EQ, .value = "==" }; srcptr++; }
        else { print_err("BASIC: Unexpected Symbol `%c`", chr); }
        ++srcptr;
    }
    *final_len = basic_tokens_len;
}