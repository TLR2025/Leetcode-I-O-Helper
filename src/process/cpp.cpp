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
#include <array>
#include <format>
#include <unordered_set>
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


const std::array<std::string, 5> zeroDimension = {
    "char",
    "double",
    "int",
    "long long",
    "string",
};

const std::array<std::string, 6> oneDimension = {
    "vector<bool>",
    "vector<char>",
    "vector<double>",
    "vector<int>",
    "vector<long long>",
    "vector<string>",
};

const std::array<std::string, 4> twoDimension = {
    "vector<vector<char>>",
    "vector<vector<int>>",
    "vector<vector<long long>>",
    "vector<vector<string>>"
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
        bool flag = (tvs.substr(0,5) == "const");
        if(flag)
            var.type = "const " + removeSpace(tvs.substr(5, i-4));
        else
            var.type = removeSpace(tvs.substr(0, i+1));
        params.push_back(var);
    }
    return params;
}

/**
 * @brief Find all PUBLIC function definitions in one class body.
 * @param source The source of one class body, starts with '{', ends with '}'.
 */
auto findFunctionDefinitions(std::string &source) {
    bool pub = false;
    std::vector<Function> funcs; 
    int balance = 0;
    for(std::size_t i=0;i<source.size();i++) {
        int prev = balance; 
        if(source[i]=='{') {
            balance ++;
        } else if(source[i]=='}') {
            balance --;
        }

        if(i>0 && std::isspace(source[i-1]) && i+6<source.size() && source.substr(i,7) == "public:") {
            pub = true;
        } else if(i>0 && std::isspace(source[i-1]) && i+7<source.size() && source.substr(i,8) == "private:") {
            pub = false;
        }

        if(!pub)
            continue;
        
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

/**
 * Given an 1d type, return its input code (that will be a lambda function). 
 */
auto generate1DInput(const std::string& type) {
    std::string rel_tp = type.substr(7, type.rfind('>') - 7);
    if(type == "vector<bool>") {
        return std::string("\t") + 
R"(auto input_1d_bool_ = [&](const string &str) {
		vector<bool> result;
		int st = str.find('[');
		int ed = str.rfind(']');
		vector<string> res_str = split_(str.substr(st + 1, ed - st - 1), ',', false);
		for(auto &it:res_str) {
			result.push_back(it.find("true")!=string::npos);
		}
		return result;
	};
)";
    } else if(type == "vector<string>") {
        return std::string(
R"(auto input_1d_string_ = [&](const string &str) {
		vector<string> result;
		int st = str.find('[');
		int ed = str.rfind(']');
		vector<string> res_str = split_(str.substr(st + 1, ed - st - 1), ',', false);
		for(auto &it:res_str) {
			result.push_back(it.substr(it.find('\"') + 1, it.rfind('\"') - it.find('\"') - 1));
		}
		return result;
	};
)");
    } else if(type == "vector<char>") {
        return std::string(
R"(auto input_1d_char_ = [&](const string &str) {
		vector<char> result;
		int st = str.find('[');
		int ed = str.rfind(']');
		vector<string> res_str = split_(str.substr(st + 1, ed - st - 1), ',', false);
		for(auto &it:res_str) {
			result.push_back(it.substr(it.find('\"') + 1, it.rfind('\"') - it.find('\"') - 1)[0]);
		}
		return result;
	};
)");
    } else {
        std::string convertFunction;
        if(rel_tp == "int") {
            convertFunction = "stoi";
        } else if(rel_tp == "long long") {
            convertFunction = "stoll";
        } else if(rel_tp == "double") {
            convertFunction = "stold";
        } else {
            throw std::runtime_error("Unsupported type: \"" + type + "\".");
        }
        return ("\t" + std::format(
R"(auto input_1d_{}_ = [&](const string &str) {{
		vector<{}> result;
		int st = str.find('[');
		int ed = str.rfind(']');
		vector<string> res_str = split_(str.substr(st + 1, ed - st - 1), ',', false);
		for(auto &it:res_str) {{
			result.push_back({}(it));
		}}
		return result;
    }};
)",     rel_tp, rel_tp, convertFunction));
    }
}

// Reimplement using std::format
/**
 * Given an entry function signature, return its corresponding main function.
 */
std::string generateMainFunction(Function &func) {
    std::string src;
    src = std::format("int main(int argc, char* argv[]) {{\n\tSolution solution;\n");

    for (std::size_t i = 0; i < func.params.size(); i++) {
        std::string tp = func.params[i].type;
        // Delete &
        tp.erase(std::remove(tp.begin(), tp.end(), '&'), tp.end());
        src += std::format("\t{} {};\n", tp, func.params[i].name);
    }
    
    // All 1d types
    std::unordered_set<std::string> _1d;
    // All 2d types
    std::unordered_set<std::string> _2d;
    for (std::size_t i = 0; i < func.params.size(); i++) {
        std::string tp = func.params[i].type;
        std::string nm = func.params[i].name;

        src += std::format("\tstd::cout << \"{}: \";\n", nm);

        if (std::find(zeroDimension.begin(), zeroDimension.end(), tp) != zeroDimension.end()) {
            if (tp == "string") {
                src += std::format("\tgetline(cin, {});\n", nm);
                src += std::format(
                    "\t{} = {}.substr({}.find('\"') + 1, {}.rfind('\"') - {}.find('\"') - 1);\n",
                    nm, nm, nm, nm, nm
                );
            } else if (tp == "char") {
                src += std::format("\tstring {}_str;\n", nm);
                src += std::format("\tcin >> {}_str;\n", nm);
                src += std::format(
                    "\t{} = {}_str.substr({}_str.find('\"') + 1, {}_str.rfind('\"') - {}_str.find('\"') - 1)[0];\n",
                    nm, nm, nm, nm, nm
                );
            } else {
                src += std::format("\tstd::cin >> {};\n", nm);
            }
        } else if (std::find(oneDimension.begin(), oneDimension.end(), tp) != oneDimension.end()) {
            _1d.insert(tp);
            src += std::format("\tstd::string {}_str;\n", nm);
            src += std::format("\tstd::getline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = {}({}_str);\n", nm,
                 std::format("input_1d_{}_",
                     tp.substr(tp.find('<') + 1, tp.find('>') - tp.find('<') - 1)
                ),
            nm);
        } else if (std::find(twoDimension.begin(), twoDimension.end(), tp) != twoDimension.end()) {
            _2d.insert(tp);
            src += std::format("\tstd::string {}_str;\n", nm);
            src += std::format("\tstd::getline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = {}({}_str);\n", nm,
                 std::format("input_2d_{}_",
                     tp.substr(tp.rfind('<') + 1, tp.find('>') - tp.rfind('<') - 1)
                ),
            nm);
        } else {
            throw std::runtime_error("Unsupported type: \"" + tp + "\".");
        }
    }

    src += "\treturn 0;\n}";
    return src;
}

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

    auto funcs = findFunctionDefinitions(solutionKlassBody);

    // std::cout << "Funcs:" << std::endl;
    // for(auto func:funcs) {
    //     std::cout << func.ret_type << " " << func.name << std::endl;
    //     for(auto param:func.params) {
    //         std::cout << param.type << " " << param.name << "; ";
    //     }
    //     std::cout << std::endl;
    // }

    if(funcs.size()==0) {
        throw std::runtime_error("Cannot find any public function in the Solution class.");
    }
    Function func;
    if(funcs.size() == 1)
        func = funcs[0];
    else {
        std::vector<std::string> stred_funcs(funcs.size());
        for(int i=0;i<funcs.size();i++) {
            stred_funcs[i] = funcs[i].ret_type + ' ' + funcs[i].name + '(';
            for(int j=0;j<funcs[i].params.size();j++) {
                stred_funcs[i] += funcs[i].params[j].type + ' ' + funcs[i].params[j].name;
                if(j!=funcs[i].params.size()-1)
                    stred_funcs[i]+=", ";
            }
            stred_funcs[i]+=");";
        }
        int ans = selection("Witch function is the entry function?", stred_funcs);
        func = funcs[ans];
    }

    std::cout << "Generating IO code for function \"" << func.name << "\" ..." << std::endl;

    std::cout << generateMainFunction(func) << std::endl;
}