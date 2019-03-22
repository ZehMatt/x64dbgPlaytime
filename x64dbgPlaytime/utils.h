#pragma once

#include <string>

namespace Utils {

std::string pathCombine(const std::string& path, const std::string& add);
std::string removeFileNameFromPath(const std::string& path);
std::string getFileNameFromPath(const std::string& path);
std::string canonicalizePath(const std::string& path);
std::string getBasePath();


} // Utils
