#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <fstream>
#include <unordered_set>
#include <vector>
#include "utils/file_io.hpp"
#include "process/process.hpp"
#include "process/cpp.hpp"
#include "CLI11.hpp"
#include "utils/selection.hpp"

namespace fs = std::filesystem;

std::unordered_set<std::string> langs = {"cpp"};
std::string APP_VERSION = "1.0.0";

int main(int argc, char* argv[]) {
    CLI::App app{"A tool that can automatically generate the Input/Output logic based on a LeetCode's Solution class.", "LeetCode I/O helper"};
    argv = app.ensure_utf8(argv);
    std::string src_path = "", o_path = "", lang = "";

    app.add_option("-s,--source", src_path, "The path of the source file.")->required()->type_name("FILEPATH");
    app.add_option("-o,--output", o_path, "The path of the output file (default: original file).")->type_name("FILEPATH");
    app.add_option("-l,--lang", lang, "The language of the source (default: automatic inference).")->type_name("LANGUAGE");
    app.set_version_flag("-v,--version", APP_VERSION);

    if(argc<=1) {
        std::cout << app.help() << std::endl;
        return 0;
    }

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &e) {
        std::cout << e.what() << std::endl;
        return 0;
    } catch (const CLI::CallForVersion &e) {
        std::cout << e.what() << std::endl;
        return 0;
    } catch (const CLI::ParseError &e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return app.exit(e);
    }
    
    if(o_path == "") o_path = src_path;

    std::string source;
    try {
        bool srcFileExists = fs::exists(src_path);
        if(!srcFileExists) {
            std::cerr << "[ERROR] Source file does not exist!" << std::endl;
            return 1;
        }
        std::ifstream inFile(src_path);
        if (!inFile) {
            std::cerr << "[ERROR] Cannot open the target source file!" << std::endl;
            return 2;
        }
        source = readFileAsString(src_path);
        inFile.close();
    } catch(...) {
        std::cerr << "[ERROR] An error occurred while reading the file." << std::endl;
        return 3;
    }

    if(lang == "") {
        std::size_t pos = src_path.find_last_of(".");
        if(pos == std::string::npos) {
            std::cerr << "[ERROR] Cannot infer the type of the source code, because it doesn't have extension name." << std::endl;
            return 4;
        }
        std::string ext = src_path.substr(pos + 1);
        for(int i=0;i<ext.size();i++)
            ext[i] = tolower(ext[i]);
        if(langs.find(ext) != langs.end()) {
            lang = ext;
        } else {
            std::cerr << "[ERROR] Unknow extension name: \"." << ext << "\", allowed values: " << std::endl;
            for(auto x:langs) {
                std::cerr << "." << x << " ";
            }
            std::cerr << std::endl;
            std::cerr << "Or use -l to specify one language." << std::endl;
            return 5;
        }
    } else {
        for(int i=0;i<lang.size();i++)
            lang[i] = tolower(lang[i]);
        if(langs.find(lang) == langs.end()) {
            std::cerr << "[ERROR] Unsupported language: \"" << lang << "\", allowed values: " << std::endl;
            for(auto x:langs) {
                std::cerr << x << " ";
            }
            std::cerr << std::endl;
            return 6;
        }
    }

    try {
        processSource(source, lang, o_path);
    } catch(std::runtime_error e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }
    return 0;
}