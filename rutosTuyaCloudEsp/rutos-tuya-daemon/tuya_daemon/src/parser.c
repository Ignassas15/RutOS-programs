#include "parser.h"

const char *argp_program_version = "tuya-cloud-daemon 1.0.0";
static char doc[] = "A daemon program for connecting to tuya cloud";
static char args_doc[] = "device_id device_secret product_id";

static struct argp_option options[]={
    {"daemon", 'd', 0, 0, "Run as daemon", 0},
    {0}
};

static struct argp argp = {options, parse_opt, args_doc, doc};


/**
 * parse_opt() - Function for parsing individual arguments 
 * More information can be found: https://www.gnu.org/software/libc/manual/html_node/Argp-Parser-Functions.html
 */
error_t parse_opt(int key, char *arg, struct argp_state *state){
    struct arguments *arguments = state->input;

    switch(key){
        case 'd':
            arguments->daemon = 1;
            break;
        case ARGP_KEY_ARG:
            if(state->arg_num >= ARG_COUNT){
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if(state->arg_num < ARG_COUNT){
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


/**
 * parse_arguments() - Parses arguments passed to the program
 * @param argc - Passed argument count
 * @param argv - Passed arguments
 * @param arguments - Pointer to the struct in which the parsed arguments will be stored
 * @return: Returns 0 if successfull 1 if not
 */
int parse_arguments(int argc, char **argv, struct arguments *arguments){
    return argp_parse(&argp, argc, argv, 0, 0, arguments);
}

/**
 * init_arguments() - Initializes options to 0
 * @param arguments - Pointer to the struct of arguments
 */
void init_arguments(struct arguments *arguments){
    arguments->daemon = 0;
}