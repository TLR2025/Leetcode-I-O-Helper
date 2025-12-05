#include <string>
#include <fstream>
#include "utils/file_io.hpp"

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#elif __linux__
    #include <unistd.h>
    #include <limits.h>
#else
    #error "Unsupported platform"
#endif

std::string readFileAsString(const std::string& path) {
    std::ifstream ifs(path);
    return std::string(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>()
    );
}

void writeStringToFile(const std::string& path, const std::string& content) {
    std::ofstream ofs(path);
    ofs << content;
}

std::filesystem::path getExecutableDir() {
    std::filesystem::path exePath;

#ifdef _WIN32
    char path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (len == 0) {
        throw std::runtime_error("Cannot get executable path");
    }
    exePath = std::filesystem::path(path);
#elif __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
        throw std::runtime_error("Buffer too small for executable path");
    }
    exePath = std::filesystem::canonical(std::filesystem::path(path));
#elif __linux__
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1) {
        throw std::runtime_error("Cannot get executable path");
    }
    exePath = std::filesystem::canonical(std::filesystem::path(std::string(path, count)));
#endif

    return exePath.parent_path();
}