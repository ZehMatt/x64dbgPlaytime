#include "lua_context.hpp"
#include "plugin.h"

// Global instance.
LuaContext* g_pLuaContext = nullptr;

LuaContext::LuaContext()
    : _globalState(nullptr),
    _coroutine(nullptr),
    _scriptState(LuaScriptState::IDLE),
    _shouldResume(false)
{
    _globalState = luaL_newstate();
    //lua_sethook(_globalState, LuaContext::luaDebugCallback, LUA_MASKCALL | LUA_MASKCOUNT | LUA_MASKLINE| LUA_MASKRET, 0);
}

LuaContext::~LuaContext()
{
    if (_globalState)
    {
        lua_close(_globalState);
        _globalState = nullptr;
    }
}

void LuaContext::update()
{
}

extern bool luaopen_registers(lua_State *L);
extern bool luaopen_x64dbgprint(lua_State *L);
extern bool luaopen_debugger(lua_State *L);

bool LuaContext::init()
{
    if(!_globalState)
        return false;

    luaL_openlibs(_globalState);

    // x64dbg specific.
    luaopen_registers(_globalState);
    luaopen_debugger(_globalState);
    luaopen_x64dbgprint(_globalState);

    return true;
}

bool LuaContext::runString(const char *lua)
{
    if(!_globalState)
        return false;

    initCoroutine();

    int res = luaL_loadstring(_coroutine, lua);
    if (!processError(res, _coroutine))
    {
        return false;
    }

    return resume();
}

bool LuaContext::resume()
{
    initCoroutine();

    _scriptState = LuaScriptState::RUNNING;
    _shouldResume = false;

    int res = lua_resume(_coroutine, nullptr, 0);
    if (!processError(res, _coroutine))
    {
        return false;
    }

    return true;
}

bool LuaContext::pause()
{
    dputs("Pausing script.\n");
    _scriptState = LuaScriptState::PAUSING;

    return true;
}

void LuaContext::initCoroutine()
{
    if (_coroutine != nullptr || _globalState == nullptr)
    {
        return;
    }

    _coroutine = lua_newthread(_globalState);
}

bool LuaContext::processError(int res, lua_State *L)
{
    bool isError;
    if(L == _coroutine)
        isError = (res != 0 && res != LUA_YIELD);
    else
        isError = (res != 0);

    if (isError)
    {
        const char *pszError = lua_tostring(L, -1);
        if (pszError)
        {
            _plugin_logprintf("Lua Error: %s\n", pszError);
        }

        if (L == _coroutine)
        {
            // Invalidate coroutine, no longer valid.
            _coroutine = nullptr;
        }

        return false;
    }

    return true;
}

void LuaContext::debugHook(lua_State *L, lua_Debug *debug)
{
    switch (debug->event)
    {
    case LUA_HOOKLINE:
    case LUA_HOOKCALL:
    case LUA_HOOKRET:
        break;
    }
}

void LuaContext::panic(lua_State *L)
{
    const char* pszError = lua_tostring(L, 1);
    if (pszError)
    {
        _plugin_logprintf("Lua Panic: %s\n", pszError);
    }
}

bool LuaContext::executeAutorunScripts()
{
    return true;
}
