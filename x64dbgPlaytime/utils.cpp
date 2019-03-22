#include "utils.h"
#include <windows.h>
#include <Shlwapi.h>
#include <assert.h>

#pragma comment(lib, "Shlwapi.lib")

namespace Utils {

std::string pathCombine(const std::string& path, const std::string& add)
{
    std::string res = path;

    bool appendSlash = false;

    if (path.size() >= 1 && path[path.size() - 1] != '\\')
        appendSlash = true;
    
    if (add.size() >= 1 && add[0] == '\\' && appendSlash == true)
        appendSlash = false;

    if(appendSlash)
        res += "\\";

    res += add;
    return res;
}

std::string removeFileNameFromPath(const std::string& path)
{
    size_t n = path.rfind('\\');
    if(n == path.npos)
        return path;

    std::string res = path.substr(0, n);
    return res;
}

std::string getFileNameFromPath(const std::string& path)
{
    size_t n = path.rfind('\\');
    if (n == path.npos)
        return path;

    std::string res = path.substr(n + 1);
    return res;
}

std::string canonicalizePath(const std::string& path)
{
    char tempBuffer[1024] = {};

    PathCanonicalizeA(tempBuffer, path.c_str());

    return tempBuffer;
}

std::string getBasePath()
{
    char tempBuffer[1024] = {};

    // Get path of current process.
    GetModuleFileNameA(nullptr, tempBuffer, sizeof(tempBuffer));

    // Strip image name.
    std::string path = removeFileNameFromPath(tempBuffer);

    // Remove architecture directory.
    std::string arch = getFileNameFromPath(path);
    if (arch == "x32" || arch == "x64")
    {
        path = canonicalizePath(path + "\\..");
    }
    else
    {
        // This will cause problems if the user has a different setup.
        assert(false);
    }

    return path;
}

} // Utils
