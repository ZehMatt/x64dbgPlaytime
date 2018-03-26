#include "plugin.h"
#include "lua_context.hpp"

enum
{
    MENU_TEST,
    MENU_DISASM_ADLER32,
    MENU_DUMP_ADLER32,
    MENU_STACK_ADLER32
};

static void Adler32Menu(int hWindow)
{
    if(!DbgIsDebugging())
    {
        dputs("You need to be debugging to use this command");
        return;
    }
    SELECTIONDATA sel;
    GuiSelectionGet(hWindow, &sel);
    duint len = sel.end - sel.start + 1;
    unsigned char* data = new unsigned char[len];
    if(DbgMemRead(sel.start, data, len))
    {
        DWORD a = 1, b = 0;
        for(duint index = 0; index < len; ++index)
        {
            a = (a + data[index]) % 65521;
            b = (b + a) % 65521;
        }
        delete[] data;
        DWORD checksum = (b << 16) | a;
        dprintf("Adler32 of %p[%X] is: %08X\n", sel.start, len, checksum);
    }
    else
        dputs("DbgMemRead failed...");
}

static bool cbLuaCommand(int argc, char* argv[])
{
    const char *luaCode = argv[0] + 4;

    _plugin_logprintf("> %s\n", luaCode);

    return g_pLuaContext->runString(luaCode);
}

static duint exprZero(int argc, duint* argv, void* userdata)
{
    return 0;
}

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
    dprintf("Debugging of %s started!\n", info->szFileName);
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    dputs("Debugging stopped!");
}

PLUG_EXPORT void CBEXCEPTION(CBTYPE cbType, PLUG_CB_EXCEPTION* info)
{
    dprintf("ExceptionRecord.ExceptionCode: %08X\n", info->Exception->ExceptionRecord.ExceptionCode);
}

PLUG_EXPORT void CBDEBUGEVENT(CBTYPE cbType, PLUG_CB_DEBUGEVENT* info)
{
    if(info->DebugEvent->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
    {
        dprintf("DebugEvent->EXCEPTION_DEBUG_EVENT->%.8X\n", info->DebugEvent->u.Exception.ExceptionRecord.ExceptionCode);
    }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_TEST:
        MessageBoxA(hwndDlg, "Test Menu Entry Clicked!", PLUGIN_NAME, MB_ICONINFORMATION);
        break;

    case MENU_DISASM_ADLER32:
        Adler32Menu(GUI_DISASSEMBLY);
        break;

    case MENU_DUMP_ADLER32:
        Adler32Menu(GUI_DUMP);
        break;

    case MENU_STACK_ADLER32:
        Adler32Menu(GUI_STACK);
        break;

    default:
        break;
    }
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

    g_pLuaContext = new LuaContext();
    _plugin_logprintf("Lua context created\n");

    if (!g_pLuaContext->registerLibs())
    {
        _plugin_logprintf("Unable to register lua libraries.\n");
    }
    else
    {
        _plugin_logprintf("Registered lua libraries.\n");
    }

    if (g_pLuaContext->executeAutorunScripts())
    {
        _plugin_logprintf("One ore multiple scripts failed.\n");
    }

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
    if (g_pLuaContext)
    {
        delete g_pLuaContext;
        g_pLuaContext = nullptr;
    }
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU_TEST, "&Menu Test");
    _plugin_menuaddentry(hMenuDisasm, MENU_DISASM_ADLER32, "&Adler32 Selection");
    _plugin_menuaddentry(hMenuDump, MENU_DUMP_ADLER32, "&Adler32 Selection");
    _plugin_menuaddentry(hMenuStack, MENU_STACK_ADLER32, "&Adler32 Selection");
}
