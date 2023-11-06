#include <stdio.h>
#include <string.h>
#include "libserialport.h"
#include "libubus.h"
#include "serial_handler.h"
#include "ubus_handler.h"
#include "logger.h"

int main()
{
	init_logger();
	struct ubus_context *ubus_ctx;
	int ret = 0;
	uloop_init();

	ubus_ctx = ubus_connect(NULL);

	if (!ubus_ctx) {
		log_error("Failed to connect to ubus \n");
		return 1;
	}

	if (subscribe_to_uloop(ubus_ctx)) {
		log_error("ERROR: Failed to subscribe to uloop");
		ret = 1;
		goto CLEANUP;
	}

	uloop_run();

CLEANUP:
	if (ubus_ctx != NULL) {
		ubus_free(ubus_ctx);
	}

	uloop_done();
	close_logger();

	return ret;
}
