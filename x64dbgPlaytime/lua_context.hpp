#ifndef LUA_CONTEXT_HPP
#define LUA_CONTEXT_HPP

#include "lua.hpp"

class LuaContext
{
private:
    lua_State* _L;

 public:
    LuaContext();
    ~LuaContext();

    void update();
    bool registerLibs();
    bool runString(const char *lua);

    bool executeAutorunScripts();
};

extern LuaContext* g_pLuaContext;

#endif // LUA_CONTEXT_HPP