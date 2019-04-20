-- Create a proxy on _G __index/__newindex to get direct access to registers without having
-- to use registers lib directly. So instead of having registers.write("rax", 1) we can say rax = 1
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

local function IS_REGISTER(k) 
	return register_names[string.lower(k)] == true 
end

proxies.install({
	__index = function(t, k) 
		if IS_DEBUGGING and IS_REGISTER(k) then
			return registers.read(k) 
		end
		return nil
	end, 
	__newindex = function(t, k, v)
		if IS_DEBUGGING and IS_REGISTER(k) then
			registers.write(k, v)
			return true
		end
		return nil
	end, 
})