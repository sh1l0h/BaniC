#include <argp.h>

#include "lexer.h"

int main(int argc, char **argv)
{
	struct lexer lexer;
	struct token token;

	lexer_init(&lexer, "test.b");

	do {
		int res = lexer_next_token(&lexer, &token);
		if (res)
			return res;
	} while (token.type != TT_EOF);

	return 0;
}
