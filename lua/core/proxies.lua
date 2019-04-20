proxies = {}

local _INSTALLED_PROXIES = {}

function proxies.install(proxy)
	table.insert(_INSTALLED_PROXIES, proxy)
end

function proxies.getInstalled()
	return _INSTALLED_PROXIES
end

setmetatable(_G, { 
	__index = function(t, k)
		for _,proxy in pairs(_INSTALLED_PROXIES) do
			if proxy.__index ~= nil then
				local r = proxy.__index(t, k)
				if r ~= nil then
					return r
				end
			end
		end
		return rawget(t, k) 
	end, 
	__newindex = function(t, k, v)
		for _,proxy in pairs(_INSTALLED_PROXIES) do
			if proxy.__newindex ~= nil then
				local r = proxy.__newindex(t, k, v)
				if r ~= nil then
					return
				end
			end
		end
		return rawset(t, k, v) 
	end, 
})