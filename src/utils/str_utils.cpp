#include "utils/str_utils.hpp"
#include <string>
#include <random>
#include <cctype>

std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }

    return s.substr(start, end - start);
}

/**
 * @brief Counts the number of consecutive occurrences of a specific character
 *        immediately before a given position in a string.
 *
 * This function is useful, for example, when analyzing source code to determine
 * if a quote character (' or ") is escaped by preceding backslashes.
 *
 * @param s   The string to search within.
 * @param pos The position in the string before which to start counting.
 *            The counting starts from s[pos-1] and goes backward.
 * @param ch  The character to count consecutively before the given position.
 * @return    The number of consecutive occurrences of 'ch' immediately preceding 'pos'.
 *
 * @note If pos is 0, the function returns 0.
 *
 * @example
 * std::string code = R"(Hello \"world\")";
 * int count = count_consecutive_before(code, 7, '\\'); // returns 1
 */
int countConsecutiveBefore(const std::string& s, size_t pos, char ch) {
    if (pos == 0) return 0; 
    int count = 0;
    for (size_t i = pos; i-- > 0;) {
        if (s[i] == ch) count++;
        else break;
    }
    return count;
}

std::string randomString(std::size_t length) {
    static const std::string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    thread_local static std::mt19937 rng(std::random_device{}());
    thread_local static std::uniform_int_distribution<std::size_t> dist(0, chars.size() - 1);

    std::string result;
    result.reserve(length);

    for (std::size_t i = 0; i < length; ++i)
        result += chars[dist(rng)];

    return result;
}


std::size_t findKeyword(const std::string &str, const std::string &tag, std::size_t st) {
    std::size_t pos = str.find(tag, st);
    if(pos == std::string::npos)
        return std::string::npos;
    if((pos!=0) && (isalnum(str[pos-1]) || str[pos-1]=='_' )) {
        return findKeyword(str, tag, pos+1);
    }
    if((pos+tag.size()<str.size()) && (isalnum(str[pos+tag.size()]) || str[pos+tag.size()]=='_')) {
        return findKeyword(str, tag, pos+1);
    }
    return pos;
}
std::size_t findKeyword(const std::string &str, const std::string &tag) {
    return findKeyword(str, tag, 0);
}
std::size_t findKeyword(const std::string &str, const char tag, std::size_t st) {
    return findKeyword(str, std::string("") + tag, st);
}
std::size_t findKeyword(const std::string &str, const char tag) {
    return findKeyword(str, tag, 0);
}

/**
 * @brief split the string by a character
 */
std::vector<std::string> split(const std::string &str, char c, bool allowEmpty) {
    std::string t=""; 
    std::vector<std::string> result;
    for(int i=0;i<str.size();i++) {
        if(str[i]!=c)
            t.push_back(str[i]);
        else {
            if(allowEmpty || t!="")
                result.push_back(t);
            t = "";
        }
    }
    if(allowEmpty || t!="") {
        result.push_back(t);
    }
    return result;
}

std::vector<std::string> split(const std::string &str, char c) {
    return split(str, c, true);
}

std::string removeSpace(const std::string &str) {
    std::string ns;
    for(std::size_t i=0;i<str.size();i++) {
        if(!std::isspace(str[i])) {
            ns.push_back(str[i]);
        }
    }
    return ns;
}