local register_names = 
{ 
	["rflags"] = true,
	["eflags"] = true,
	["rax"] = true, 
	["eax"] = true, 
	["ax"] = true, 
	["ah"] = true, 
	["al"] = true,
	["rbx"] = true, 
	["ebx"] = true, 
	["bx"] = true, 
	["bh"] = true, 
	["bl"] = true, 
	["rcx"] = true, 
	["ecx"] = true, 
	["cx"] = true, 
	["ch"] = true, 
	["cl"] = true, 
	["rdx"] = true, 
	["edx"] = true, 
	["dx"] = true, 
	["dh"] = true, 
	["dl"] = true, 
	["rdi"] = true, 
	["edi"] = true, 
	["di"] = true, 
	["dil"] = true, 
	["rsi"] = true, 
	["esi"] = true, 
	["si"] = true, 
	["sil"] = true, 
	["rbp"] = true, 
	["ebp"] = true, 
	["bp"] = true, 
	["bpl"] = true, 
	["rsp"] = true, 
	["esp"] = true, 
	["sp"] = true, 
	["spl"] = true, 
	["rip"] = true, 
	["eip"] = true, 
	["r8"] = true, 
	["r8d"] = true, 
	["r8w"] = true, 
	["r8b"] = true, 
} 

local function is_register(k) 
	return register_names[string.lower(k)] == true 
end

setmetatable(_G, { 
	__index = function(t, k) 
		if IS_DEBUGGING and is_register(k) then
			local registers = rawget(t, "registers")
			local registers_read = rawget(registers, "read")
			return registers_read(k) 
		end 
		return rawget(t, k) 
	end, 
	__newindex = function(t, k, v) 
		if IS_DEBUGGING and is_register(k) then 
			local registers = rawget(t, "registers")
			local registers_write = rawget(registers, "write")
			return registers_write(k, v) 
		end 
		return rawset(t, k, v) 
	end, 
})

function printtable(node)
    -- to make output beautiful
    local function tab(amt)
        local str = ""
        for i=1,amt do
            str = str .. "  "
        end
        return str
    end

    local cache, stack, output = {},{},{}
    local depth = 1
    local output_str = "{\n"

    while true do
        local size = 0
        for k,v in pairs(node) do
            size = size + 1
        end

        local cur_index = 1
        for k,v in pairs(node) do
            if (cache[node] == nil) or (cur_index >= cache[node]) then

                if (string.find(output_str,"}",output_str:len())) then
                    output_str = output_str .. ",\n"
                elseif not (string.find(output_str,"\n",output_str:len())) then
                    output_str = output_str .. "\n"
                end

                -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
                table.insert(output,output_str)
                output_str = ""

                local key
                if (type(k) == "number" or type(k) == "boolean") then
                    key = "["..tostring(k).."]"
                else
                    key = "['"..tostring(k).."']"
                end

                if (type(v) == "number" or type(v) == "boolean") then
                    output_str = output_str .. tab(depth) .. key .. " = "..tostring(v)
                elseif (type(v) == "table") then
                    output_str = output_str .. tab(depth) .. key .. " = {\n"
                    table.insert(stack,node)
                    table.insert(stack,v)
                    cache[node] = cur_index+1
                    break
                else
                    output_str = output_str .. tab(depth) .. key .. " = '"..tostring(v).."'"
                end

                if (cur_index == size) then
                    output_str = output_str .. "\n" .. tab(depth-1) .. "}"
                else
                    output_str = output_str .. ","
                end
            else
                -- close the table
                if (cur_index == size) then
                    output_str = output_str .. "\n" .. tab(depth-1) .. "}"
                end
            end

            cur_index = cur_index + 1
        end

        if (size == 0) then
            output_str = output_str .. "\n" .. tab(depth-1) .. "}"
        end

        if (#stack > 0) then
            node = stack[#stack]
            stack[#stack] = nil
            depth = cache[node] == nil and depth + 1 or depth - 1
        else
            break
        end
    end

    -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
    table.insert(output,output_str)
    output_str = table.concat(output)

    print(output_str)
end