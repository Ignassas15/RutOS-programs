#ifndef PARSER_H
#define PARSER_H

#include <argp.h>
#include <syslog.h>

#define ARG_COUNT 3

struct arguments{
    char *args[ARG_COUNT];
    int daemon;
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

int parse_arguments(int argc, char **argv, struct arguments *arguments);

void init_arguments(struct arguments *arguments);


#endif