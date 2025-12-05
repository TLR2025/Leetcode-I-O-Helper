/*
CustomFunction
ListNode
Master
MountainArray
Node
TreeNode
char
double
int
long long
string
vector<Employee*>
vector<ListNode*>
vector<TreeNode*>
vector<bool>
vector<char>
vector<double>
vector<int>
vector<long long>
vector<string>
vector<vector<char>>
vector<vector<int>>
vector<vector<long long>>
vector<vector<string>>
*/

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cctype>
#include "process/cpp.hpp"
#include "utils/str_utils.hpp"
#include "utils/selection.hpp"

struct Var {
    std::string name;
    std::string type;
};

struct Function {
    std::string name;
    std::vector<Var> params;
    std::string ret_type;
};


/**
 * @brief Remove all comment codes and string and character literals of the source.
 * @return The new source code will be returned.
 * @param source The original source string.
 */
std::string removeCommentsAndStrs(std::string &source) {
    std::string rst;
    bool inBlockComment = false;
    bool inString = false;
    bool inCharacter = false;
    for(int i=0;i<source.size();i++) {
        if(inBlockComment) {
            // Detect the end of the block comment
            if(i+1!=source.size() && source[i]=='*' && source[i+1]=='/') {
                inBlockComment = false;
                i++;
            }
        } else if(inString) {
            // Detect the end of the string literal
            if(source[i]=='\"' && countConsecutiveBefore(source, i, '\\')%2==0) {
                inString = false;
            }
        } else if(inCharacter) {
            // Detect the end of the char literal
            if(source[i]=='\'' && countConsecutiveBefore(source, i, '\\')%2==0) {
                inCharacter = false;
            }
        } else if(i+1!=source.size() && source[i]=='/' && source[i+1]=='/') {
            // Detect the start of the line comment
            int j=i;
            while(j<source.size() && source[j]!='\n') {
                j++;
            }
            i=j-1;
        } else if(i+1!=source.size() && source[i]=='/' && source[i+1]=='*') {
            // Detect the start of the block comment
            inBlockComment = true;
            i++;
        } else if(source[i]=='\"' && countConsecutiveBefore(source, i, '\\')%2==0) {
            // Detect the start of the string literal
            inString = true;
        } else if(source[i]=='\'' && countConsecutiveBefore(source, i, '\\')%2==0) {
            // Detect the start of the char literal
            inCharacter = true;
        } else {
            // Normal case
            rst.push_back(source[i]);
        }
    }
    return rst;
};

/**
 * @brief Find all class definitions in the source with comments and string and character literals removed.
 * @param source The source with comments and string and character literals removed.
 * @return An array of {name, st, ct}, among them, name is the name of the class, st and ct describe the interval of the part between { and }.
 */
auto findKlassDefinitions(std::string &source) {
    struct Klass {
        std::string name;
        std::size_t st, ct;
    };
    std::vector<Klass> rs;
    // Ensure that there isn't any "real" character adjacent to it
    std::size_t loc = findKeyword(source, "class");
    if(loc == std::string::npos) {
        return rs;
    }
    while(loc != std::string::npos) {
        // The first '{' position
        std::size_t st = source.find('{', loc);
        std::size_t ed = st+1;
        int balance = 1;
        while(balance != 0) {
            if(ed>=source.size()) {
                throw std::runtime_error("A match error occurred while analyzing the code.");
            }
            if(source[ed]=='{') {
                balance ++;
            } else if(source[ed]=='}') {
                balance --;
            }
            ed++;
        }
        std::string klass_name = trim(source.substr(loc+5, st-loc-5));
        rs.push_back({
            klass_name,
            st,
            ed-st
        });
        loc = findKeyword(source, "class", ed);
    }
    return rs;
}

/**
 * @brief Based on a definition (string) of the list of parameters of a function, return the list.
 * @param pstr The string of the definition of the parameters, between "()", included.
 */
auto extractParameters(std::string pstr) {
    std::vector<Var> params;
    auto variableStrs = split(pstr.substr(1,pstr.size()-2), ',');
    for(auto vs: variableStrs) {
        Var var;
        std::string tvs = trim(vs);
        int i = tvs.size()-1;
        while(std::isalnum(tvs[i]) || tvs[i]=='_') {
            i--;
        }
        var.name = tvs.substr(i+1);
        while(tvs[i]!='>' && !std::isalnum(tvs[i]) && tvs[i]!='_' && tvs[i]!='&' && tvs[i]!='*') {
            i--;
        }
        var.type = removeSpace(tvs.substr(0, i+1));
        params.push_back(var);
    }
    return params;
}

/**
 * @brief Find all public function definitions in one class body.
 * @param source The source of one class body, starts with '{', ends with '}'.
 */
auto findFunctionDefinitions(std::string &source) {
    std::vector<Function> funcs; 
    int balance = 0;
    for(std::size_t i=0;i<source.size();i++) {
        int prev = balance; 
        if(source[i]=='{') {
            balance ++;
        } else if(source[i]=='}') {
            balance --;
        }
        
        if(balance == 2 && prev == 1) {
            // Function body starts
            int ped = i-1; // parenthesis' end position
            while(ped>=0 && std::isspace(source[ped])) {
                ped--;
            }
            if(ped<0 || source[ped]!=')')
                continue;
            int pst = ped; // parenthesis' start position
            while(pst>=0 && source[pst]!='(') {
                pst--;
            }
            if(pst<0)
                throw std::runtime_error("A match error occurred while analyzing the code.");

            // [pst, ped]: Function's params' string, included "()".
            // params: Function's params
            auto params = extractParameters(source.substr(pst, ped-pst+1));
            int ned = pst-1; // name's end position
            while(std::isspace(ned)) {
                ned--;
            }
            if(!isalnum(source[ned]) && source[ned]!='_') 
                continue;
            int nst = ned-1; // name's start position
            while(nst>=0 && !std::isspace(source[nst])) {
                nst--;
            }
            if(nst<0)
                throw std::runtime_error("A match error occurred while analyzing the code.");
            nst++;
            // [nst, ned]: The function's name interval
            std::string name = source.substr(nst, ned-nst+1);
            int ted = nst-1; // Type's end position
            while(std::isspace(source[ted]))
                ted--;
            int tst = ted-1; // Type's start position
            while(tst>=0 && source[tst]!='}' && source[tst]!=';' && source[tst]!='{' && source[tst]!=':') {
                tst--;
            }
            tst++;
            // removeSpace(source[tst, ted]): function's type
            std::string type = removeSpace(source.substr(tst, ted-tst+1));
            Function fun = {name, params, type};
            funcs.push_back(fun);
        }
    }
    
    if(balance!=0)
        throw std::runtime_error("A match error occurred while analyzing the code.");
    
    return funcs;
};

void processSourceCpp(std::string &source, std::string o_path) {
    // Remove the comments and string and character literals
    std::string newSource = removeCommentsAndStrs(source);
    
    // Find all class definitions
    auto klasses = findKlassDefinitions(newSource);
    if(klasses.size()==0) {
        throw std::runtime_error("Cannot find any class definition in the given source!");
    }

    // Find the Solution class
    std::size_t si = 0;
    for(;si<klasses.size();si++)
        if(klasses[si].name=="Solution")
            break;
    if(si==klasses.size()) {
        throw std::runtime_error("Cannot find the Solution class!");
    }
    auto solutionKlass = klasses[si];
    auto solutionKlassBody = newSource.substr(solutionKlass.st, solutionKlass.ct);

    std::cout << "Solution class:" << std::endl;
    std::cout << solutionKlassBody << std::endl;

    auto funcs = findFunctionDefinitions(solutionKlassBody);
    std::cout << "Funcs:" << std::endl;
    for(auto func:funcs) {
        std::cout << func.ret_type << " " << func.name << std::endl;
        for(auto param:func.params) {
            std::cout << param.type << " " << param.name << "; ";
        }
        std::cout << std::endl;
    }
}