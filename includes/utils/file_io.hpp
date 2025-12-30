#pragma once
#include <string>
#include <filesystem>

std::string readFileAsString(const std::string& path);
void writeStringToFile(const std::string& path, const std::string& content);

std::filesystem::path getExecutableDir();