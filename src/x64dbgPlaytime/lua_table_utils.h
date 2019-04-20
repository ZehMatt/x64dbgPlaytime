#pragma once

#include "lua.hpp"
#include <string>

/* Returns 0 if the key is not found otherwise 1, expects table to be on top of stack */
static bool lua_table_haskey(lua_State *L, const char* key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushstring(L, key);
    lua_gettable(L, -2);

    bool res = lua_isnil(L, -1) == false;

    lua_pop(L, 1);
    return res;
}

/* Returns 0 if the key is not found otherwise 1, expects table to be on top of stack */
static int lua_table_haskey(lua_State *L, lua_Number key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushnumber(L, key);
    lua_gettable(L, -2);

    bool res = lua_isnil(L, -1) == false;

    lua_pop(L, 1);
    return res;
}

/* Returns 0 if the key is not found otherwise 1, expects table to be on top of stack */
static int lua_table_haskey(lua_State *L, lua_Integer key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushinteger(L, key);
    lua_gettable(L, -2);

    bool res = lua_isnil(L, -1) == false;

    lua_pop(L, 1);
    return res;
}

/* Gets the member value of a table, expects table to be on top of stack */
static lua_Number lua_table_getfield_number(lua_State *L, const char* key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isnumber(L, -1))
        luaL_error(L, "%s expected as number", key);

    lua_Number res = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return res;
}

/* Gets the member value of a table, expects table to be on top of stack */
static lua_Integer lua_table_getfield_integer(lua_State *L, const char* key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isinteger(L, -1))
        luaL_error(L, "%s expected as integer", key);

    lua_Integer res = lua_tointeger(L, -1);
    lua_pop(L, 1);

    return res;
}

/* Gets the member value of a table, expects table to be on top of stack */
static std::string lua_table_getfield_string(lua_State *L, const char* key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isstring(L, -1))
        luaL_error(L, "%s expected as string", key);

    std::string res = lua_tostring(L, -1);
    lua_pop(L, 1);

    return res;
}

/* Gets the member value of a table, expects table to be on top of stack */
static bool lua_table_getfield_boolean(lua_State *L, const char* key)
{
    if (lua_istable(L, -1) == false)
        luaL_error(L, "Table expected, got: %s", luaL_typename(L, -1));

    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isboolean(L, -1))
        luaL_error(L, "%s expected as boolean", key);

    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);

    return res;
}
