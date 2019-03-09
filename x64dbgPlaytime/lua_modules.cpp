#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "pluginsdk/_scriptapi_module.h"

/*
SCRIPT_EXPORT bool InfoFromAddr(duint addr, ModuleInfo* info);
SCRIPT_EXPORT bool InfoFromName(const char* name, ModuleInfo* info);

SCRIPT_EXPORT duint BaseFromAddr(duint addr);
SCRIPT_EXPORT duint BaseFromName(const char* name);
SCRIPT_EXPORT duint SizeFromAddr(duint addr);
SCRIPT_EXPORT duint SizeFromName(const char* name);

SCRIPT_EXPORT bool NameFromAddr(duint addr, char* name); //name[MAX_MODULE_SIZE]
SCRIPT_EXPORT bool PathFromAddr(duint addr, char* path); //path[MAX_PATH]
SCRIPT_EXPORT bool PathFromName(const char* name, char* path); //path[MAX_PATH]

SCRIPT_EXPORT duint EntryFromAddr(duint addr);
SCRIPT_EXPORT duint EntryFromName(const char* name);
SCRIPT_EXPORT int SectionCountFromAddr(duint addr);
SCRIPT_EXPORT int SectionCountFromName(const char* name);
SCRIPT_EXPORT bool SectionFromAddr(duint addr, int number, ModuleSectionInfo* section);
SCRIPT_EXPORT bool SectionFromName(const char* name, int number, ModuleSectionInfo* section);
SCRIPT_EXPORT bool SectionListFromAddr(duint addr, ListOf(ModuleSectionInfo) list);
SCRIPT_EXPORT bool SectionListFromName(const char* name, ListOf(ModuleSectionInfo) list);
SCRIPT_EXPORT bool GetMainModuleInfo(ModuleInfo* info);
SCRIPT_EXPORT duint GetMainModuleBase();
SCRIPT_EXPORT duint GetMainModuleSize();
SCRIPT_EXPORT duint GetMainModuleEntry();
SCRIPT_EXPORT int GetMainModuleSectionCount();
SCRIPT_EXPORT bool GetMainModuleName(char* name); //name[MAX_MODULE_SIZE]
SCRIPT_EXPORT bool GetMainModulePath(char* path); //path[MAX_PATH]
SCRIPT_EXPORT bool GetMainModuleSectionList(ListOf(ModuleSectionInfo) list); //caller has the responsibility to free the list
SCRIPT_EXPORT bool GetList(ListOf(ModuleInfo) list); //caller has the responsibility to free the list

struct ModuleInfo
{
    duint base;
    duint size;
    duint entry;
    int sectionCount;
    char name[MAX_MODULE_SIZE];
    char path[MAX_PATH];
};

struct ModuleSectionInfo
{
    duint addr;
    duint size;
    char name[MAX_SECTION_SIZE * 5];
};
*/

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
    { "getSectionList", lua_modules_getSectionList },
    { "getList", lua_modules_getList },
    { nullptr, nullptr },
};

bool luaopen_modules(lua_State *L)
{
    luaL_newlib(L, lua_modules);
    lua_setglobal(L, "modules");

    return true;
}
