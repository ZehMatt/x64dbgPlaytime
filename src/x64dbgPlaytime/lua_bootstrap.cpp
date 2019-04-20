#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"
#include "utils.h"

static constexpr const char* CoreScripts[] =
{
    "core\\init.lua",
};

bool luaopen_x64dbg_bootstrap(lua_State *L, bool debugState)
{
    std::string basePath = g_pLuaContext->getBasePath();

    bool res = true;

    const uint32_t countCoreFiles = _countof(CoreScripts);
    dprintf("Loading %u core scripts\n", countCoreFiles);

    for (uint32_t i = 0; i < countCoreFiles; i++)
    {
        const char *coreScript = CoreScripts[i];

        std::string scriptPath = Utils::pathCombine(basePath, coreScript);

        res = g_pLuaContext->runFile(scriptPath.c_str(), true);
    }

    return res;
}
