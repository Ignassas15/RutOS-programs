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
        //log_info("Received action message from Tuya IoT cloud");
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

/// @brief Sends messages with data received from each lua script to cloud
/// @param client Tuya mqtt client
void send_messages_to_tuya(tuya_mqtt_context_t *client){

    int script_count = get_valid_script_count();

    for (int i = 0; i < script_count; i++) {
			char data_buffer[256];
			char message_buffer[512];
			call_script_getdata(i, data_buffer, sizeof(data_buffer));
			snprintf(message_buffer, sizeof(message_buffer), "{\"data\":%s}", data_buffer);
			tuyalink_thing_property_report(client, client->config.device_id, message_buffer);
	}

}

void handle_incoming_actions(tuya_mqtt_context_t* context, const tuyalink_message_t *message){
    cJSON *msg_json = cJSON_Parse(message->data_string);

    if(msg_json == NULL){
        log_error("Error: Could not parse action message JSON");
    }

    cJSON *action_code_json = cJSON_GetObjectItem(msg_json, "actionCode");

    if(action_code_json == NULL || !cJSON_IsString(action_code_json)){
        log_error("Error: Could not parse action code JSON");
        goto CLEANUP;
    }

    char *action_code[256];
    strncpy(action_code,action_code_json->valuestring, sizeof(action_code));
    
    char* script_name_token= strtok(action_code, "_");
    char* function_name = strtok(NULL, "_");

    char* script_name[256];


    if(script_name_token == NULL || function_name == NULL){
        log_error("Error: could not parse script or function name from action");
        goto CLEANUP;
    }

    strncpy(script_name, script_name_token, sizeof(script_name));
    strncat(script_name, ".lua", sizeof(script_name));

    if(!is_lua_file(script_name)){
        log_error("Error: Action lua file does not have .lua extension");
        goto CLEANUP;
    }

    char return_buff[256];
    if(call_action_script(script_name, function_name, msg_json, return_buff, sizeof(return_buff))){
        log_error("Error: Something went wrong when calling action script");
        
    }else{
        tuyalink_thing_property_report(context, context->config.device_id, return_buff);
    }

    CLEANUP:
        if(msg_json != NULL){
            cJSON_Delete(msg_json);
        }
}
