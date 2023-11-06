#include "utils.h"

/**
 * become_daemon() - Turns the program into daemon process
 * @return: Returns 0 if successfull
 */
int become_daemon()
{
	int maxfd, fd;

	switch (fork()) {
	case -1:
		log_error("Couldn't successfully fork");
		return -1;
		break;
	case 0:
		break;
	default:
		exit(0);
	}

	if (setsid() == -1) {
		log_error("Couldn't create a new session");
		return -1;
	}

	switch (fork()) {
	case -1:
		log_error("Couldn't successfully double fork");
		return -1;
		break;
	case 0:
		break;
	default:
		exit(0);
	}

	umask(0);
	chdir("/");

	maxfd = sysconf(_SC_OPEN_MAX);
	if (maxfd == -1) {
		maxfd = BD_MAX_CLOSE;
	}

	for (fd = 0; fd < maxfd; fd++) {
		close(fd);
	}

	close(STDIN_FILENO);
	fd = open("/dev/null", O_RDWR);
	if (fd != STDIN_FILENO)
		return -1;
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		return -2;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
		return -3;

	return 0;
}

/**
 * load_device_info() - Loads tuya iot device information parsed from arguments to variables 
 * @param arguments - Argument struct containing needed information
 * @param product_id - Pointer to product_id
 * @param device_id - Pointer to device id
 * @param device_secret - Pointer to device secret
 */
void load_device_info(struct arguments arguments, char *product_id, char *device_id, char *device_secret)
{
	strncpy(product_id, arguments.args[0], 32);
	strncpy(device_id, arguments.args[1], 32);
	strncpy(device_secret, arguments.args[2], 32);
}

/// @brief Replaces single quotes with double one's
/// @param string String in which to replace quotes
void replace_quotes(char *string)
{
	for (int i = 0; i < strlen(string); i++) {
		if (string[i] == '\'') {
			string[i] = '\"';
		}
	}
}

int is_lua_file(char *filename){
	return ends_with(filename, ".lua");
}

int ends_with(char *str, char *end){
	int str_len = strlen(str);
	int end_len = strlen(end);

	if(end_len > str_len){
		return 0;
	}

	const char *str_end = str + (str_len - end_len);

    return strcmp(str_end, end) == 0;
}
