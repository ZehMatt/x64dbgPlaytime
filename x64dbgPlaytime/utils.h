#pragma once

#include <string>
#include <vector>

namespace Utils {

struct DirectoryEntry_t
{
    std::string filePath;
    bool isDirectory;
};

std::vector<DirectoryEntry_t> readDirectory(const std::string& path, const std::string& searchPattern);

std::string pathCombine(const std::string& path, const std::string& add);
std::string removeFileNameFromPath(const std::string& path);
std::string getFileNameFromPath(const std::string& path);
std::string canonicalizePath(const std::string& path);
std::string getBasePath();


} // Utils
