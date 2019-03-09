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
    bool waitForPause = lua_isboolean(L, 1) ? lua_toboolean(L, 1) : true;
    bool res = DbgCmdExecDirect("sto");

    if (res && waitForPause)
        _plugin_waituntilpaused();

    lua_pushboolean(L, res);
    return 0;
}

static int lua_debugger_stepIn(lua_State *L)
{
    bool waitForPause = lua_isboolean(L, 1) ? lua_toboolean(L, 1) : true;
    bool res = DbgCmdExecDirect("sti");

    if (res && waitForPause)
        _plugin_waituntilpaused();

    lua_pushboolean(L, res);
    return 0;
}

static int lua_debugger_run(lua_State *L)
{
    bool waitForPause = lua_isboolean(L, 1) ? lua_toboolean(L, 1) : true;
    bool res = DbgCmdExecDirect("run");

    if (res && waitForPause)
        _plugin_waituntilpaused();

    lua_pushboolean(L, res);
    return 1;
}

static int lua_debugger_isRunning(lua_State *L)
{
    lua_pushboolean(L, DbgIsRunning());
    return 1;
}

static int lua_debugger_wait(lua_State *L)
{
    _plugin_waituntilpaused();
    return 0;
}

static int lua_debugger_switchThread(lua_State *L)
{
    lua_Integer threadId = lua_tointeger(L, 1);

    char cmd[128] = {};
    sprintf_s(cmd, "switchthread %X", (uint32_t)threadId);

    bool res = DbgCmdExecDirect(cmd);
    lua_pushboolean(L, res);

    return 1;
}

static const luaL_Reg lua_debugger[] =
{
    { "stepOver", lua_debugger_stepOver },
    { "stepIn", lua_debugger_stepIn },
    { "run", lua_debugger_run },
    { "wait", lua_debugger_wait },
    { "isRunning", lua_debugger_isRunning },
    { "switchThread", lua_debugger_switchThread },
    { nullptr, nullptr },
};

bool luaopen_debugger(lua_State *L)
{
    luaL_newlib(L, lua_debugger);
    lua_setglobal(L, "debugger");
    return true;
}
