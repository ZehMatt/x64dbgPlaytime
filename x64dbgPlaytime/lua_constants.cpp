#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "pluginsdk/_scriptapi_memory.h"

static void define_constant(lua_State *L, const char *name, lua_Integer val)
{
    lua_pushinteger(L, val);
    lua_setglobal(L, name);
}

bool luaopen_constants(lua_State *L)
{
    // Flags
    define_constant(L, "EFL_C", 1 << 0);
    define_constant(L, "EFL_P", 1 << 2);
    define_constant(L, "EFL_A", 1 << 4);
    define_constant(L, "EFL_Z", 1 << 6);
    define_constant(L, "EFL_S", 1 << 7);
    define_constant(L, "EFL_T", 1 << 8);
    define_constant(L, "EFL_I", 1 << 9);
    define_constant(L, "EFL_D", 1 << 10);
    define_constant(L, "EFL_O", 1 << 11);

    return true;
}
