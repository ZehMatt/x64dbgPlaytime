#ifndef LUA_CONTEXT_HPP
#define LUA_CONTEXT_HPP

#include "lua.hpp"

#include <stdint.h>
#include <string>
#include <vector>

class LuaContext;

extern LuaContext* g_pLuaContext;

enum class LuaScriptState : uint8_t
{
    IDLE = 0,
    RUNNING,
};

struct LuaDebugState
{
    uint32_t processId;
    lua_State *L;
};

class LuaContext
{
private:
    lua_State* _globalState;
    lua_State* _coroutine;
    LuaScriptState _scriptState;
    bool _shouldResume;
    std::string _basePath;

 public:
    LuaContext();
    ~LuaContext();

    void update();
    void setBasePath(const std::string& path);
    const std::string& getBasePath() const;

    bool openLibraries(bool debugState);
    bool loadCoreScripts();

    bool createGlobalState();
    bool shutdownGlobalState();

    bool createDebugState(uint32_t processId);
    bool shutdownDebugState(uint32_t processId);

    bool runFile(const char *file, bool registerAutoupdate = false);
    bool runString(const char *lua, const char *path = "", const char *scriptName = "RUNSTRING");
    bool executeAutorunScripts();
    bool protectedLuaCall(int numParams = 0, int numReturns = 0);

    LuaScriptState getScriptState()
    {
        return _scriptState;
    }

    void setShouldResumeScript()
    {
        _shouldResume = true;
    }

    bool shouldResumeScript()
    {
        return _shouldResume;
    }

private:
    static void luaDebugCallback(lua_State *L, lua_Debug *Debug)
    {
        g_pLuaContext->debugHook(L, Debug);
    }

    static void luaPanicCallback(lua_State *L)
    {
        g_pLuaContext->panic(L);
    }

private:
    void initCoroutine();
    bool processError(int res, lua_State *L);
    void debugHook(lua_State *L, lua_Debug *debug);
    void panic(lua_State *L);
};

#define luaL_checktable(L,n)      (luaL_checktype(L, (n), LUA_TTABLE))

#endif // LUA_CONTEXT_HPP
