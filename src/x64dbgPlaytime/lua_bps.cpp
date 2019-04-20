#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>

static int lua_bps_add(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);

    bool res = Script::Debug::SetBreakpoint(addr);
    lua_pushboolean(L, res);

    return 1;
}

static int lua_bps_remove(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);

    bool res = Script::Debug::DeleteBreakpoint(addr);
    lua_pushboolean(L, res);

    return 1;
}

static int lua_bps_disable(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);

    bool res = Script::Debug::DisableBreakpoint(addr);
    lua_pushboolean(L, res);

    return 1;
}

static const luaL_Reg lua_bps[] =
{
    { "add", lua_bps_add },
    { "remove", lua_bps_remove },
    { "disable", lua_bps_disable },
    { nullptr, nullptr },
};

bool luaopen_bps(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_bps);
    lua_setglobal(L, "bps");

    return true;
}
