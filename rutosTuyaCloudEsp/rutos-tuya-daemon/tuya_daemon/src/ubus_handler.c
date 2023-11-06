#include "ubus_handler.h"

struct ubus_context *g_ubus_context;

struct blob_buf b;

enum { DEVICE_INFO, __DEVICE_INFO_MAX };

static const struct blobmsg_policy device_info_policy[] = {
	[DEVICE_INFO] = { .name = "Devices", .type = BLOBMSG_TYPE_ARRAY },
};


/// @brief Callback that parses device information received from esp_controller using ubus
/// @param req 
/// @param type 
/// @param msg 
/// @return 0 on success error code otherwise
static int device_info_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct blob_attr *tb[__DEVICE_INFO_MAX];
	struct DeviceData *deviceData = (struct DeviceData *)req->priv;
	blobmsg_parse(device_info_policy, __DEVICE_INFO_MAX, tb, blob_data(msg), blob_len(msg));
	if (tb[__DEVICE_INFO_MAX]) {
		int len		    = blobmsg_check_array(tb[DEVICE_INFO], BLOBMSG_TYPE_STRING);
		deviceData->devices = (char **)malloc(len * sizeof(char *));

		if (deviceData->devices == NULL) {
			log_error("ERROR: Could not allocate memory for device data");
			return -1;
		}

		int i = 0;
		struct blob_attr *cur;
		size_t rem;

		blobmsg_for_each_attr (cur, tb[DEVICE_INFO], rem) {
			if (blobmsg_type(cur) != BLOBMSG_TYPE_STRING) {
				continue;
			}

			deviceData->devices[i++] = blobmsg_get_string(cur);
		}

		deviceData->len = len;
	}

	return 0;
}

enum { ESP_RESPONSE, __LED_ACTION_MAX };

static const struct blobmsg_policy led_action_policy[] = {
	[ESP_RESPONSE] = { .name = "ESP_response", .type = BLOBMSG_TYPE_STRING },
};

/// @brief Callback for parsing led action response
/// @param req 
/// @param type 
/// @param msg 
/// @return 0 on success error code otherwise
static int led_action_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct blob_attr *tb[__LED_ACTION_MAX];
	struct ResponseData *responseData = (struct ResponseData *)req->priv;
	blobmsg_parse(led_action_policy, __LED_ACTION_MAX, tb, blob_data(msg), blob_len(msg));
	if (tb[__LED_ACTION_MAX]) {
		responseData->response = blobmsg_get_string(tb[ESP_RESPONSE]);
	}else{
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	return 0;
}

/// @brief Call devices function from esp_controller using ubus
/// @param ctx 
/// @param deviceData 
/// @return 0 on success -1 otherwise
int get_device_info(struct ubus_context *ctx, struct DeviceData *deviceData)
{
	uint32_t id;
	if (ubus_lookup_id(ctx, "esp_controller", &id) ||
	    ubus_invoke(ctx, id, "devices", NULL, device_info_cb, deviceData, 3000)) {
		log_error("ERROR: cannot request device information from esp_controller \n");
		return -1;
	} else {
		return 0;
	}
}

/// @brief Calls led action from esp_controller using ubus
/// @param ctx Ubus context
/// @param command Command to call
/// @param port Port of the ESP
/// @param pin Pin of the ESP
/// @param response Pointer to a struct in which the response will be stored
/// @return 0 on success error code otherwise
int call_led_action(struct ubus_context *ctx, char *command, char *port, int pin, struct ResponseData *response)
{
	uint32_t id;

	int ret = 0;
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "port", port);
	blobmsg_add_u32(&b, "pin", pin);

	ret = (ubus_lookup_id(ctx, "esp_controller", &id) ||
	       ubus_invoke(ctx, id, command, b.head, led_action_cb, response,3500));

	if (ret) {
		log_error("ERROR: Could not invoke led action");
	}

	blob_buf_free(&b);
	return ret;
}