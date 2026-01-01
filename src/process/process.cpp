#include <vector>
#include <string>
#include <stdexcept>
#include "process/cpp.hpp"
#include "process/process.hpp"

void processSource(std::string &source, std::string lang, std::string o_path) {
    if(lang=="cpp") {
        processSourceCpp(source, o_path);
    } else {
        throw std::runtime_error("Sorry, this language is not currently supported.");
    }
}

