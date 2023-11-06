local ConfigService = require("api/ConfigService")

local BridgeConfigService = ConfigService:new({create = false})

local BridgeDeviceConfig = BridgeConfigService:section("network", "device")

BridgeDeviceConfig:make_primary()

BridgeDeviceConfig.default_options.id.maxlength = 20

    local opt_mtu = BridgeDeviceConfig:option("mtu")

    function opt_mtu:validate(value)
        return self.dt:range(value, 98, 65535)
    end

    --Check if selected section is really a bridge before setting value
    function opt_mtu:set(value)
        if self:table_get(self.config, self.sid, "type") == "bridge" then
            self:table_set(self.config, self.sid, "mtu", value)
        end
    end

    local opt_stp = BridgeDeviceConfig:option("stp")

    function opt_stp:validate(value)
        return self.dt:is_bool(value)
    end

    function opt_stp:set(value)
        if self:table_get(self.config, self.sid, "type") == "bridge" then
            self:table_set(self.config, self.sid, "stp", value)
        end
    end

    local opt_name = BridgeDeviceConfig:option("name")
    
    opt_name.maxlength = 32
    
    function opt_name:validate(value)
        if startsWith(value, "br-") then
            return true
        end
        return false
    end 

    function opt_name:set(value)
        if self:table_get(self.config, self.sid, "type") == "bridge" then
            self:table_set(self.config, self.sid, "name", value)
        end
    end

    local opt_ports = BridgeDeviceConfig:option("ports", { list = true })
		opt_ports.readonly = true

    function startsWith(str, prefix)
        return string.sub(str, 1, #prefix) == prefix
    end

return BridgeConfigService
