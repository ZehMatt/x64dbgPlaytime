#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>
#include <unordered_map>
#include <stdint.h>
#include <string>
#include <functional>
#include <algorithm>

static int lua_debugger_stepOver(lua_State *L)
{
    DbgCmdExec("step");

    g_pLuaContext->setShouldResumeScript();
    lua_yield(L, 0);

    return 0;
}

static int lua_debugger_stepIn(lua_State *L)
{
    DbgCmdExec("sti");

    g_pLuaContext->setShouldResumeScript();
    lua_yield(L, 0);

    return 0;
}

static int lua_debugger_run(lua_State *L)
{
    DbgCmdExec("run");

    g_pLuaContext->setShouldResumeScript();
    lua_yield(L, 0);

    return 0;
}

static const luaL_Reg lua_debugger[] =
{
    { "stepOver", lua_debugger_stepOver },
    { "stepIn", lua_debugger_stepIn },
    { "run", lua_debugger_run },
    { nullptr, nullptr },
};

bool luaopen_debugger(lua_State *L)
{
    luaL_newlib(L, lua_debugger);
    lua_setglobal(L, "debugger");
    return true;
}