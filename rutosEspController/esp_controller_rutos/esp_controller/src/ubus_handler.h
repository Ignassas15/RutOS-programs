#ifndef UBUS_HANDLER_H
#define UBUS_HANDLER_H
#include <libubox/blobmsg_json.h>
#include "json-c/json.h"
#include "libserialport.h"
#include "libubus.h"
#include "serial_handler.h"
#include "logger.h"


int get_devices(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

enum {
    TURN_ON_PORT,
    TURN_ON_PIN,
    __TURN_ON_MAX
};


int turn_on(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

enum {
    TURN_OFF_PORT,
    TURN_OFF_PIN,
    __TURN_OFF_MAX
};


int turn_off(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);



int subscribe_to_uloop(struct ubus_context *ctx);



#endif