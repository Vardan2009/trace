#include "trace_basic/basic.h"
#include "trace_basic/lexer.h"
#include "lib/stdio.h"

void run_basic(const char *srcbuf, int srclen) {
	basic_lex(srcbuf, srclen);

	for(int i = 0; i < basic_tokens_len; ++i)
		printf("%d(%s)\n", basic_tokens[i].type, basic_tokens[i].value);
}