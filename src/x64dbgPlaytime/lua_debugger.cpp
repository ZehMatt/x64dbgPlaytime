#include "lua_context.hpp"
#include "lauxlib.h"
#include "lua_table_utils.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "utils.h"

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

static int lua_debugger_start(lua_State *L)
{
    if (lua_istable(L, 1) == false)
    {
        luaL_error(L, "Table expected for paramter 1");
    }

    lua_pushvalue(L, 1);
    std::string executable = lua_table_getfield_string(L, "executable");
    std::string arguments = lua_table_haskey(L, "arguments") ? lua_table_getfield_string(L, "arguments") : "";
    std::string directory = lua_table_haskey(L, "directory") ? lua_table_getfield_string(L, "directory") : "";
    bool waitForPause = lua_table_haskey(L, "wait") ? lua_table_getfield_boolean(L, "wait") : false;
    lua_pop(L, 1);

    std::string cmd = "InitDebug \"" + executable + "\"";
    if (arguments.empty() == false)
    {
        cmd += ",\"" + Utils::escapeQuotes(arguments) + "\"";
    }
    if (directory.empty() == false)
    {
        cmd += ",\"" + directory + "\"";
    }

    bool res = DbgCmdExec(cmd.c_str());

    if (res && waitForPause)
        _plugin_waituntilpaused();

    lua_pushboolean(L, res);

    return 1;
}

static int lua_debugger_stop(lua_State *L)
{
    bool res = DbgCmdExecDirect("stop");
    lua_pushboolean(L, res);

    return 1;
}

static const luaL_Reg lua_debugger_global[] =
{
    { "start", lua_debugger_start },
    { nullptr, nullptr },
};

static const luaL_Reg lua_debugger[] =
{
    { "stepOver", lua_debugger_stepOver },
    { "stepIn", lua_debugger_stepIn },
    { "run", lua_debugger_run },
    { "wait", lua_debugger_wait },
    { "isRunning", lua_debugger_isRunning },
    { "switchThread", lua_debugger_switchThread },
    { "stop", lua_debugger_stop },
    { nullptr, nullptr },
};

bool luaopen_debugger(lua_State *L, bool debugState)
{
    if(debugState)
        luaL_newlib(L, lua_debugger);
    else
        luaL_newlib(L, lua_debugger_global);
    lua_setglobal(L, "debugger");
    return true;
}
