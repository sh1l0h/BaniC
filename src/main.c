#include <argp.h>

#include "../include/preprocessor.h"

#define SAVE_TEMPS_KEY 300

static char doc[] = "Bani compiler.";
static char args_doc[] = "FILE...";
static struct argp_option options[] = {
    { NULL, 'S', NULL, 0, "Compile only; do not assemble or link.", 1 },
    { NULL, 'c', NULL, 0, "Compile and assemble, but do not link.", 1 },
    { "output", 'o', "FILE", 0, "Place the output into FILE.", 1 },
    { "target", 't', "TARGER", 0, "Set output assembler to TARGET; TARGET can be: nasm32, nasm64.", 1 },
    { "save-temps", SAVE_TEMPS_KEY, NULL, 0, "Save intermediate files.", 2 },
    { 0 }
};

struct ArgpArguments {
    bool save_temps;
    char *input_file, *output_path;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct ArgpArguments *arguments = state->input;

    switch (key) {

    case 'o':
        arguments->output_path = arg;
        break;

    case SAVE_TEMPS_KEY:
        arguments->save_temps = true;
        break;

    case ARGP_KEY_NO_ARGS:
        argp_error(state, "No input files");
        return ARGP_ERR_UNKNOWN;

    case ARGP_KEY_END:
        break;

    case ARGP_KEY_ARG:
        if (arguments->input_file == NULL) {

            arguments->input_file = arg;
            break;
        }

        return ARGP_ERR_UNKNOWN;

    default:
        return ARGP_ERR_UNKNOWN;

    }

    return 0;
}

int main(int argc, char **argv)
{
    log_init();

    struct ArgpArguments arguments = {
        .input_file = NULL,
        .output_path = NULL,
        .save_temps = false
    };

    struct argp argp = { options, parse_opt, args_doc, doc, NULL, NULL, NULL };
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    char *pp_output_path;
    preprocess(arguments.input_file, &pp_output_path);

    return EXIT_SUCCESS;
}
