#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "pluginsdk/_scriptapi_memory.h"

#include <memory>

static int lua_memory_read(lua_State *L)
{
    lua_Integer addr = lua_tointeger(L, 1);
    lua_Integer len = lua_tointeger(L, 2);

    std::unique_ptr<uint8_t[]> buf(new uint8_t[(size_t)len]);

    duint dataRead = 0;
    if (!Script::Memory::Read(addr, buf.get(), (duint)len, &dataRead))
    {
        lua_pushnil(L);
        lua_pushinteger(L, 0);
    }
    else
    {
        lua_pushlstring(L, (const char*)buf.get(), dataRead);
        lua_pushinteger(L, dataRead);
    }

    return 2;
}

static int lua_memory_write(lua_State *L)
{
    size_t dataSize;
    lua_Integer addr = lua_tointeger(L, 1);
    const char *buf = luaL_tolstring(L, 2, &dataSize);

    duint dataWritten = 0;
    if (!Script::Memory::Write((duint)addr, buf, dataSize, &dataWritten))
    {
        lua_pushinteger(L, 0);
    }
    else
    {
        lua_pushinteger(L, dataWritten);
    }

    return 1;
}

template<typename T>
static int TypePtrMeta_Index(lua_State *L)
{
    luaL_checktable(L, 1);
    lua_Integer addr = lua_tointeger(L, 2);

    T res = 0;

    duint dataRead = 0;
    if (!Script::Memory::Read((duint)addr, &res, sizeof(T), &dataRead) || dataRead != sizeof(T))
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushinteger(L, res);
    }

    return 1;
}

template<typename T>
static int TypePtrMeta_NewIndex(lua_State *L)
{
    luaL_checktable(L, 1);

    lua_Integer addr = lua_tointeger(L, 2);
    lua_Integer val = lua_tointeger(L, 3);

    T ulValue = static_cast<T>(val);

    duint dataWritten = 0;
    if (!Script::Memory::Write((duint)addr, &ulValue, sizeof(T), &dataWritten) || dataWritten != sizeof(T))
    {
        // HMM
    }

    GuiUpdateMemoryView();
    GuiUpdateDisassemblyView();

    return 0;
}

static const luaL_Reg lua_memory[] =
{
    { "read", lua_memory_read },
    { "write", lua_memory_write },
    { nullptr, nullptr },
};

bool luaopen_memory(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_memory);
    lua_setglobal(L, "memory");

    // QWORD_PTR Metatable
    lua_newtable(L);
    {
        lua_pushvalue(L, -1);	// Backup of table.

        luaL_newmetatable(L, "qword_ptr");
        {
            lua_pushstring(L, "__index");
            lua_pushcfunction(L, TypePtrMeta_Index<uint64_t>);
            lua_settable(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, TypePtrMeta_NewIndex<uint64_t>);
            lua_settable(L, -3);

        } lua_setmetatable(L, -2);	// Pops the table.

    } lua_setglobal(L, "qword_ptr"); // Make table global, pops it again.

    // DWORD_PTR Metatable
    lua_newtable(L);
    {
        lua_pushvalue(L, -1);	// Backup of table.

        luaL_newmetatable(L, "dword_ptr");
        {
            lua_pushstring(L, "__index");
            lua_pushcfunction(L, TypePtrMeta_Index<uint32_t>);
            lua_settable(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, TypePtrMeta_NewIndex<uint32_t>);
            lua_settable(L, -3);

        } lua_setmetatable(L, -2);	// Pops the table.

    } lua_setglobal(L, "dword_ptr"); // Make table global, pops it again.

    // WORD_PTR Metatable
    lua_newtable(L);
    {
        lua_pushvalue(L, -1);	// Backup of table.

        luaL_newmetatable(L, "word_ptr");
        {
            lua_pushstring(L, "__index");
            lua_pushcfunction(L, TypePtrMeta_Index<uint16_t>);
            lua_settable(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, TypePtrMeta_NewIndex<uint16_t>);
            lua_settable(L, -3);

        } lua_setmetatable(L, -2);	// Pops the table.

    } lua_setglobal(L, "word_ptr"); // Make table global, pops it again.

    // BYTE_PTR Metatable
    lua_newtable(L);
    {
        lua_pushvalue(L, -1);	// Backup of table.

        luaL_newmetatable(L, "byte_ptr");
        {
            lua_pushstring(L, "__index");
            lua_pushcfunction(L, TypePtrMeta_Index<uint8_t>);
            lua_settable(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, TypePtrMeta_NewIndex<uint8_t>);
            lua_settable(L, -3);

        } lua_setmetatable(L, -2);	// Pops the table.

    } lua_setglobal(L, "byte_ptr"); // Make table global, pops it again.

    return true;
}
