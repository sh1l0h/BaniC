#include "lexer.h"
#include <errno.h>
#include <limits.h>
#include <ctype.h>

int lexer_init(struct lexer *lex, const char *file_path)
{
	lex->file_path = file_path;
	lex->file = fopen(file_path, "r");
	if (!lex->file)
		return -errno;
	return 0;
}

int lexer_deinit(struct lexer *lex)
{
	int res = fclose(lex->file);

	if (!res)
		return -errno;
	return 0;
}

static int lexer_match(struct lexer *lexer, char c)
{
	int curr_char = fgetc(lexer->file);
	int res;

	if (ferror(lexer->file))
		return -errno;

	if (curr_char == c)
		return 1;

	res = fseek(lexer->file, -1, SEEK_CUR);
	if (res == -1)
		return -errno;

	return 0;
}

static int lexer_handle_digit(struct lexer *lexer, struct token *result)
{
	int curr_char;
	bool was_last_dot = false;

	result->type = TT_INT;

	do {
		curr_char = fgetc(lexer->file);

		if (ferror(lexer->file))
			return -errno;

		if (was_last_dot) {
			if (!isdigit(curr_char)) {
				lexer->err_reason = "Expected digit after dot";
				return -EINVAL;
			}
			was_last_dot = false;
		}

		if (curr_char == '.') {
			if (result->type == TT_FLOAT) {
				lexer->err_reason = "Second dot in float";
				return -EINVAL;
			}
			result->type = TT_FLOAT;
			was_last_dot = true;
		}

	} while (isdigit(curr_char) || was_last_dot);


	if (!isspace(curr_char) && curr_char != EOF) {
		lexer->err_reason = "Nonblack character after digit";
		return -EINVAL;
	}

	return 0;
}

int lexer_next_token(struct lexer *lexer, struct token *result)
{
	bool repeat;

	do {
		int curr_char;
		long pos = ftell(lexer->file);
		int res;

		if (pos == -1)
			return -errno;

		if ((unsigned long) pos > UINT_MAX)
			return -ERANGE;

		result->pos = (unsigned int) pos;
		curr_char = fgetc(lexer->file);

		repeat = false;
		switch (curr_char) {
		case EOF:
			if (!feof(lexer->file))
				return -errno;
			result->type = TT_EOF;
			break;

		case '+':
			res = lexer_match(lexer, '=');
			if (res < 0)
				return res;
			if (res)
				result->type = TT_PLUS_EQ;
			else
				result->type = TT_PLUS;
			break;

		case '-':
			res = lexer_match(lexer, '=');
			if (res < 0)
				return res;
			if (res)
				result->type = TT_MINUS_EQ;
			else
				result->type = TT_MINUS;
			break;

		case '*':
			res = lexer_match(lexer, '=');
			if (res < 0)
				return res;
			if (res)
				result->type = TT_STAR_EQ;
			else
				result->type = TT_STAR;
			break;

		case '/':
			res = lexer_match(lexer, '=');
			if (res < 0)
				return res;
			if (res)
				result->type = TT_SLASH_EQ;
			else
				result->type = TT_SLASH;
			break;

		case ' ':
		case '\n':
		case '\r':
		case '\t':
			repeat = true;
			break;

		default:
			if (isdigit(curr_char)) {
				res = lexer_handle_digit(lexer, result);
				if (res < 0)
					return res;
			}
		}

	} while (repeat);

	return 0;
}
