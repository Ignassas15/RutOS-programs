#include "logger.h"
#include <stdlib.h>

/**
 * init_logger() - Initializes the log file, and logs when it is done
 */
void init_logger()
{
	openlog(NULL, LOG_PID, LOG_DAEMON);
	log_info("Opened log");
}

/**
 * close_logger() - Closes the logger
 */
void close_logger()
{
	log_info("Closing log");
	closelog();
}

/**
 * log_info() - Writes message to log file as an informational log
 * @param message: Message to log 
 */
void log_info(const char *message)
{
	syslog(LOG_INFO, "%s", message);
}

/**
 * log_info() - Writes message to log file as an error log
 * @param message: Message to log 
 */
void log_error(const char *message)
{
	syslog(LOG_ERR, "%s", message);
}