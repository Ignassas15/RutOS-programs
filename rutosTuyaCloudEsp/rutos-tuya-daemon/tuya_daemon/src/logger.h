#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

void init_logger();

void close_logger();

void log_info(const char *message);

void log_error(const char *message);

#endif