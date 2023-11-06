#ifndef UBUS_HANDLER_H
#define UBUS_HANDLER_H
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "logger.h"


extern struct ubus_context *g_ubus_context; //Global variable for ubus_context


struct DeviceData{
	char **devices;
	int len;
};

struct ResponseData{
	char *response;
};


int get_device_info(struct ubus_context *ctx, struct DeviceData *deviceData);
int call_led_action(struct ubus_context *ctx, char *command, char *port, int pin, struct ResponseData *response);
static int device_info_cb(struct ubus_request *req, int type, struct blob_attr *msg);
static int led_action_cb(struct ubus_request *req, int type, struct blob_attr *msg);

#endif