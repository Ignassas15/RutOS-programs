#include "ubus_handler.h"

struct blob_buf b;

const struct blobmsg_policy turn_on_policy[] = {
	[TURN_ON_PORT] = { .name = "port", .type = BLOBMSG_TYPE_STRING },
	[TURN_ON_PIN]  = { .name = "pin", .type = BLOBMSG_TYPE_INT32 }
};

const struct blobmsg_policy turn_off_policy[] = {
	[TURN_OFF_PORT] = { .name = "port", .type = BLOBMSG_TYPE_STRING },
	[TURN_OFF_PIN]	= { .name = "pin", .type = BLOBMSG_TYPE_INT32 }
};

const struct ubus_method controller_methods[] = { UBUS_METHOD_NOARG("devices", get_devices),
						  UBUS_METHOD("on", turn_on, turn_on_policy),
						  UBUS_METHOD("off", turn_off, turn_off_policy) };

struct ubus_object_type controller_object_type = UBUS_OBJECT_TYPE("esp_controller", controller_methods);

struct ubus_object controller_object = {
	.name	   = "esp_controller",
	.type	   = &controller_object_type,
	.methods   = controller_methods,
	.n_methods = ARRAY_SIZE(controller_methods),
};


/// @brief Get's the list of all connected esp devices that are supported 
/// @param ctx Ubus context
/// @param obj 
/// @param req 
/// @param method 
/// @param msg 
/// @return 0 on success error code otherwise
int get_devices(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		const char *method, struct blob_attr *msg)
{
	struct sp_port **port_list;

	int ret = get_device_list(&port_list);

	if (ret) {
		log_error("Couldn't get device list");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	blob_buf_init(&b, 0);

	void *cookie = blobmsg_open_array(&b, "Devices");

	int i;
	for (i = 0; port_list[i] != NULL; i++) {
		struct sp_port *port = port_list[i];

		enum sp_transport transport = sp_get_port_transport(port);

		if (transport == SP_TRANSPORT_USB) {
			char *port_name = sp_get_port_name(port);
			int usb_vid, usb_pid;
			sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);

			char line_buff[256];
			if (is_device_esp(usb_vid, usb_pid)) {
				snprintf(line_buff, sizeof(line_buff),
					 "{'port': '%s', 'usb_vid': %d, 'usb_pid': %d}", port_name,
					 usb_vid, usb_pid);

				blobmsg_add_string(&b, "device", line_buff);
			}
		}

		sp_free_port(port);
	}

	if (port_list != NULL) {
		free(port_list);
	}

	blobmsg_close_array(&b, cookie);
	ubus_send_reply(ctx, req, b.head);
	
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

/// @brief Handles turn on messages received trough ubus and tries to turn on led on esp 
/// @param ctx Ubus context
/// @param obj 
/// @param req 
/// @param method 
/// @param msg 
/// @return 0 on success error code otherwise
int turn_on(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
	    const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[__TURN_ON_MAX];

	blobmsg_parse(turn_on_policy, ARRAY_SIZE(turn_on_policy), tb, blob_data(msg), blob_len(msg));

	if (!tb[TURN_ON_PORT]) {
		log_error("Could not parse port");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (!tb[TURN_ON_PIN]) {
		log_error("Could not parse pin");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	char *port_name = blobmsg_get_string(tb[TURN_ON_PORT]);
	int pin		= blobmsg_get_u32(tb[TURN_ON_PIN]);

	char *resp_buf[256] = {};

	if (call_led_op(port_name, pin, LED_ON, resp_buf, sizeof(resp_buf))) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		blob_buf_init(&b, 0);
		blobmsg_add_string(&b, "ESP_response", resp_buf);
		ubus_send_reply(ctx, req, b.head);
		blob_buf_free(&b);
		return UBUS_STATUS_OK;
	}
}

/// @brief Handles turn off messages received trough ubus and tries to turn off led on esp 
/// @param ctx Ubus context
/// @param obj 
/// @param req 
/// @param method 
/// @param msg 
/// @return 0 on success error code otherwise
int turn_off(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
	     const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[__TURN_OFF_MAX];

	blobmsg_parse(turn_off_policy, ARRAY_SIZE(turn_off_policy), tb, blob_data(msg), blob_len(msg));

	if (!tb[TURN_OFF_PORT]) {
		log_error("Could not parse port");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (!tb[TURN_OFF_PIN]) {
		log_error("Could not parse pin");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	char *port_name = blobmsg_get_string(tb[TURN_OFF_PORT]);
	int pin		= blobmsg_get_u32(tb[TURN_OFF_PIN]);


	char *resp_buf[256] = {};

	if (call_led_op(port_name, pin, LED_OFF, resp_buf, sizeof(resp_buf))) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		blob_buf_init(&b, 0);
		blobmsg_add_string(&b, "ESP_response", resp_buf);
		ubus_send_reply(ctx, req, b.head);
		blob_buf_free(&b);
		return UBUS_STATUS_OK;
	}
}

/// @brief Function for subscribing to uloop
/// @param ctx Ubus context
/// @return 0 on success error code otherwise
int subscribe_to_uloop(struct ubus_context *ctx)
{
	ubus_add_uloop(ctx);
	return ubus_add_object(ctx, &controller_object);
}