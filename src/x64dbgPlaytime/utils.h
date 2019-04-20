#pragma once

#include <string>
#include <vector>

namespace Utils {

bool readFileContents(const char *file, const char *mode, std::vector<uint8_t>& data);

struct DirectoryEntry_t
{
    std::string basePath;
    std::string fullFilePath;
    std::string fileName;
    bool isDirectory;
};

std::vector<DirectoryEntry_t> readDirectory(const std::string& path, const std::string& searchPattern);

std::string pathCombine(const std::string& path, const std::string& add);
std::string removeFileNameFromPath(const std::string& path);
std::string getFileNameFromPath(const std::string& path);
std::string canonicalizePath(const std::string& path);
std::string escapeQuotes(const std::string& val);
std::string getx64DbgBasePath();

} // Utils
