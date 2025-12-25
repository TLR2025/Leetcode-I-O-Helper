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
#include "json.hpp"
#include "utils/selection.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

std::unordered_set<std::string> langs = {"cpp","java","py","py3","c","cs","js","ts","php","swift","kt","dart","go","rb","scala","rs","rkt","erl","ex"};

int main(int argc, char* argv[]) {

    // std::ifstream file(getExecutableDir() / "assets" / "starter_codes.json");
    // json data = json::parse(file);
    // auto q = data.at("questions").get<std::vector<json>>();
    // std::set<std::string> tps;
    // int cnt = 0;
    // for(auto p: q) {
    //     // std::cerr << "id: " << p.at("problem_id").get<std::string>() << std::endl;
    //     try {
    //         auto source = p.at("code_snippets").at("cpp").get<std::string>();
    //         try {
    //             std::vector<std::string> tpsa = processSourceCpp(source, "");
                
    //             for(auto x:tpsa) {
    //                 tps.insert(x);
    //             }
    //         } catch(std::runtime_error e) {
    //             std::cerr << cnt++ << p.at("problem_id").get<std::string>() + ": " << e.what() << std::endl;
    //         }
    //     } catch(...) {
    //         std::cerr << cnt++ << "Fatal error: " << p.at("problem_id").get<std::string>() << std::endl;
    //     }
    // }
    // for(auto x:tps) {
    //     std::cerr << x << std::endl;
    // }
    // return 0;

    CLI::App app{"LeetCode I-O helper"};
    argv = app.ensure_utf8(argv);
    std::string src_path = "", o_path = "", lang = "";
    int id = 0;

    app.add_option("-s,--source", src_path, "The path of the source file.")->required()->type_name("FILEPATH");
    app.add_option("-o,--output", o_path, "The path of the output file (original file as default).")->type_name("FILEPATH");
    app.add_option("-l,--lang", lang, "The language of the source (automatic inference as default).")->type_name("LANGUAGE");
    app.add_option("-i,--id", id, "The id of the problem (optional).")->type_name("NUMBER")->check(CLI::PositiveNumber);

    if(argc<=1) {
        std::cout << app.help() << std::endl;
        return 0;
    }

    try {
        app.parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        std::cerr << "[ERROR] "; 
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
            std::cerr << "[ERROR] Unknow language: \"" << lang << "\", allowed values: " << std::endl;
            for(auto x:langs) {
                std::cerr << x << " ";
            }
            std::cerr << std::endl;
            return 6;
        }
    }

    try {
        processSource(source, lang, o_path, id);
    } catch(std::runtime_error e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }
    return 0;
}