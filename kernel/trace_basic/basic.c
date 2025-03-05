#include "trace_basic/basic.h"
#include "trace_basic/lexer.h"
#include "trace_basic/parser.h"
#include "lib/stdio.h"
#include "lib/malloc.h"
#include "shell.h"

void print_node(basic_node_t *node, int indent) {
	for(int i = 0; i < indent * 4; ++i) putc(' ');

	switch(node->value_type) {
	case T_FLOAT:
		printf("node_type_t %d [float %f]\n",
			node->type,
			node->value_num
		);
		break;
	case T_TOKTYPE:
		printf("node_type_t %d [toktype %d]\n",
			node->type,
			node->value_token_type
		);
		break;
	case T_STRING:
		printf("node_type_t %d [string \"%s\"]\n",
			node->type,
			node->value_str
		);
		break;
	default:
		printf("node_type_t %d\n", node->type);
	}

	for(int i = 0; i < node->children_count; ++i)
		print_node(node->children[i], indent + 1);
}

void run_basic(const char *srcbuf, int srclen) {
	basic_token_t *tokens = (basic_token_t *)malloc(BASIC_MAX_TOKENS);
	if(!tokens) {
		print_err("BASIC: malloc failed");
		return;
	}

	int tokens_len;
	basic_lex(srcbuf, srclen, tokens, &tokens_len);

	for(int i = 0; i < tokens_len; ++i)
		printf("%d(%s)\n", tokens[i].type, tokens[i].value);

	basic_node_t *root = basic_parse(tokens, tokens_len);

	print_node(root, 0);
	free_basic_node(root);
	free(tokens);
}