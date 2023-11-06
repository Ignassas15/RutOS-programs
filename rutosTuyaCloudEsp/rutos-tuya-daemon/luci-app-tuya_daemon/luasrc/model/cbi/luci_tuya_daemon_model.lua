
map = Map("tuya_daemon", "Tuya IoT")
section = map:section(NamedSection, "config_sct", "daemon_config", "Daemon configuration section")
flag = section:option(Flag, "enable", "Enable", "Enable Tuya IoT daemon")
product_id = section:option(Value, "product_id", "Product Id")
product_id.maxlength = 32
product_id.optional = false
device_id = section:option(Value, "device_id", "Device Id")
device_id.maxlength = 32
device_id.optional = false
device_secret = section:option(Value, "device_secret", "Device secret")
device_secret.maxlength = 32
device_secret.optional = false

return map