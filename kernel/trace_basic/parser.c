#include "trace_basic/parser.h"
#include "lib/malloc.h"
#include "lib/stdlib.h"
#include "lib/string.h"
#include "shell.h"

basic_node_t* create_basic_node(basic_node_type_t type) {
	basic_node_t* node = malloc(sizeof(basic_node_t));
	if (!node) return NULL;
	node->type = type;
	node->children = NULL;
	node->children_count = 0;
	node->value_type = T_NONE;

	return node;
}

void add_child(basic_node_t *parent, basic_node_t *child) {
	if (!parent || !child) return;
	parent->children = realloc(parent->children, 
							  (parent->children_count + 1) * sizeof(basic_node_t*));
	if (!parent->children) return;
	parent->children[parent->children_count++] = child;
}

void free_basic_node(basic_node_t *node) {
	if (!node) return;
	for (int i = 0; i < node->children_count; i++) free_basic_node(node->children[i]);
	free(node->children);
	free(node);
}

basic_token_t *tokptr;
basic_token_t *tokendptr;

basic_token_t *peek() { return tokptr; }
basic_token_t *advance() { return tokptr++; }

basic_token_t *consume_t(basic_node_type_t type) {
	if(tokptr->type == type) return advance();
	else {
		print_err("BASIC: Expected token of type %d, got %d\n", type, tokptr->type);
		return NULL;
	}
} 

basic_token_t *consume(basic_node_type_t type, const char *value) {
	if(tokptr->type == type && strcmp(tokptr->value, value) == 0) return advance();
	else {
		print_err("BASIC: Expected %s, got %s\n", value, tokptr->value);
		return NULL;
	}
} 

#define SEEING_TOKEN(TYPE, VALUE) (peek()->type == (TYPE) && strcmp(peek()->value, (VALUE)) == 0)
#define SEEING_TOKEN_T(TYPE) (peek()->type == (TYPE))

basic_node_t *parse_root_node() {
	basic_node_t *root = create_basic_node(ROOT);
	while(tokptr < tokendptr) {
		if(SEEING_TOKEN(IDENT, BASIC_STMT_END)) break;
		add_child(root, parse_statement());
	}
	return root;
}

basic_node_t *parse_statement() {
	if(SEEING_TOKEN(IDENT, "PRINT")) {
		consume(IDENT, "PRINT");
		basic_node_t *print_stmt = create_basic_node(PRINT);
		add_child(print_stmt, parse_expr());
		return print_stmt;
	}
	else return parse_expr();
}

basic_node_t *parse_expr() {
	basic_node_t *n = parse_term();
	while(true) {
		basic_token_t *op = peek();
		if
		(
			op->type == PLUS ||
			op->type == MINUS ||
			op->type == EQ ||
			op->type == NEQ ||
			op->type == GT ||
			op->type == LT ||
			op->type == GTE ||
			op->type == LTE
		) {
			advance();
			basic_node_t *r = parse_term();
			basic_node_t *res = create_basic_node(BINOP);
			res->value_token_type = op->type;
			res->value_type = T_TOKTYPE;
			add_child(res, n);
			add_child(res, r);
			n = res;
		} else if (op->type == ASSIGN) {
			advance();
			basic_node_t *r = parse_expr();
			basic_node_t *res = create_basic_node(ASSIGN_NODE);
			add_child(res, n);
			add_child(res, r);
			n = res;
		} else break;
	}
	return n;
}

basic_node_t *parse_term() {
	basic_node_t *n = parse_factor();
	while(
		peek()->type == MUL ||
		peek()->type == DIV ||
		peek()->type == MOD
	) {
		basic_token_t *op = advance();
		basic_node_t *r = parse_factor();
		basic_node_t *res = create_basic_node(BINOP);
		res->value_token_type = op->type;
		res->value_type = T_TOKTYPE;

		add_child(res, n);
		add_child(res, r);
		n = res;
	}
	return n;
}

basic_node_t *parse_factor() {
	basic_token_t *t = peek();
	switch(t->type) {
		case NUMBER: {
			basic_token_t *op = advance();
			basic_node_t *res = create_basic_node(NUM_LIT);
			res->value_num = stoi(op->value);
			res->value_type = T_FLOAT;

			return res;
		}
		case STRING: {
			basic_token_t *op = advance();
			basic_node_t *res = create_basic_node(STR_LIT);
			strcpy(res->value_str, op->value);
			res->value_type = T_STRING;
			return res;
		}
		case VARIABLE: {
			basic_token_t *op = advance();
			basic_node_t *res = create_basic_node(VARIABLE_NODE);
			strcpy(res->value_str, op->value);
			res->value_type = T_STRING;
			return res;
		}
		default:
			print_err("BASIC: Invalid factor %s", t->value);
			return NULL;
	}
}

basic_node_t *basic_parse(const basic_token_t *tokens, int tokens_count) {
	if(!tokens) {
		print_err("BASIC: Parser received NULL pointer");
		return NULL;
	}

	tokptr = (basic_token_t *)tokens;
	tokendptr = (basic_token_t *)tokens + tokens_count;

	return parse_root_node();
}