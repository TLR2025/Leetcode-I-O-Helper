#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "process/cpp.hpp"
#include "process/process.hpp"

std::map<std::string, std::string> e2l = {
    {"cpp", "cpp"},
    {"java", "java"},
    {"py", "python"},
    {"py3", "python3"},
    {"c", "c"},
    {"cs", "csharp"},
    {"js", "javascript"},
    {"ts", "typescript"},
    {"php", "php"},
    {"swift", "swift"},
    {"kt", "kotlin"},
    {"dart", "dart"},
    {"go", "golang"},
    {"rb", "ruby"},
    {"scala", "scala"},
    {"rs", "rust"},
    {"rkt", "racket"},
    {"erl", "erlang"},
    {"ex", "elixir"}
};

void processSource(std::string &source, std::string lang, std::string o_path) {
    if(lang=="cpp") {
        processSourceCpp(source, o_path);
    } else {
        throw std::runtime_error("Sorry, this language is not currently supported.");
    }
}

