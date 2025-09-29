#ifndef BANIC_TOKEN_H
#define BANIC_TOKEN_H

enum token_type {
	TT_EOF = 0,

	TT_PLUS,
	TT_MINUS,
	TT_STAR,
	TT_SLASH,

	TT_PLUS_EQ,
	TT_MINUS_EQ,
	TT_STAR_EQ,
	TT_SLASH_EQ,

	TT_WORD,
	TT_INT,
	TT_FLOAT
};

struct token {
	enum token_type type;
	unsigned int pos;
};

#endif
