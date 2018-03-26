#include "lua_context.hpp"
#include "plugin.h"

// Global instance.
LuaContext* g_pLuaContext = nullptr;

LuaContext::LuaContext()
    : _L(nullptr)
{
    _L = luaL_newstate();
}

LuaContext::~LuaContext()
{
    if (_L)
    {
        lua_close(_L);
        _L = nullptr;
    }
}

void LuaContext::update()
{

}

extern bool luaopen_registers(lua_State *L);
extern bool luaopen_x64dbgprint(lua_State *L);

bool LuaContext::registerLibs()
{
    if(!_L)
        return false;

    luaL_openlibs(_L);

    // x64dbg specific.
    luaopen_registers(_L);
    luaopen_x64dbgprint(_L);

    return true;
}

inline bool processError(lua_State *L, int res)
{
    if (res != 0)
    {
        const char *pszError = lua_tostring(L, -1);
        if (pszError)
        {
            _plugin_logprintf("Lua Error: %s\n", pszError);
        }
        return false;
    }
    return true;
}

bool LuaContext::runString(const char *lua)
{
    if(!_L)
        return false;

    int res = luaL_loadstring(_L, lua);
    if (!processError(_L, res))
    {
        return false;
    }

    res = lua_pcall(_L, 0, LUA_MULTRET, 0);
    if (!processError(_L, res))
    {
        return false;
    }

    return true;
}

bool LuaContext::executeAutorunScripts()
{
    return true;
}
