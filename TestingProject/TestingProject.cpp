
#include "pch.h"
#include <iostream>
#include <intrin.h>

#pragma optimize("", off)
extern "C" __declspec(noinline) void loop_function()
{
    for (int i = 0; i < 100; i++)
    {
        __nop();
    }
}
#pragma optimize("", on)

int main()
{
    loop_function();
}
