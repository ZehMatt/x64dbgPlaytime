#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>

static int lua_utils_alert(lua_State *L)
{
    const char *str = lua_tostring(L, 1);
    const char *title = lua_tostring(L, 2);

    MessageBox(nullptr, str, title, MB_OK|MB_ICONWARNING);
    return 0;
}

static int lua_utils_question(lua_State *L)
{
    const char *str = lua_tostring(L, 1);
    const char *title = lua_tostring(L, 2);

    int res = MessageBox(nullptr, str, title, MB_YESNO | MB_ICONWARNING);
    lua_pushboolean(L, res == IDYES);
    
    return 1;
}

static const luaL_Reg lua_utils[] =
{
    { "alert", lua_utils_alert },
    { "question", lua_utils_question },
    { nullptr, nullptr },
};

bool luaopen_utils(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_utils);
    lua_setglobal(L, "utils");

    return true;
}
