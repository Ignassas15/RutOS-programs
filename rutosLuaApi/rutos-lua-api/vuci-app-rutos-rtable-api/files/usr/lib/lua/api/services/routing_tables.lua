--API for getting information about active routing tables and their routes

local FunctionService = require("api/FunctionService")

local RTableService = FunctionService:new()

function RTableService:GET_TYPE_tables()
    return self:ResponseOK(formatAvailableRTables())
end

function RTableService:GET_TYPE_mainroutes()
    return self:ResponseOK(getRoutingTableEntries())
end

function getAvailableRoutingTables()
    local routingTables = {}

    local mappingFile = io.open("/etc/iproute2/rt_tables", "r")

    if mappingFile == nil then
            return routingTables
    end

    for line in mappingFile:lines() do
            local k, v = parseMappingFileEntry(line)

            if k ~= nil and v ~= nil then
                    routingTables[k] = v
            end
    end

    return routingTables

end

function parseMappingFileEntry(entry)
    local k,v

    if entry == nil then
        return nil, nil
    end


    local entryStart, _ = string.find(entry, "#")--Find first occurence of a comment

    --Take a substring from start to comment 
    if entryStart then
        entry = string.sub(entry, 1, entryStart-1)
    end

    local values = {}
    for value in string.gmatch(entry, "%S+") do
        table.insert(values,value)
    end

    k = values[1]
    v = values[2]

    return k,v

end

function formatAvailableRTables()

    local formattedInfo = {}
    formattedInfo.entries = {}

    local routingTables = getAvailableRoutingTables()

    for k,v in pairs(routingTables) do
            local entry = {}
            entry.id = k
            entry.name = v
            table.insert(formattedInfo.entries, entry)
    end

    return formattedInfo
end


--Returns data about main routing table route entries in a lua table
function getRoutingTableEntries()
    local routesFile = io.popen("ip route show")
    local routes = {}
    routes.entries = {}

    
    if routesFile == nil then
            return routes
    end

    for line in routesFile:lines() do
            local route = parseRoutingTableEntry(line)
            if route ~= nil then
                table.insert(routes.entries, route)
            end
    end

    return routes

end

--Parses routing table entry into a lua table, returns nil if unable to parse
function parseRoutingTableEntry(entry)
    local route = {}

    if entry == nil then
        return nil
    end

    local values = {}
    for value in string.gmatch(entry, "%S+") do
        table.insert(values, value)
    end

   
    if (#values-1) % 2 == 0 then
        route.type = "unicast"
        route.destination = values[1]
        for i = 2, (#values)-1, 2 do
            route[values[i]] = values[i+1]
        end
    else
        route.type = values[1]
        route.destination = values[2]
        for i = 3, (#values)-1, 2 do
            route[values[i]] = values[i+1]
        end
    end

    return route

end

return RTableService