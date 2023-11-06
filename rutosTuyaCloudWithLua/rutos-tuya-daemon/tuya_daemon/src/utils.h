#ifndef UTILS_H
#define UTILS_H

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"
#include "parser.h"
#include "cJSON.h"

#define BD_MAX_CLOSE 8192

int become_daemon();
void load_device_info(struct arguments arguments, char *product_id, char *device_id, char *device_secret);
void replace_quotes(char *string);
int is_lua_file(char *filename);

#endif