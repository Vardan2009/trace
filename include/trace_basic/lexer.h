#ifndef BASIC_LEXER_H
#define BASIC_LEXER_H

#define BASIC_MAX_TOKENS 2048
#define BASIC_MAX_TOKEN_LEN 128

typedef enum {
	IDENT,
	NUMBER,
	STRING,
	VARIABLE,
	OPERATOR,
	LPAREN,
	RPAREN,
} basic_token_type_t;

typedef struct {
	basic_token_type_t type;
	char value[BASIC_MAX_TOKEN_LEN];
} basic_token_t;

extern basic_token_t basic_tokens[BASIC_MAX_TOKENS];
extern int basic_tokens_len;
void basic_lex(const char *, int);

#endif // BASIC_LEXER_H