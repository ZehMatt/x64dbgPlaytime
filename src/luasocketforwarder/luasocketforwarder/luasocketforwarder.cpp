#ifdef _WIN64
#pragma comment(linker, "/export:luaopen_socket_core=x64dbgPlaytime.dp64.luaopen_socket_core")
#pragma comment(linker, "/export:luaopen_mime_core=x64dbgPlaytime.dp64.luaopen_mime_core")
#else
#pragma comment(linker, "/export:luaopen_socket_core=x64dbgPlaytime.dp32.luaopen_socket_core")
#pragma comment(linker, "/export:luaopen_mime_core=x64dbgPlaytime.dp32.luaopen_mime_core")
#endif //_WIN64