#ifndef BASIC_PARSER_H
#define BASIC_PARSER_H

#define BASIC_MAX_NODES 2048
#define BASIC_NODE_MAX_CHILDREN 16

#include "trace_basic/lexer.h"

#define BASIC_STMT_END "END"

typedef enum {
	ROOT,
	BINOP,
	ASSIGN_NODE,
	PRINT,
	NUM_LIT,
	STR_LIT,
	VARIABLE_NODE,
} basic_node_type_t;

typedef enum {
	T_NONE,
	T_FLOAT,
	T_TOKTYPE,
	T_STRING
} node_value_dtype_t;

typedef struct basic_node {
	basic_node_type_t type;
	
	node_value_dtype_t value_type;
	float value_num;
	basic_token_type_t value_token_type;
	char value_str[64];

	struct basic_node **children;
	int children_count;
} basic_node_t;

basic_node_t *create_basic_node(basic_node_type_t);
void add_child(basic_node_t *, basic_node_t *);
void free_basic_node(basic_node_t *);

basic_node_t *parse_root_node();
basic_node_t *parse_statement();
basic_node_t *parse_expr();
basic_node_t *parse_term();
basic_node_t *parse_factor();

basic_node_t *basic_parse(const basic_token_t *, int);

#endif // BASIC_PARSER_H