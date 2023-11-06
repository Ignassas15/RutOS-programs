#ifndef TUYA_HANDLER_H
#define TUYA_HANDLER_H

#include "cJSON.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "logger.h"
#include "utils.h"


int initialise_tuya_mqtt(tuya_mqtt_context_t *client, char *device_id, char *device_secret);
void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_messages(tuya_mqtt_context_t* context, void *user_data, const tuyalink_message_t* message);
void on_disconnect();

#endif
