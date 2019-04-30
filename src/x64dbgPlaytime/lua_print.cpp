#include "lua_context.hpp"
#include "plugin.h"

int lua_print(lua_State* L)
{
    int nargs = lua_gettop(L);

    for (int i = 1; i <= nargs; i++)
    {
        if(i > 1)
            _plugin_logprintf("%s", "\t");

        if (lua_isstring(L, i))
        {
            const char *pszStr = lua_tostring(L, i);
            _plugin_logprintf("%s", pszStr);
        }
        else if (lua_isinteger(L, i))
        {
            _plugin_logprintf("%u", lua_tointeger(L, i));
        }
        else if (lua_isnumber(L, i))
        {
            _plugin_logprintf("%f", lua_tonumber(L, i));
        }
        else if (lua_isboolean(L, i))
        {
            _plugin_logprintf("%s", lua_toboolean(L, i) == 1 ? "true" : "false");
        }
        else if (lua_istable(L, i))
        {
            _plugin_logprintf("table(%p)", lua_topointer(L, i));
        }
        else if (lua_isnil(L, i))
        {
            _plugin_logprintf("%s", "<nil>");
        }
        else if (lua_isfunction(L, i))
        {
            _plugin_logprintf("function(%p)", lua_topointer(L, i));
        }
    }

    _plugin_logprintf("\n");

    return 0;
}

bool luaopen_x64dbgprint(lua_State *L, bool debugState)
{
    lua_getglobal(L, "_G");

    lua_pushcfunction(L, lua_print);
    lua_setfield(L, -2, "print");

    lua_setmetatable(L, -2);

    return true;
}
