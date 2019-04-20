#include "utils.h"
#include <windows.h>
#include <Shlwapi.h>
#include <assert.h>

#pragma comment(lib, "Shlwapi.lib")

namespace Utils {

bool readFileContents(const char *file, const char *mode, std::vector<uint8_t>& data)
{
    uint8_t tempBuffer[2048];

    FILE* fp = nullptr;
    fopen_s(&fp, file, mode);
    if(fp == nullptr)
        return false;

    // Clears buffer, we don't extend existing ones.
    data.clear();

    while (!feof(fp))
    {
        int bytesRead = fread(tempBuffer, 1, sizeof(tempBuffer), fp);
        if (bytesRead > 0)
        {
            data.insert(data.end(), &tempBuffer[0], &tempBuffer[bytesRead]);
        }
    }

    fclose(fp);
    return true;
}

std::vector<DirectoryEntry_t> readDirectory(const std::string& path, const std::string& searchPattern)
{
    std::vector<DirectoryEntry_t> res;

    std::string search = pathCombine(path, searchPattern);

    WIN32_FIND_DATAA fd = {};

    HANDLE hFind = FindFirstFileA(search.c_str(), &fd);
    if(hFind == nullptr)
        return res;

    do 
    {
        if(fd.cFileName[0] == '.' && fd.cFileName[1] == '\0')
            continue;
        if (fd.cFileName[0] == '.' && fd.cFileName[1] == '.' && fd.cFileName[2] == '\0')
            continue;

        DirectoryEntry_t entry;
        entry.fileName = fd.cFileName;
        entry.basePath = path;
        entry.fullFilePath = pathCombine(path, fd.cFileName);
        entry.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        res.emplace_back(std::move(entry));

    } while (FindNextFileA(hFind, &fd) != FALSE);

    FindClose(hFind);

    return res;
}

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


std::string escapeQuotes(const std::string& val)
{
    std::string res = val;
    for (auto it = res.begin(); it != res.end();)
    {
        if (*it == '"')
        {
            it = res.insert(it, '\\');
            it++;
        }
        it++;
    }
    return res;
}

std::string getx64DbgBasePath()
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
