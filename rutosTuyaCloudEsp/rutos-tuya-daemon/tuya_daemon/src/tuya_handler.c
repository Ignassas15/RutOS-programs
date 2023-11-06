#include "tuya_handler.h"

const char cacert[] = {\
"-----BEGIN CERTIFICATE-----\n"\
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n"\
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n"\
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n"\
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\n"\
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n"\
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\n"\
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\n"\
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\n"\
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\n"\
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\n"\
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\n"\
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\n"\
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\n"\
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\n"\
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\n"\
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\n"\
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\n"\
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\n"\
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\n"\
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\n"\
"4uJEvlz36hz1\n"\
"-----END CERTIFICATE-----\n"};



/// @brief Initialises connection to tuya cloud
/// @param client 
/// @param device_id ID of the device to connect to
/// @param device_secret Secret of the device to connect to
/// @return 0 on success error code otherwise
int initialise_tuya_mqtt(tuya_mqtt_context_t *client, char *device_id, char *device_secret){
    int ret = 0;
     ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t){
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = cacert,
        .cacert_len = sizeof(cacert),
        .device_id = device_id,
        .device_secret = device_secret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });

    if(ret){
        return ret;
    }

    return tuya_mqtt_connect(client);
    

}

/**
 * on_connected() - Currently logs that program has successfully connected to tuya cloud
 */
void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
    log_info("Connected to Tuya IoT cloud");
}

/**
 * on_messages() - Logs that program received messages from tuya cloud and handles them if they are actions
 */
void on_messages(tuya_mqtt_context_t* context, void *user_data, const tuyalink_message_t* message){
    
    switch (message->type) {
    case THING_TYPE_ACTION_EXECUTE:
        log_info("Received action message from Tuya IoT cloud");
        handle_incoming_actions(context, message);
        break;
    default:
        break;
    }
}

/**
 * on_disconnect() - Currently logs that program has disconnected from tuya cloud
 */
void on_disconnect(){
    log_info("Disconnected from Tuya IoT cloud");
}


/// @brief Parses incoming action messages and calls appropriate functions
/// @param context
/// @param msg 
void handle_incoming_actions(tuya_mqtt_context_t* context, const tuyalink_message_t *msg){
    cJSON *msg_json = cJSON_Parse(msg->data_string);
    if(msg_json == NULL){
        log_error("ERROR: Could not parse action message JSON");
        return;
    }

    cJSON *action_code_json = cJSON_GetObjectItem(msg_json, "actionCode");

    if(action_code_json == NULL || !cJSON_IsString(action_code_json)){
        log_error("ERROR: Could not parse action code JSON");
        goto CLEANUP;
    }

    char *action_code = action_code_json->valuestring;

    if(strcmp(action_code, "GetDevices") == 0){
        get_devices_action(context);
    }else if(strcmp(action_code, "LedAction") == 0){
        led_action(context, msg_json);
    }else{
        tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Invalid action or unable to parse\"}");
    }

    CLEANUP:
        if(msg_json != NULL){
            cJSON_Delete(msg_json);
        }

}

/// @brief Parses parameters of led action
/// @param params JSON that contains inputParams
/// @param command Pointer for returning the parsed command
/// @param pin Pointer for returning the parsed pin
/// @param port Pointer for returning the parsed port
/// @return 0 on success 1 otherwise
int parse_led_action_params(cJSON *params, char *command, int *pin, char *port){

    if(params == NULL){
        return 1;
    }

    cJSON *command_json = cJSON_GetObjectItem(params, "command");

    if(command_json == NULL || !cJSON_IsString(command_json)){
        log_error("ERROR: Could not parse command");
        return 1;
    }

    strncpy(command,command_json->valuestring, sizeof(command));


    cJSON *pin_json = cJSON_GetObjectItem(params, "pin");

    if(pin_json == NULL || !cJSON_IsNumber(pin_json)){
        log_error("ERROR: Could not parse pin");
        return 1;
    }

    *pin = pin_json->valueint;

    cJSON *port_json = cJSON_GetObjectItem(params, "port");

    if(port_json == NULL || !cJSON_IsString(port_json)){
        log_error("ERROR: Could not parse port");
        return 1;
    }

    strncpy(port, port_json->valuestring, 32);

    return 0;

}

/// @brief Handles the getDevices action
/// @param context 
void get_devices_action(tuya_mqtt_context_t *context){
    char device_data[512];
        if(format_device_message(g_ubus_context, device_data, sizeof(device_data))){
            tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Could not get device list\"}");
        }else{
            tuyalink_thing_property_report(context, context->config.device_id, device_data);
        }
}

/// @brief Handles led action
/// @param context 
/// @param action_message Action message JSON
void led_action(tuya_mqtt_context_t *context, cJSON *action_message){
    cJSON *param_json = cJSON_GetObjectItem(action_message,"inputParams");

    if(param_json == NULL){
        tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Could not parse param JSON\"}");
        return;
    }

    char command[5];
    int pin;
    char port[32];

    
    if(parse_led_action_params(param_json, command, &pin, port)){
        tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Could not parse params\"}");
        return;
    }

    int ret = 0;

    struct ResponseData response = {0};
    ret = call_led_action(g_ubus_context, command, port, pin, &response);

    if(ret){
        tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Could not send led action\"}");
        return;
    }

    if(response.response == NULL){
        tuyalink_thing_property_report(context, context->config.device_id, "{\"Error\":\"Could not get response from ESP\"}");
        return;
    }

    tuyalink_thing_property_report(context, context->config.device_id, response.response);

}
