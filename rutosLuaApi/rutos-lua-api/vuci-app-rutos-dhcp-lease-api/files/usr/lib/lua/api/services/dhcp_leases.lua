--API for getting information about dhcp devices
require "ubus"

local FunctionService = require("api/FunctionService")


local DHCPService = FunctionService:new()

function DHCPService:GET_TYPE_ipv4leases()
    return self:ResponseOK(getipv4Leases())
    
end

function DHCPService:GET_TYPE_ipv6leases()
    return self:ResponseOK(formatipv6Leases(getipv6Leases()))
end

--Gets and returns ipv6leases from ubus dhcp ipv6leases
function getipv6Leases()
    local connection = ubus.connect()

    if not connection then
            return nil
    end

    local leases = connection:call("dhcp", "ipv6leases", {msg = ""})
    return leases
end

--Returns formatted table of ipv6leases
function formatipv6Leases(ipv6leases)
    formattedLeases = {}
    formattedLeases.leases = {}

    if ipv6leases == nil or type(ipv6leases) ~= "table" then
            return formattedLeases
    end

    for interface, interfaceTable in pairs(ipv6leases.device) do
            for key, leaseInfo in pairs(interfaceTable.leases) do
                    local lease = leaseInfo
                    if not (next(lease) == nil) then
                            lease.interface = interface
                            table.insert(formattedLeases.leases, lease)
                    end
            end
    end

    return formattedLeases
end

--Returns table of ipv4 leases
function getipv4Leases()
    local ipv4Leases = {}
    ipv4Leases.leases = {}

    local leaseFile = io.open("/tmp/dhcp.leases", "r")

    if leaseFile == nil then
            return ipv4Leases
    end

    for line in leaseFile:lines() do
            local lease = parseLease(line)
            if lease then
                table.insert(ipv4Leases.leases, lease)
            end
    end

    return ipv4Leases
end


--Parses line of information about lease from /tmp/dhcp.leases file
--Returns leaseInfo table if lease looks valid or nil if not
function parseLease(line)
    local values = {}
    local count = 0

    for value in string.gmatch(line, "%S+") do
            table.insert(values, value)

            count = count + 1
            if count >= 4 then --only get first 4 values timestamp, mac, ip and hostname
                    break
            end
    end

    local leaseInfo = {}

    leaseInfo.timestamp = values[1]
    leaseInfo.macaddr = values[2]
    leaseInfo.ip = values[3]
    leaseInfo.hostname = values[4]

    if validateipv4Lease(leaseInfo) then
        return leaseInfo
    end

    return nil
end

function is_valid_mac(mac)
    if string.len(mac) ~= 17 then
        return false
    end

    local pattern = "%x+:%x+:%x+:%x+:%x+:%x+" --Pattern for validating mac
    if string.match(mac, pattern) == mac then
        return true
    else
        return false
    end
end

local function is_valid_ipv4(ip)
    local parts = {ip:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")}
    if #parts == 4 then
        for _, part in ipairs(parts) do
            local num = tonumber(part)
            if num == nil or num < 0 or num > 255 then
                return false
            end
        end
        return true
    else
        return false
    end
end


function validateipv4Lease(lease)
    return (tonumber(lease.timestamp) ~= nil and is_valid_mac(lease.macaddr) and is_valid_ipv4(lease.ip) and lease.hostname ~= nil)
end



return DHCPService