#pragma once

#include <string>
#include <vector>

std::string trim(const std::string& s);

int countConsecutiveBefore(const std::string& s, size_t pos, char ch);

std::string randomString(std::size_t length);

std::size_t findKeyword(const std::string &str, const std::string &tag, std::size_t st);
std::size_t findKeyword(const std::string &str, const std::string &tag);
std::size_t findKeyword(const std::string &str, const char tag, std::size_t st);
std::size_t findKeyword(const std::string &str, const char tag);

std::vector<std::string> split(const std::string &str, char c, bool allowEmpty);
std::vector<std::string> split(const std::string &str, char c);

std::string removeSpace(const std::string &str);