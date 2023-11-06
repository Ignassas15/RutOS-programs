#include "serial_handler.h"

/**
 * get_device_list() - Get list of connected device ports and returns it trough port_list parameter
 * @port_list: Triple pointer of struct sp_port that will store the array of connected devices
 * @return 0 on success, negative error code otherwise
 */
int get_device_list(struct sp_port ***port_list)
{
	enum sp_return result = sp_list_ports(port_list);

	if (result != SP_OK) {
		log_error("Failed to find ports");
		return result;
	}

	return 0;
}

/**
 * call_led_op() - Sends a message to the device trough serial and tells it to turn on or off a particular led
 * @port_name: Name of port device is connected to
 * @pin: Number of pin to turn on
 * @op: Operation to perform ON or OFF
 * @return: 0 on success error code otherwise
 */
int call_led_op(char *port_name, int pin, enum led_op op, char* resp_buf, int resp_buf_size)
{
	struct sp_port *port;

	if (open_port_by_name(&port, port_name)) {
		log_error("ERROR: Couldn't open port by name");
		return 1;
	}

	struct sp_port *tx_port = port;
	struct sp_port *rx_port = port;

	char *data[256];

	char operation[5];

	switch (op) {
	case LED_ON:
		strcpy(operation, "on");
		break;
	case LED_OFF:
		strcpy(operation, "off");
		break;
	default:
		strcpy(operation, "on");
		break;
	}

	snprintf(data, sizeof(data), "{\"action\": \"%s\", \"pin\": %d}", operation, pin);

	int size	     = strlen(data);
	unsigned int timeout = 3000;

	int result;

	result = check(sp_blocking_write(tx_port, data, size, timeout));

	result = check(sp_blocking_read(rx_port, resp_buf, resp_buf_size, timeout));
	
	return close_port(port);
}

/**
 * check() - Checks and interprets sp_return codes
 * @result: sp_return code
 * @return: The result code passed in after printing a message
 */
int check(enum sp_return result)
{
	char *error_message;

	switch (result) {
	case SP_ERR_ARG:
		log_error("Error: Invalid argument.\n");
		return result;
	case SP_ERR_FAIL:
		error_message = sp_last_error_message();
		log_error(error_message);
		sp_free_error_message(error_message);
		return result;
	case SP_ERR_SUPP:
		log_error("Error: Not supported.\n");
		return result;
	case SP_ERR_MEM:
		log_error("Error: Couldn't allocate memory.\n");
		return result;
	case SP_OK:
	default:
		return result;
	}
}

/**
 * close_port() - Closes specified port and frees memory
 * @port: Port to close
 * @return: 0 on success 1 otherwise
 */
int close_port(struct sp_port *port)
{
	if (port == NULL) {
		return 1;
	}

	if (check(sp_close(port))) {
		log_error("ERROR: Couldn't close port \n");
		return 1;
	}
	sp_free_port(port);

	return 0;
}

/**
 * open_port_by_name() - Opens specified port from it's name, and gives it back trough port argument
 * @port: Pointer to where port will be stored
 * @port_name: Name of the port to open
 * @return: 0 on success 1 otherwise
 */
int open_port_by_name(struct sp_port **port, char *port_name)
{
	if (check(sp_get_port_by_name(port_name, port))) {
		log_error("ERROR: Couldn't get port by name \n");
		return 1;
	}

	if (check(sp_open(*port, SP_MODE_READ_WRITE))) {
		log_error("ERROR: couldn't open port \n");
		return 1;
	}
	if (check(sp_set_baudrate(*port, 9600))) {
		log_error("ERROR: Couldn't set baudrate \n");
		return 1;
	}

	if (check(sp_set_bits(*port, 8))) {
		log_error("ERROR: Couldn't set data bit size \n");
		return 1;
	}

	if (check(sp_set_parity(*port, SP_PARITY_NONE))) {
		log_error("ERROR: Couldn't set parity bits \n");
		return 1;
	}

	if (check(sp_set_stopbits(*port, 1))) {
		log_error("ERROR: Couldn't set stop bit size \n");
		return 1;
	}

	if (check(sp_set_flowcontrol(*port, SP_FLOWCONTROL_NONE))) {
		log_error("ERROR: Couldn't set flowcontrol \n");
		return 1;
	}

	return 0;
}

/**
 * is_device_esp() - Checks wether the device is esp
 * @vid: Vendor id of the device
 * @pid: Product id of the device
 * @return: 1 if true 0 otherwise
 */
int is_device_esp(int vid, int pid)
{
	return ((vid == ESP_USBVID) && (pid == ESP_USBPID));
}