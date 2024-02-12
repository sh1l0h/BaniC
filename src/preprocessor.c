#include "../include/preprocessor.h"

static char *get_token(char *input_line, Location *loc)
{
    static char *line = NULL;
    static size_t column = 0;

    if (input_line != NULL) {
        line = input_line;
        column = 0;
    }

    while (isspace(line[column]))
        column++;

    if (line[column] == '\0')
        return NULL;

    char *result = line + column;
    loc->column_start = column + 1;

    while (!isspace(line[column]) && line[column] != '\0')
        column++;

    loc->column_end = column;
    line[column++] = '\0';

    return result;
}

static int preprocess_file(char *input_path, FILE *input_file,
                           FILE *output_file)
{
    int result = 0;

    size_t line_num = 0;

    char *line = NULL;
    size_t line_buffer_size = 0;
    ssize_t line_len;

    while ((line_len = getline(&line, &line_buffer_size, input_file)) != -1) {
        line_num++;

        if (line_len == 0)
            continue;

        if (line[0] == '#') {
            Location directive_loc;
            directive_loc.file_path = input_path;
            directive_loc.line = line_num;

            char *directive = get_token(line, &directive_loc);

            if (strcmp(directive, "#copy") != 0) {
                log_error_with_loc(&directive_loc,
                                   "Invalid preprocessing directive \"%s\".",
                                   directive);

                result = 1;
                continue;
            }

            Location copy_path_loc;
            copy_path_loc.file_path = input_path;
            copy_path_loc.line = line_num;

            char *copy_path = get_token(NULL, &copy_path_loc);
            size_t copy_path_len = strlen(copy_path);

            if ((copy_path_len < 2 ||
                    copy_path[0] != '\"' ||
                    copy_path[copy_path_len - 1] != '\"')) {

                log_error_with_loc(&copy_path_loc,
                                   "\"#copy\" expects a string literal.");

                result = 1;
                continue;
            }

            copy_path[copy_path_len - 1] = '\0';
            copy_path++;

            char *copy_absolute_path = realpath(copy_path, NULL);

            if (copy_absolute_path == NULL) {
                log_error_with_loc(&copy_path_loc, "%s: %s.", copy_path, strerror(errno));

                result = 1;
                continue;
            }

            FILE *copy_file = fopen(copy_absolute_path, "r");

            if (copy_file == NULL) {
                log_error_with_loc(&copy_path_loc, "%s: %s.", copy_path, strerror(errno));

                free(copy_absolute_path);

                result = 1;
                continue;
            }

            fprintf(output_file, "#1 %s\n", copy_path);

            result = preprocess_file(copy_path, copy_file, output_file);
            fclose(copy_file);

            if (result != 0)
                log_info_with_loc(&copy_path_loc, "\"%s\" copied here", copy_path);
            else
                fprintf(output_file, "#%ld %s\n", line_num, input_path);

            continue;
        }

        bool inside_str_literal = false;

        for (char *curr = line; *curr != '\0'; curr++) {

            switch (*curr) {

            case '\"':
                inside_str_literal = !inside_str_literal;
                break;

            case '/':
                if (!inside_str_literal && curr[1] == '/') {
                    *curr = '\0';
                    goto end_while;
                }
                break;

            }

        }

    end_while:
        fprintf(output_file, "%s", line);

    }

    free(line);

    return result;
}

int preprocess(char *input_path, char **output_path)
{
    FILE *input_file = fopen(input_path, "r");

    if (input_file == NULL) {
        log_fatal("%s: %s.", input_path, strerror(errno));
        return 1;
    }

    char *input_base_name = get_base_name(input_path);
    size_t input_base_name_len = strlen(input_base_name);

    *output_path = malloc(sizeof(char *) * (input_base_name_len + 2));
    strcpy(*output_path, input_base_name);

    (*output_path)[input_base_name_len] = 'i';
    (*output_path)[input_base_name_len + 1] = '\0';

    FILE *output_file = fopen(*output_path, "w");

    if (output_file == NULL) {
        log_fatal("%s: %s.", *output_path, strerror(errno));

        free(output_path);
        fclose(input_file);

        return 1;
    }

    int result = preprocess_file(input_path, input_file, output_file);

    fclose(input_file);
    fclose(output_file);

    return result;
}
