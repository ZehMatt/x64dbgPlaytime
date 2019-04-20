#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "pluginsdk/_scriptapi_module.h"

static int lua_push_modulesectioninfo(lua_State *L, const Script::Module::ModuleSectionInfo& info)
{
    lua_newtable(L);
    lua_pushstring(L, "base"); lua_pushinteger(L, info.addr);
    lua_rawset(L, -3);
    lua_pushstring(L, "size"); lua_pushinteger(L, info.size);
    lua_rawset(L, -3);
    lua_pushstring(L, "name"); lua_pushstring(L, info.name);
    lua_rawset(L, -3);
    return 1;
}

static int lua_push_moduleinfo(lua_State *L, const Script::Module::ModuleInfo& info, bool includeSections = false)
{
    lua_newtable(L);
    lua_pushstring(L, "base"); lua_pushinteger(L, info.base);
    lua_rawset(L, -3);
    lua_pushstring(L, "size"); lua_pushinteger(L, info.size);
    lua_rawset(L, -3);
    lua_pushstring(L, "entrypoint"); lua_pushinteger(L, info.entry);
    lua_rawset(L, -3);
    lua_pushstring(L, "name"); lua_pushstring(L, info.name);
    lua_rawset(L, -3);
    lua_pushstring(L, "path"); lua_pushstring(L, info.path);
    lua_rawset(L, -3);

    if (includeSections == true)
    {
        lua_pushstring(L, "sections");
        lua_newtable(L);
        {
            BridgeList<Script::Module::ModuleSectionInfo> list;
            if (Script::Module::SectionListFromAddr(info.base, &list))
            {
                for (int32_t i = 0; i < list.Count(); i++)
                {
                    const Script::Module::ModuleSectionInfo& info = list[i];
                    lua_pushinteger(L, i + 1);
                    lua_push_modulesectioninfo(L, info);
                    lua_rawset(L, -3);
                }
            }
        }
        lua_rawset(L, -3);
    }

    return 1;
}

/*
static int lua_modules_getSectionList(lua_State *L)
{
    BridgeList<Script::Module::ModuleSectionInfo> list;

    lua_Integer addr = lua_tointeger(L, 1);

    bool res = Script::Module::SectionListFromAddr(addr, &list);
    lua_newtable(L);
    if (res)
    {
        for (int32_t i = 0; i < list.Count(); i++)
        {
            const Script::Module::ModuleSectionInfo& info = list[i];
            lua_pushinteger(L, i + 1);
            lua_push_modulesectioninfo(L, info);
            lua_rawset(L, -3);
        }
    }

    return 1;
}
*/

static int lua_modules_getMain(lua_State *L)
{
    const char *name = lua_tostring(L, 1);

    Script::Module::ModuleInfo info = {};
    bool res = Script::Module::GetMainModuleInfo(&info);
    if (res)
    {
        lua_push_moduleinfo(L, info, true);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_modules_findByAddress(lua_State *L)
{
    lua_Integer addr = lua_tointeger(L, 1);

    Script::Module::ModuleInfo info = {};
    bool res = Script::Module::InfoFromAddr((duint)addr, &info);
    if (res)
    {
        lua_push_moduleinfo(L, info, true);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_modules_findByName(lua_State *L)
{
    const char *name = lua_tostring(L, 1);

    Script::Module::ModuleInfo info = {};
    bool res = Script::Module::InfoFromName(name, &info);
    if (res)
    {
        lua_push_moduleinfo(L, info, true);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_modules_getList(lua_State *L)
{
    BridgeList<Script::Module::ModuleInfo> list;

    bool res = Script::Module::GetList(&list);
    lua_newtable(L);
    if (res)
    {
        for (int32_t i = 0; i < list.Count(); i++)
        {
            const Script::Module::ModuleInfo& info = list[i];
            lua_pushinteger(L, i + 1);
            lua_push_moduleinfo(L, info, true);
            lua_rawset(L, -3);
        }
    }

    return 1;
}

static const luaL_Reg lua_modules[] =
{
    { "getMain", lua_modules_getMain },
    { "findByAddress", lua_modules_findByAddress },
    { "findByName", lua_modules_findByName },
    { "getList", lua_modules_getList },
    { nullptr, nullptr },
};

bool luaopen_modules(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_modules);
    lua_setglobal(L, "modules");

    return true;
}
