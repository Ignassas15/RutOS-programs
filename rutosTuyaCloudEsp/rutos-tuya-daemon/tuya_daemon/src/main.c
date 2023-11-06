#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "ubus_handler.h"
#include "parser.h"
#include "logger.h"
#include "utils.h"
#include "tuya_handler.h"
#include "cJSON.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "libubus.h"

volatile sig_atomic_t EXIT_INTERRUPT = 0;

void signal_handler(int signum);

tuya_mqtt_context_t client_instance;

int main(int argc, char **argv)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	const char product_id[32];
	const char device_id[32];
	const char device_secret[32];

	int ret = 0;
	init_logger();

	struct arguments arguments;
	init_arguments(&arguments);
	if (parse_arguments(argc, argv, &arguments)) {
		printf("Could not parse arguments try using --help");
		return 1;
	}

	load_device_info(arguments, product_id, device_id, device_secret);
	if (arguments.daemon) {
		ret = become_daemon();
		if (ret != 0) {
			log_error("Couldn't daemonize exiting");
			return 1;
		}
	}

	
	g_ubus_context = ubus_connect(NULL);
	if (!g_ubus_context) {
		log_error("Couldn't connect to UBUS exiting");
		return 1;
	}

	tuya_mqtt_context_t *client = &client_instance;
	if (initialise_tuya_mqtt(client, device_id, device_secret)) {
		log_error("Could not initialise tuya mqtt exiting");
		goto CLEANUP;
	}

	for (;;) {
		if (EXIT_INTERRUPT == 1) {
			tuya_mqtt_disconnect(client);
			break;
		}
		tuya_mqtt_loop(client);
		
	}

CLEANUP:
	close_logger();
	tuya_mqtt_deinit(client);
UBUS_CLEANUP:
	ubus_free(g_ubus_context);

	return ret;
}

/**
 * signal_handler() - Function for handling interrupt signas
 * @param signum: signal number
 */
void signal_handler(int signum)
{
	EXIT_INTERRUPT = 1;
}
