#ifndef BANIC_TOKEN_H
#define BANIC_LEXER_H

#include "token.h"
#include <stdio.h>

struct lexer {
	const char *file_path;
	FILE *file;
	char *err_reason;
};

int lexer_init(struct lexer *lex, const char *file_path);
int lexer_deinit(struct lexer *lex);

int lexer_next_token(struct lexer *lex, struct token *token);

#endif
