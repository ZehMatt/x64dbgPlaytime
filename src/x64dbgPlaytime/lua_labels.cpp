#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "pluginsdk/_scriptapi_label.h"

/*
SCRIPT_EXPORT bool Set(duint addr, const char* text, bool manual = false);
SCRIPT_EXPORT bool Set(const LabelInfo* info);
SCRIPT_EXPORT bool FromString(const char* label, duint* addr);
SCRIPT_EXPORT bool Get(duint addr, char* text); //text[MAX_LABEL_SIZE]
SCRIPT_EXPORT bool GetInfo(duint addr, LabelInfo* info);
SCRIPT_EXPORT bool Delete(duint addr);
SCRIPT_EXPORT void DeleteRange(duint start, duint end);
SCRIPT_EXPORT void Clear();
SCRIPT_EXPORT bool GetList(ListOf(LabelInfo) list); //caller has the responsibility to free the list
*/

static int lua_label_setname(lua_State *L)
{
    lua_Integer addr = lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    bool manual = false;
    if(lua_isboolean(L, 3))
        manual = lua_toboolean(L, 3);

    bool res = Script::Label::Set((duint)addr, name, manual);

    if (res)
    {
        GuiUpdateAllViews();
    }

    lua_pushboolean(L, res);
    return 1;
}

static int lua_label_getname(lua_State *L)
{
    lua_Integer addr = lua_tointeger(L, 1);

    char name[MAX_LABEL_SIZE] = {};
    bool res = Script::Label::Get((duint)addr, name);
    if(res)
        lua_pushstring(L, name);
    else
        lua_pushnil(L);
    return 1;
}

static int lua_push_labelinfo(lua_State *L, const Script::Label::LabelInfo& info)
{
    lua_newtable(L);
    lua_pushstring(L, "module"); lua_pushstring(L, info.mod);
    lua_rawset(L, -3);
    lua_pushstring(L, "rva"); lua_pushinteger(L, info.rva);
    lua_rawset(L, -3);
    lua_pushstring(L, "text"); lua_pushstring(L, info.text);
    lua_rawset(L, -3);
    lua_pushstring(L, "manual"); lua_pushboolean(L, info.manual);
    lua_rawset(L, -3);
    return 1;
}

static int lua_label_getinfo(lua_State *L)
{
    lua_Integer addr = lua_tointeger(L, 1);

    Script::Label::LabelInfo info = {};
    bool res = Script::Label::GetInfo((duint)addr, &info);
    if (res)
    {
        lua_push_labelinfo(L, info);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_label_remove(lua_State *L)
{
    bool res = false;

    lua_Integer addr = lua_tointeger(L, 1);
    if (lua_isinteger(L, 2))
    {
        lua_Integer addrEnd = lua_tointeger(L, 2);
        Script::Label::DeleteRange((duint)addr, (duint)addrEnd);
        res = true; // Hmm
    }
    else
    {
        res = Script::Label::Delete((duint)addr);
    }

    if (res)
    {
        GuiUpdateAllViews();
    }

    lua_pushboolean(L, res);
    return 1;
}

static int lua_label_getList(lua_State *L)
{
    BridgeList<Script::Label::LabelInfo> list;

    bool res = Script::Label::GetList(&list);
    lua_newtable(L);
    if (res)
    {
        for (int32_t i = 0; i < list.Count(); i++)
        {
            const Script::Label::LabelInfo& info = list[i];
            lua_pushinteger(L, i + 1);
            lua_push_labelinfo(L, info);
            lua_rawset(L, -3);
        }
    }

    return 1;
}

static const luaL_Reg lua_labels[] =
{
    { "setName", lua_label_setname },
    { "getName", lua_label_getname },
    { "getInfo", lua_label_getinfo },
    { "remove", lua_label_remove },
    { "getList", lua_label_getList },
    { nullptr, nullptr },
};

bool luaopen_labels(lua_State *L, bool debugState)
{
    luaL_newlib(L, lua_labels);
    lua_setglobal(L, "labels");

    return true;
}
