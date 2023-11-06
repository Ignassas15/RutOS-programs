--API for getting information about interfaces and bridges
require("ubus")

local FunctionService = require("api/FunctionService")

local BridgeService = FunctionService:new()

--GET /api/bridges/members
function BridgeService:GET_TYPE_members()
    os.execute("sleep 2")
	
    local bridgeInfo = getBridgesInformation()

	if bridgeInfo == nil then
		return self:ResponseError({})
	end

    return self:ResponseOK(bridgeInfo)
        
end


function getBridgesInformation()
	local interfaceInfo = getInterfaceInformation()
	local bridges = getBridges(interfaceInfo)
	local bridgeInfo = formatBridgeInfo(bridges)

	if bridgeInfo == nil then
		return nil
	end

	return bridgeInfo 
end

function getInterfaceInformation()
        local connection = ubus.connect()
        if not connection then
                        return nil
        end

        local interfaceInfo = connection:call("network.device", "status", {msg = ""})

        return interfaceInfo

end


function getBridges(interfaceInfo)
        local bridges = {}
        if interfaceInfo == nil then
                return nil
        end

        for k, v in pairs(interfaceInfo) do
                if v.type == "bridge" then
			bridges[k] = v
                end
        end

        return bridges
end

function formatBridgeInfo(bridges)
        if bridges == nil then
                return nil
        end

        local bridgeInfo = {}
        bridgeInfo.bridges = {}
        for k, v in pairs(bridges) do
                local bridge = {}
                bridge.name = k
                bridge["bridge-members"] = v["bridge-members"]
                table.insert(bridgeInfo.bridges, bridge)
        end


        return bridgeInfo

end

return BridgeService
