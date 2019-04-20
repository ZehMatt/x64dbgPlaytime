#include "lua_context.hpp"
#include "plugin.h"
#include "utils.h"

// Global instance.
LuaContext* g_pLuaContext = nullptr;

LuaContext::LuaContext()
    : _globalState(nullptr),
    _coroutine(nullptr),
    _scriptState(LuaScriptState::IDLE),
    _shouldResume(false)
{
}

LuaContext::~LuaContext()
{
    shutdownGlobalState();
}

void LuaContext::update()
{
}

void LuaContext::setBasePath(const std::string& path)
{
    _basePath = path;
}

const std::string& LuaContext::getBasePath() const
{
    return _basePath;
}

bool LuaContext::openLibraries(bool debugState)
{
    extern bool luaopen_registers(lua_State *L, bool debugState);
    extern bool luaopen_x64dbgprint(lua_State *L, bool debugState);
    extern bool luaopen_debugger(lua_State *L, bool debugState);
    extern bool luaopen_memory(lua_State *L, bool debugState);
    extern bool luaopen_constants(lua_State *L, bool debugState);
    extern bool luaopen_utils(lua_State *L, bool debugState);
    extern bool luaopen_labels(lua_State *L, bool debugState);
    extern bool luaopen_modules(lua_State *L, bool debugState);
    extern bool luaopen_xrefs(lua_State *L, bool debugState);
    extern bool luaopen_bps(lua_State *L, bool debugState);
    extern bool luaopen_globals(lua_State *L, bool debugState);

    luaopen_debugger(_globalState, debugState);
    luaopen_constants(_globalState, debugState);
    luaopen_x64dbgprint(_globalState, debugState);
    luaopen_registers(_globalState, debugState);
    luaopen_utils(_globalState, debugState);
    luaopen_memory(_globalState, debugState);
    luaopen_labels(_globalState, debugState);
    luaopen_modules(_globalState, debugState);
    luaopen_xrefs(_globalState, debugState);
    luaopen_bps(_globalState, debugState);
    luaopen_globals(_globalState, debugState);

    return true;
}

bool LuaContext::loadCoreScripts()
{
    extern bool luaopen_x64dbg_bootstrap(lua_State *L, bool debugState);

    if (!luaopen_x64dbg_bootstrap(_globalState, false))
    {
        return false;
    }

    return true;
}

bool LuaContext::createGlobalState()
{
    if(_globalState != nullptr)
        return false;

    _globalState = luaL_newstate();
    //lua_sethook(_globalState, LuaContext::luaDebugCallback, LUA_MASKCALL | LUA_MASKCOUNT | LUA_MASKLINE| LUA_MASKRET, 0);

    if(!_globalState)
        return false;

    luaL_openlibs(_globalState);

    // x64dbg specific.
    openLibraries(false);

    return true;
}

bool LuaContext::shutdownGlobalState()
{
    if (_globalState)
    {
        lua_close(_globalState);
        _globalState = nullptr;

        return true;
    }

    return false;
}

bool LuaContext::createDebugState(uint32_t processId)
{
    openLibraries(true);
    return true;
}

bool LuaContext::shutdownDebugState(uint32_t processId)
{
    openLibraries(false);
    return true;
}

bool LuaContext::runFile(const char *file, bool registerAutoupdate /* = false*/)
{
    if (!_globalState)
        return false;

    std::vector<uint8_t> scriptData;

    dprintf("Loading script: %s\n", file);

    if (!Utils::readFileContents(file, "rt", scriptData))
    {
        dprintf("Unable to open file: %s\n", file);
        return false;
    }

    // Make sure the string is null terminated.
    scriptData.push_back(0);

    std::string path = Utils::removeFileNameFromPath(file);
    std::string fileName = Utils::getFileNameFromPath(file);

    return runString((const char*)scriptData.data(), path.c_str(), fileName.c_str());
}

bool LuaContext::runString(const char *lua, const char *path, const char *scriptName)
{
    std::string fullPath = "@" + Utils::pathCombine(path, scriptName);
    std::string currentPath;
    std::string currentScript;

    int res = luaL_loadbuffer(_globalState, lua, strlen(lua), fullPath.c_str());
    if (!processError(res, _globalState))
    {
        return false;
    }

    // Retrieve current path/name.
    {
        lua_getglobal(_globalState, "_SCRIPT_PATH");
        currentPath = lua_isstring(_globalState, -1) ? lua_tostring(_globalState, -1) : "";

        lua_getglobal(_globalState, "_SCRIPT_NAME");
        currentScript = lua_isstring(_globalState, -1) ? lua_tostring(_globalState, -1) : "";

        lua_pop(_globalState, 2);
    }

    // Assign new current path/name
    {
        lua_pushstring(_globalState, path);
        lua_setglobal(_globalState, "_SCRIPT_PATH");

        lua_pushstring(_globalState, scriptName);
        lua_setglobal(_globalState, "_SCRIPT_NAME");
    }
    
    // Execute.
    res = protectedLuaCall() ? 1 : 0;

    // Restore previous path/name
    {
        lua_pushstring(_globalState, currentPath.c_str());
        lua_setglobal(_globalState, "_SCRIPT_PATH");

        lua_pushstring(_globalState, currentScript.c_str());
        lua_setglobal(_globalState, "_SCRIPT_NAME");
    }

    return res == 1;
}

bool LuaContext::protectedLuaCall(int numArgs /*= 0*/, int numReturns /*= 0*/)
{
    _scriptState = LuaScriptState::RUNNING;

    int res = lua_pcall(_globalState, numArgs, numReturns, 0);
    if (!processError(res, _globalState))
    {
        _scriptState = LuaScriptState::IDLE;
        return false;
    }

    _scriptState = LuaScriptState::IDLE;

    return true;
}

void LuaContext::initCoroutine()
{
    if (_coroutine != nullptr || _globalState == nullptr)
    {
        return;
    }

    _coroutine = lua_newthread(_globalState);
}

bool LuaContext::processError(int res, lua_State *L)
{
    bool isError;
    if(L == _coroutine)
        isError = (res != 0 && res != LUA_YIELD);
    else
        isError = (res != 0);

    if (isError)
    {
        const char *pszError = lua_tostring(L, -1);
        if (pszError)
        {
            dprintf("Lua Error: %s\n", pszError);
        }

        if (L == _coroutine)
        {
            // Invalidate coroutine, no longer valid.
            _coroutine = nullptr;
        }
        else
        {
            // Get rid of the error on stack.
            lua_pop(L, 1);
        }

        return false;
    }

    return true;
}

void LuaContext::debugHook(lua_State *L, lua_Debug *debug)
{
    switch (debug->event)
    {
    case LUA_HOOKLINE:
    case LUA_HOOKCALL:
    case LUA_HOOKRET:
        break;
    }
}

void LuaContext::panic(lua_State *L)
{
    const char* pszError = lua_tostring(L, 1);
    if (pszError)
    {
        dprintf("Lua Panic: %s\n", pszError);
    }
}

bool LuaContext::executeAutorunScripts()
{
    std::vector<std::string> loadQueue;

    std::string autorunPath = Utils::pathCombine(_basePath, "autorun");

    // Queue up all single lua files.
    std::vector<Utils::DirectoryEntry_t> luaFiles = Utils::readDirectory(autorunPath, "*.lua");
    for (const auto& file : luaFiles)
    {
        loadQueue.push_back(file.fullFilePath);
    }

    // Queue up directory ones.
    std::vector<Utils::DirectoryEntry_t> luaDirs = Utils::readDirectory(autorunPath, "*.*");
    for (const auto& file : luaDirs)
    {
        if (file.isDirectory)
        {
            loadQueue.push_back(Utils::pathCombine(file.fullFilePath, "init.lua"));
        }
    }

    dprintf("Loading %u autostart scripts\n", (uint32_t)loadQueue.size());

    for (auto& queued : loadQueue)
    {
        runFile(queued.c_str(), true);
    }

    return true;
}
