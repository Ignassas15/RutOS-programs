#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libserialport.h"
#include "logger.h"

#define ESP_USBVID 4292
#define ESP_USBPID 60000

/// @brief Enum that stores supported operations for leds LED_ON and LED_OFF
enum led_op { LED_ON, LED_OFF };

int get_device_list(struct sp_port ***port_list);
int call_led_op(char *port_name, int pin, enum led_op op, char* resp_buf, int resp_buf_size);
int check(enum sp_return result);
int open_port_by_name(struct sp_port **port, char *port_name);
int close_port(struct sp_port *port);
int is_device_esp(int vid, int pid);

#endif