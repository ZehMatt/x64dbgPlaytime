#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "utils.h"

static int lua_include(lua_State *L)
{
    std::string includeFile = lua_tostring(L, 1);

    lua_getglobal(L, "_SCRIPT_PATH");
    std::string currentPath = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";

    if(currentPath.empty())
        currentPath = g_pLuaContext->getBasePath();

    std::string includeFilePath = Utils::pathCombine(currentPath, includeFile);

    bool res = g_pLuaContext->runFile(includeFilePath.c_str(), true);
    lua_pushboolean(L, res);

    return 1;
}

bool luaopen_globals(lua_State *L, bool debugState)
{
    lua_pushcfunction(L, lua_include);
    lua_setglobal(L, "include");

    return true;
}
