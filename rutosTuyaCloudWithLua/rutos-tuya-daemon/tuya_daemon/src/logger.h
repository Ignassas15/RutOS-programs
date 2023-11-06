#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <stdarg.h>

void init_logger();

void close_logger();

void log_info(const char *message);

void log_error(const char *message);

void log_finfo(const char *format, ...);

void log_ferror(const char *format, ...);



#endif