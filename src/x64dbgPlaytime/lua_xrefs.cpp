#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>

static int lua_xrefs_analyze(lua_State *L)
{
    bool block = lua_isboolean(L, 1) ? lua_toboolean(L, 1) : false;
    bool res;

    if(block)
        res = DbgCmdExecDirect("analx");
    else
        res = DbgCmdExec("analx");

    lua_pushboolean(L, res);
    return 1;
}

static int lua_xrefs_get(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);
    bool extended = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : false;

    XREF_INFO info {};

    lua_newtable(L);

    if (DbgXrefGet(addr, &info))
    {
        for (duint i = 0; i < info.refcount; i++)
        {
            lua_pushinteger(L, i + 1);
            if (extended)
            {
                lua_newtable(L);
                lua_pushstring(L, "address");
                lua_pushinteger(L, info.references[i].addr);
                lua_rawset(L, -3);

                lua_pushstring(L, "type");
                lua_pushinteger(L, info.references[i].type);
                lua_rawset(L, -3);
            }
            else
            {
                lua_pushinteger(L, info.references[i].addr);
            }
            lua_rawset(L, -3);
        }
    }

    return 1;
}

static int lua_xrefs_add(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);
    duint from = lua_tointeger(L, 2);

    bool res = DbgXrefAdd(addr, from);
    lua_pushboolean(L, res);

    return 1;
}

static int lua_xrefs_remove(lua_State *L)
{
    duint addr = lua_tointeger(L, 1);

    bool res = DbgXrefDelAll(addr);
    lua_pushboolean(L, res);

    return 1;
}

static const luaL_Reg lua_xrefs[] =
{
    { "analyze", lua_xrefs_analyze },
    { "get", lua_xrefs_get },
    { "add", lua_xrefs_add },
    { "remove", lua_xrefs_remove },
    { nullptr, nullptr },
};

bool luaopen_xrefs(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_xrefs);
    lua_setglobal(L, "xrefs");

    return true;
}
