#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>

static int lua_assembler_encode(lua_State *L)
{
    duint addr = (duint)lua_tointeger(L, 1);
    const char *instr = lua_tostring(L, 2);

    uint8_t dest[16];
    int outputSize = 0;
    char error[MAX_ERROR_SIZE];

    if (Script::Assembler::AssembleEx(addr, dest, &outputSize, instr, error))
    {
        lua_pushlstring(L, (const char*)dest, outputSize);
        lua_pushnil(L);
    }
    else
    {
        lua_pushnil(L);
        lua_pushstring(L, error);
    }

    return 2;
}

static const luaL_Reg lua_assembler[] =
{
    { "encode", lua_assembler_encode },
    { nullptr, nullptr },
};

bool luaopen_assembler(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_assembler);
    lua_setglobal(L, "assembler");

    return true;
}
