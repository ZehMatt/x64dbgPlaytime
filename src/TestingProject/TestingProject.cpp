#include "pch.h"

void debugPrint(const char* format, ...)
{
    char buffer[1024]{};

    va_list argptr;
    va_start(argptr, format);
    vsprintf_s(buffer, format, argptr);
    va_end(argptr);

    printf("%s", buffer);

    // Remove newline.
    size_t len = strlen(buffer);
    if (len >= 1 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
    OutputDebugStringA(buffer);
}

#pragma optimize("", off)
extern "C" __declspec(noinline) void loop_function()
{
    for (int i = 0; i < 100; i++)
    {
        __nop();
    }
}
#pragma optimize("", on)

int main(int argc, const char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        debugPrint("argv[%d] = %s\n", i, argv[i]);
    }
    
    char currentDir[MAX_PATH]{};
    GetCurrentDirectoryA(sizeof(currentDir), currentDir);
    debugPrint("cwd: %s\n", currentDir);

    loop_function();
}
