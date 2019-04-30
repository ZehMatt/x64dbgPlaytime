#include "plugin.h"
#include "lua_context.hpp"
#include "utils.h"

enum
{
    MENU_LOAD_LUA_FILE,
};

static bool cbLuaCommand(int argc, char* argv[])
{
    const char *luaCode = argv[0] + 4;

    dprintf("> %s\n", luaCode);

    return g_pLuaContext->runString(luaCode);
}

static duint exprZero(int argc, duint* argv, void* userdata)
{
    return 0;
}

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
    dprintf("Debugging of %s started!\n", info->szFileName);
    g_pLuaContext->createDebugState(0);
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    dputs("Debugging stopped!");
    g_pLuaContext->shutdownDebugState(0);
}

PLUG_EXPORT void CBEXCEPTION(CBTYPE cbType, PLUG_CB_EXCEPTION* info)
{
    dprintf("ExceptionRecord.ExceptionCode: %08X\n", info->Exception->ExceptionRecord.ExceptionCode);
}

PLUG_EXPORT void CBPAUSEDEBUG(CBTYPE cbType, PLUG_CB_PAUSEDEBUG *info)
{
}

PLUG_EXPORT void CBRESUMEDEBUG(CBTYPE cbType, PLUG_CB_RESUMEDEBUG *info)
{
}

PLUG_EXPORT void CBSTEPPED(CBTYPE cbType, PLUG_CB_STEPPED *info)
{
}

PLUG_EXPORT void CBDEBUGEVENT(CBTYPE cbType, PLUG_CB_DEBUGEVENT* info)
{
    if(info->DebugEvent->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
    {
        dprintf("DebugEvent->EXCEPTION_DEBUG_EVENT->%.8X\n", info->DebugEvent->u.Exception.ExceptionRecord.ExceptionCode);
    }
}

void plugin_menu_load_lua_file()
{
    OPENFILENAMEA ofn = {};       // common dialog box structure
    char szFile[260] = {};       // if using TCHAR macros

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Lua\0*.lua\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        g_pLuaContext->runFile(ofn.lpstrFile);
    }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_LOAD_LUA_FILE:
        plugin_menu_load_lua_file();
        break;
    default:
        break;
    }
}

bool cbLuaExecuteScript(const char* text)
{
    dprintf("> %s\n", text);

    return g_pLuaContext->runString(text);
}

void cbLuaScriptComplete(const char* text, char** entries, int* entryCount)
{
    if(entryCount)
        *entryCount = 0;
}

bool pluginScriptInit()
{
    SCRIPTTYPEINFO sti = {};
    sti.execute = cbLuaExecuteScript;
    sti.id = 'L' + 'u' + 'a' + '5' + '3';
    strcpy_s(sti.name, "Playtime");

    GuiRegisterScriptLanguage(&sti);

    return true;
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, "lua", cbLuaCommand, false);

    if (g_pLuaContext != nullptr)
    {
        // Double init?
        return false;
    }

    std::string basePath = Utils::getx64DbgBasePath();
    dprintf("Base Path: %s\n", basePath.c_str());

    std::string luaBasePath = Utils::pathCombine(basePath, "lua");
    dprintf("Lua Base Path: %s\n", luaBasePath.c_str());

    g_pLuaContext = new LuaContext();
    g_pLuaContext->setBasePath(luaBasePath);

    dprintf("Lua context created\n");

    if (!g_pLuaContext->createGlobalState())
    {
        dprintf("Unable to register lua libraries.\n");
    }
    else
    {
        dprintf("Registered lua libraries.\n");
    }

    if (!g_pLuaContext->executeAutorunScripts())
    {
        dprintf("One ore multiple scripts failed.\n");
    }

    pluginScriptInit();

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
    if (g_pLuaContext)
    {
        dprintf("Shutting down lua environment");

        delete g_pLuaContext;
        g_pLuaContext = nullptr;
    }
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU_LOAD_LUA_FILE, "&Load Lua File");
}
