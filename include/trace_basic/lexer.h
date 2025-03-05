#ifndef BASIC_LEXER_H
#define BASIC_LEXER_H

#define BASIC_MAX_TOKENS 512
#define BASIC_MAX_TOKEN_LEN 128

typedef enum {
	IDENT,
	NUMBER,
	STRING,
	VARIABLE,
	PLUS,
	MINUS,
	DIV,
	MUL,
	MOD,
	ASSIGN,
	EQ,
	NEQ,
	GT,
	LT,
	GTE,
	LTE,
	CONCAT,
	LPAREN,
	RPAREN,
} basic_token_type_t;

typedef struct {
	basic_token_type_t type;
	char value[BASIC_MAX_TOKEN_LEN];
} basic_token_t;

void basic_lex(const char *, int, basic_token_t *, int *);

#endif // BASIC_LEXER_H