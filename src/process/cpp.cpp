/*
UNSUPPORTED TYPES:
ListNode
Master
MountainArray
Node
TreeNode
vector<Employee*>
vector<ListNode*>
vector<TreeNode*>
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
#include <utility>
#include <optional>
#include <fstream>
#include <filesystem>
#include "process/cpp.hpp"
#include "utils/str_utils.hpp"
#include "utils/selection.hpp"
#include "utils/file_io.hpp"

struct Var {
    std::string name;
    std::string type;
};

struct Function {
    std::string name;
    std::vector<Var> params;
    std::string retType;
};


const std::array<std::string, 6> zeroDimension = {
    "bool",
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

const std::array<std::string, 6> twoDimension = {
    "vector<vector<bool>>",
    "vector<vector<char>>",
    "vector<vector<double>>",
    "vector<vector<int>>",
    "vector<vector<long long>>",
    "vector<vector<string>>",
};

/**
 * @brief Remove all comment code and string and character literals of the source.
 * @return A pair that contains the new source code and the mapping array.
 * @param source The original source string.
 */
std::pair<std::string, std::vector<std::size_t>> removeCommentsAndStrs(const std::string &source) {
    std::string rst;
    std::vector<std::size_t> mapping;
    bool inBlockComment = false;
    bool inString = false;
    bool inCharacter = false;
    for(std::size_t i=0;i<source.size();i++) {
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
            mapping.push_back(i);
        }
    }
    return {rst, mapping};
};

/**
 * @brief Find all class definitions in the source with comments and string and character literals removed.
 * @param source The source with comments and string and character literals removed.
 * @return An array of {name, st, ct}, among them, name is the name of the class, st and ct describe the interval of the part between { and }.
 */
auto findKlassDefinitions(const std::string &source) {
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
auto extractParameters(const std::string& pstr) {
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
 * @brief Find all public function definitions in one class body.
 * @param source The source of one class body, starts with '{', ends with '}'.
 */
auto findFunctionDefinitions(const std::string &source) {
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
            while(ned >= 0 && std::isspace(source[ned])) {
                ned--;
            }
            if(ned < 0 ||(!isalnum(source[ned]) && source[ned]!='_')) 
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
 * Inserts the given headers (directly to the source variable) if there are not included.
 */
auto insertHeadersIfNotIncluded(std::string &source, const std::vector<std::string> &headers) {
    std::unordered_set<std::string> includedHeaders;
    std::size_t loc = source.find("#include");
    while(loc!=std::string::npos) {
        std::string h = source.substr(loc+8, source.find('\n', loc) - loc - 8);

        if(h.find('<')!=std::string::npos && h.find('>')!=std::string::npos) {
            h = h.substr(h.find('<')+1, h.find('>')-h.find('<')-1);
        } else if(h.find('"')!=std::string::npos && h.find('"')!=h.rfind('"')) {
            h = h.substr(h.find('"')+1, h.rfind('"')-h.find('"')-1);
        } else {
            throw std::runtime_error("An error occurred while parsing the header file.");
        }
        h = trim(h);
        includedHeaders.insert(h);
        loc = source.find("#include", loc+1);
    }
    if(includedHeaders.count("bits/stdc++.h")) {
        return;
    }
    for(auto &header:headers) {
        if(!includedHeaders.count(header)) {
            source.insert(0, std::format("#include <{}>\n", header));
        }
    }

}

// Compressed code
/**
 * Given an 1d type, return its input code (that will be a lambda function). 
 */
auto generate1DInput(const std::string& type) {
    std::string rel_tp = type.substr(7, type.rfind('>') - 7);
    if(type == "vector<bool>") {
        return std::string("\t") + 
R"(auto input_1d_bool_=[&](const string&str){vector<bool>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return result;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);for(auto&it:res_str){result.push_back(it.find("true")!=string::npos);}return result;};)"
        + "\n";
    } else if(type == "vector<string>") {
        return std::string("\t") + 
R"(auto input_1d_string_=[](const string&s){vector<string>res;string temp;bool in_str=false;for(size_t i=0;i<s.size();++i){char c=s[i];if(c!='\\'){if(c=='"'){if(in_str){res.push_back(temp);temp.clear();}in_str=!in_str;}else if(in_str){temp.push_back(c);}continue;}if(++i>=s.size()||!in_str){throw runtime_error("Dangling backslash");}switch(s[i]){case '"':temp.push_back('"');break;case '\\':temp.push_back('\\');break;case '/':temp.push_back('/');break;case 'b':temp.push_back('\b');break;case 'f':temp.push_back('\f');break;case 'n':temp.push_back('\n');break;case 'r':temp.push_back('\r');break;case 't':temp.push_back('\t');break;default:throw runtime_error("Invalid escape");}}return res;};)"
        + "\n";
    } else if(type == "vector<char>") {
        return std::string("\t") + 
R"(auto input_1d_char_=[](const string&s){vector<char>res;for(size_t i=0;i<s.size();++i){char c=s[i];if(c=='"'){if(++i<s.size()){if(s[i]!='\\'){res.push_back(s[i]);}else{if(++i<s.size()){switch(s[i]){case '"':res.push_back('"');break;case '\\':res.push_back('\\');break;case '/':res.push_back('/');break;case 'b':res.push_back('\b');break;case 'f':res.push_back('\f');break;case 'n':res.push_back('\n');break;case 'r':res.push_back('\r');break;case 't':res.push_back('\t');break;default:throw runtime_error("Invalid escape");}}else{throw runtime_error("Parsing error!");}}++i;if(i>s.size()||s[i]!='"'){throw runtime_error("Parsing error!");}}else{throw runtime_error("Parsing error!");}}}return res;};)"
        + "\n";
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
R"(auto input_1d_{}_=[&](const string&str){{vector<{}>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return result;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);for(auto&it:res_str){{result.push_back({}(it));}}return result;}};)",
        rel_tp, rel_tp, convertFunction) + "\n");
    }
}

/**
 * Given an 2d type, return its input code (that will be a lambda function). 
 */
auto generate2DInput(const std::string& type) {
    std::string rel_tp = type.substr(type.rfind('<') + 1, type.find('>') - type.rfind('<') - 1);
if(type == "vector<vector<bool>>") {
        return std::string("\t") + 
R"(auto input_2d_bool_=[&](const string&str){vector<vector<bool>>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos||st>=ed)return result;for(size_t i=st+1;i<ed;++i){if(str[i]=='['){auto rpos=str.find(']',i);if(rpos==string::npos)break;string row=str.substr(i+1,rpos-i-1);auto row_data=split_(row,',',false);result.emplace_back();for(auto&res_str:row_data){result.back().push_back(res_str.find("true")!=string::npos);}i=rpos;}}return result;};)"
        + "\n";
    } else if(type == "vector<vector<string>>") {
        return std::string("\t") + 
R"(auto input_2d_string_=[](const string&s){auto st=s.find('['),ed=s.rfind(']');if(st==string::npos||ed==string::npos||ed<st){throw runtime_error("Parsing error!");}vector<vector<string>>res;string temp;bool in_str=false;for(size_t i=st+1;i<ed;++i){char c=s[i];if(c=='['&&!in_str){res.emplace_back();continue;}else if(c!='\\'){if(c=='"'){if(in_str){res[res.size()-1].push_back(temp);temp.clear();}in_str=!in_str;}else if(in_str){temp.push_back(c);}continue;}if(++i>=s.size()||!in_str){throw runtime_error("Dangling backslash");}switch(s[i]){case '"':temp.push_back('"');break;case '\\':temp.push_back('\\');break;case '/':temp.push_back('/');break;case 'b':temp.push_back('\b');break;case 'f':temp.push_back('\f');break;case 'n':temp.push_back('\n');break;case 'r':temp.push_back('\r');break;case 't':temp.push_back('\t');break;default:throw runtime_error("Invalid escape");}}return res;};)"
        + "\n";
    } else if(type == "vector<vector<char>>") {
        return std::string("\t") + 
R"(auto input_2d_char_=[](const string&s){auto st=s.find('['),ed=s.rfind(']');if(st==string::npos||ed==string::npos||ed<st){throw runtime_error("Parsing error!");}vector<vector<char>>res;for(size_t i=st+1;i<ed;++i){char c=s[i];if(c=='['){res.emplace_back();}if(c=='"'){if(++i<s.size()){if(s[i]!='\\'){res[res.size()-1].push_back(s[i]);}else{if(++i<s.size()){switch(s[i]){case '"':res[res.size()-1].push_back('"');break;case '\\':res[res.size()-1].push_back('\\');break;case '/':res[res.size()-1].push_back('/');break;case 'b':res[res.size()-1].push_back('\b');break;case 'f':res[res.size()-1].push_back('\f');break;case 'n':res[res.size()-1].push_back('\n');break;case 'r':res[res.size()-1].push_back('\r');break;case 't':res[res.size()-1].push_back('\t');break;default:throw runtime_error("Invalid escape");}}else{throw runtime_error("Parsing error!");}}++i;if(i>s.size()||s[i]!='"'){throw runtime_error("Parsing error!");}}else{throw runtime_error("Parsing error!");}}}return res;};)"
        + "\n";
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
R"(auto input_2d_{}_=[&](const string&str){{vector<vector<{}>>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos||st>=ed)return result;for(size_t i=st+1;i<ed;++i){{if(str[i]=='['){{auto rpos=str.find(']',i);if(rpos==string::npos)break;string row=str.substr(i+1,rpos-i-1);auto row_data=split_(row,',',false);result.emplace_back();for(auto&num_str:row_data){{result.back().push_back({}(num_str));}}i=rpos;}}}}return result;}};)",
            rel_tp, rel_tp, convertFunction) + "\n");
    }
}

/**
 * Based on the function's return type, generate the corresponding output code for the `result_` variable.
 */
std::string generateOutputCode(const std::string& type) {
    if(std::find(zeroDimension.begin(), zeroDimension.end(), type) != zeroDimension.end()) {
        if(type == "bool")
            return "\tcout << boolalpha << result_ << endl;\n";
        if(type == "string")
            return std::string("\t") + R"(cout<<'"';for(size_t i=0;i<result_.size();i++){char ch_[2]={result_[i],'\0'};cout<<(result_[i]=='"'?"\\\"" : (result_[i] == '\\' ? "\\\\" : (result_[i] == '\b' ? "\\b" : (result_[i] == '\f' ? "\\f" : (result_[i] == '\n' ? "\\n" : (result_[i] == '\r' ? "\\r" : (result_[i] == '\t' ? "\\t" : ch_)))))));}cout<<'"';)" + "\n";
        if(type == "char")
            return std::string("\t") + R"(char result_str_[]={result_,'\0'};cout<<'"';cout<<(result_=='"'?"\\\"" : (result_=='\\' ? "\\\\" : (result_=='\b' ? "\\b" : (result_ == '\f' ? "\\f" : (result_ == '\n' ? "\\n" : (result_ == '\r' ? "\\r" : (result_ == '\t' ? "\\t" : result_str_)))))));cout<<'"';)" + "\n";
        return "\tcout << result_ << endl;\n";
    } else if(std::find(oneDimension.begin(), oneDimension.end(), type) != oneDimension.end()) {
        if(type == "vector<bool>")
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<boolalpha<<"["<<result_[0];for(size_t i=1;i<result_.size();i++){cout<<boolalpha<<", "<<result_[i];}cout<<"]"<<endl;})" + "\n";
        if(type == "vector<string>")
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'[';for(size_t i=0;i<result_.size();i++){if(i!=0)cout<<", ";cout<<'"';for(size_t j=0;j<result_[i].size();j++){char ch_[2]={result_[i][j],'\0'};cout<<(result_[i][j]=='"'?"\\\"" : (result_[i][j] == '\\' ? "\\\\" : (result_[i][j] == '\b' ? "\\b" : (result_[i][j] == '\f' ? "\\f" : (result_[i][j] == '\n' ? "\\n" : (result_[i][j] == '\r' ? "\\r" : (result_[i][j] == '\t' ? "\\t" : ch_)))))));}cout<<'"';}cout<<']'<<endl;})" + "\n";
        if(type == "vector<char>")
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'[';for(size_t i=0;i<result_.size();i++){if(i!=0)cout<<", ";cout<<'"';char ch_[2]={result_[i],'\0'};cout<<(result_[i]=='"'?"\\\"" : (result_[i] == '\\' ? "\\\\" : (result_[i] == '\b' ? "\\b" : (result_[i] == '\f' ? "\\f" : (result_[i] == '\n' ? "\\n" : (result_[i] == '\r' ? "\\r" : (result_[i] == '\t' ? "\\t" : ch_)))))));cout<<'"';}cout<<']'<<endl;})" + "\n";
        return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<"["<<result_[0];for(size_t i=1;i<result_.size();i++){cout<<", "<<result_[i];}cout<<"]"<<endl;})" + "\n";
    } else if(std::find(twoDimension.begin(), twoDimension.end(), type) != twoDimension.end()) {
        if(type == "vector<vector<bool>>")
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'['<<endl;for(size_t i=0;i<result_.size();i++){const auto&row_=result_[i];if(row_.size()==0)cout<<"[]"<<endl;else{cout<<"\t[";cout<<setw(5)<<boolalpha<<row_[0];for(size_t j=1;j<row_.size();j++){cout<<", ";cout<<setw(5)<<boolalpha<<row_[j];}cout<<']';}if(i!=result_.size()-1)cout<<',';cout<<endl;}cout<<']'<<endl;})" + "\n";
        if(type == "vector<vector<string>>") 
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'['<<endl;for(size_t i=0;i<result_.size();i++){const auto&row_=result_[i];if(row_.size()==0)cout<<"[]"<<endl;else{cout<<"\t[";for(size_t j=0;j<row_.size();j++){if(j!=0)cout<<", ";cout<<'"';for(size_t k=0;k<row_[j].size();k++){char ch_[2]={row_[j][k],'\0'};cout<<(row_[j][k]=='"'?"\\\"" : (row_[j][k]=='\\' ? "\\\\" : (row_[j][k]=='\b' ? "\\b" : (row_[j][k] == '\f' ? "\\f" : (row_[j][k] == '\n' ? "\\n" : (row_[j][k] == '\r' ? "\\r" : (row_[j][k] == '\t' ? "\\t" : ch_)))))));}cout<<'"';}cout<<']';}if(i!=result_.size()-1)cout<<',';cout<<endl;}cout<<']'<<endl;})" + "\n";
        if(type == "vector<vector<char>>")
            return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'[';for(size_t i=0;i<result_.size();i++){if(i!=0)cout<<", ";cout<<'"';char ch_[2]={result_[i],'\0'};cout<<(result_[i]=='"'?"\\\"" : (result_[i] == '\\' ? "\\\\" : (result_[i] == '\b' ? "\\b" : (result_[i] == '\f' ? "\\f" : (result_[i] == '\n' ? "\\n" : (result_[i] == '\r' ? "\\r" : (result_[i] == '\t' ? "\\t" : ch_)))))));cout<<'"';}cout<<']'<<endl;})" + "\n";
        return std::string("\t") + R"(if(result_.size()==0)cout<<"[]"<<endl;else{cout<<'['<<endl;for(size_t i=0;i<result_.size();i++){const auto&row_=result_[i];if(row_.size()==0)cout<<"[]"<<endl;else{cout<<"\t[";cout<<setw(10)<<row_[0];for(size_t j=1;j<row_.size();j++){cout<<", ";cout<<setw(10)<<row_[j];}cout<<']';}if(i!=result_.size()-1)cout<<',';cout<<endl;}cout<<']'<<endl;})" + "\n";
    } else if(type == "ListNode*") {
        return std::string("\t") + R"(if(result_==nullptr)cout<<"[]"<<endl;else{cout<<"["<<result_->val;result_=result_->next;while(result_!=nullptr){cout<<", "<<result_->val;result_=result_->next;}cout<<"]";})" + "\n";
    }
    throw std::runtime_error("An error has occurred while generating the output code.");
}

// Reimplement using std::format
/**
 * Given an entry function signature, return its corresponding main function.
 */
std::string generateMainFunction(Function &func) {
    std::string src;
    src = std::format("int main(int argc, char* argv[]) {{\n\tSolution solution;\n");
    
    std::size_t lambdaInsertLoc = src.size();

    // All 1d types
    std::unordered_set<std::string> _1d;
    // All 2d types
    std::unordered_set<std::string> _2d;
    // Is the split_ function needed?
    bool needSplit = false;
    // Is the input_treenode_ function needed?
    bool treenode = false;
    // Is the input_listnode_ function needed?
    bool listnode = false; 
    for (std::size_t i = 0; i < func.params.size(); i++) {
        std::string tp = func.params[i].type;
        // Delete &
        tp.erase(std::remove(tp.begin(), tp.end(), '&'), tp.end());
        src += std::format("\t{} {};\n", tp, func.params[i].name);

        std::string nm = func.params[i].name;

        src += std::format("\tcout << \"{}: \";\n", nm);

        if (std::find(zeroDimension.begin(), zeroDimension.end(), tp) != zeroDimension.end()) {
            if(tp == "bool") {
                src += std::format("\tcin >> boolalpha >> {};\n", nm);
            } else if (tp == "string") {
                src += std::format("\tgetline(cin, {});\n\t", nm);
                src += std::format(
                    R"({}={}.substr({}.find('\"')+1,{}.rfind('\"')-{}.find('\"')-1);for(size_t i=0;i<{}.size();i++){{if({}[i]!='\\')continue;switch({}[i+1]){{case '"':{}.replace(i,2,"\"");;break;case '\\':{}.replace(i,2,"\\");;break;case '/':{}.replace(i,2,"/");;break;case 'b':{}.replace(i,2,"\b");;break;case 'f':{}.replace(i,2,"\f");;break;case 'n':{}.replace(i,2,"\n");;break;case 'r':{}.replace(i,2,"\r");;break;case 't':{}.replace(i,2,"\t");;break;default:throw std::runtime_error("Invalid escape");}}}})",
                    nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm
                );
            } else if (tp == "char") {
                src += std::format("\tstring {}_str;\n", nm);
                src += std::format("\tgetline(cin, {}_str);\n", nm);
                src += std::format(
                    "\t{}_str = {}_str.substr({}_str.find('\"') + 1, {}_str.rfind('\"') - {}_str.find('\"') - 1);\n",
                    nm, nm, nm, nm, nm
                );
                src += "\t" + std::format(
                    R"({}={}_str[0]=='\\'?({}_str[1]=='"'?'"':({}_str[1]=='\\'?'\\':({}_str[1]=='/'?'/':({}_str[1]=='b'?'\b':({}_str[1]=='f'?'\f':({}_str[1]=='n'?'\n':({}_str[1]=='r'?'\r':({}_str[1]=='t'?'\t':' ')))))))):{}_str[0];)",
                    nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm
                ) + "\n";
            } else {
                src += std::format("\tcin >> {};\n", nm);
            }
        } else if (std::find(oneDimension.begin(), oneDimension.end(), tp) != oneDimension.end()) {
            _1d.insert(tp);
            if (tp != "vector<char>" && tp != "vector<string>") 
                needSplit = true;
            src += std::format("\tstring {}_str;\n", nm);
            src += std::format("\tgetline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = {}({}_str);\n", nm,
                 std::format("input_1d_{}_",
                     tp.substr(tp.find('<') + 1, tp.find('>') - tp.find('<') - 1)
                ),
            nm);
        } else if (std::find(twoDimension.begin(), twoDimension.end(), tp) != twoDimension.end()) {
            _2d.insert(tp);
            if (tp != "vector<vector<char>>" && tp != "vector<vector<string>>") 
                needSplit = true;
            src += std::format("\tstring {}_str;\n", nm);
            src += std::format("\tgetline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = {}({}_str);\n", nm,
                 std::format("input_2d_{}_",
                     tp.substr(tp.rfind('<') + 1, tp.find('>') - tp.rfind('<') - 1)
                ),
            nm);
        } else if (tp == "TreeNode*") {
            treenode = true;
            src += std::format("\tstring {}_str;\n", nm);
            src += std::format("\tgetline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = input_treenode_({}_str);\n", nm, nm);
        } else if (tp == "ListNode*") {
            listnode = true;
            src += std::format("\tstring {}_str;\n", nm);
            src += std::format("\tgetline(std::cin, {}_str);\n", nm);
            src += std::format("\t{} = input_listnode_({}_str);\n", nm, nm);
        } else {
            throw std::runtime_error("Unsupported type: \"" + tp + "\".");
        }
    }
    if(listnode) {
        src.insert(lambdaInsertLoc, 
            std::string("\t")
            + R"(auto input_listnode_=[&](const string&str)->ListNode* {ListNode*root=new ListNode();ListNode*now=root;vector<int>arr=input_1d_int_(str);if(arr.size()==0)return nullptr;for(size_t i=0;i<arr.size()-1;i++)now->val=arr[i],now->next=new ListNode(),now=now->next;now->val=arr[arr.size()-1];return root;};)"
            + "\n"
        );
        _1d.insert("vector<int>");
        needSplit=true;
    }
    for(auto &tp:_1d) {
        src.insert(lambdaInsertLoc, generate1DInput(tp));
    }
    for(auto &tp:_2d) {
        src.insert(lambdaInsertLoc, generate2DInput(tp));
    }
    if(treenode) {
        needSplit = true;
        src.insert(lambdaInsertLoc,
            std::string("\t") 
            + R"(auto input_treenode_=[&](const string&str)->TreeNode* {vector<TreeNode*>treenode_arr;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return nullptr;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);if(res_str.size()==0)return nullptr;for(auto&it:res_str){if(it.find("null")!=string::npos){treenode_arr.push_back(nullptr);}else{TreeNode*nodep=new TreeNode(stoi(it));treenode_arr.push_back(nodep);}}queue<size_t>que;size_t ind=0;que.push(0);while(!que.empty()){size_t fr=que.front();que.pop();if(treenode_arr[fr]==nullptr)continue;++ind;if(ind>=treenode_arr.size())break;treenode_arr[fr]->left=treenode_arr[ind];que.push(ind);++ind;if(ind>=treenode_arr.size())break;treenode_arr[fr]->right=treenode_arr[ind];que.push(ind);}return treenode_arr[0];};)"
            + "\n"
        );
    }
    if(needSplit) {
        // Insert lambda function split_
        src.insert(lambdaInsertLoc, 
            std::string("\t") 
            + R"(auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};)"
            + "\n"
        );
    }
    // Call the entry function and get result
    std::string paramStr = func.params[0].name;
    for(int i=1;i<func.params.size();i++) 
        paramStr.append(", " + func.params[i].name);
    src += std::format("\tauto result_ = solution.{}({});\n", func.name, paramStr);
    src += "\tcout << \"result: \" << endl;\n";
    src += generateOutputCode(func.retType);
    // End of main function
    src += "\treturn 0;\n}";
    return src;
}

/**
 * Find the location interval of the main function.
 */
std::optional<std::pair<std::size_t, std::size_t>> findMainFunction(const std::string& source) {
    auto iLoc = findKeyword(source, "int");
    while(iLoc!=std::string::npos) {
        auto mLoc = iLoc + 3;
        while(mLoc<source.size() && isspace(source[mLoc])) {
            mLoc++;
        }
        if(mLoc==source.size()) {
            return std::nullopt;
        }
        if(mLoc!=iLoc+3 && source.substr(mLoc, 4)=="main") {
            auto pLoc = mLoc+4;
            while(pLoc<source.size() && std::isspace(source[pLoc])) {
                pLoc++;
            }
            if(pLoc>=source.size()) {
                return std::nullopt;
            }
            if(source[pLoc]=='(') {
                auto ed = pLoc;
                int ba = 0;
                while(ed<source.size()) {
                    if(source[ed]=='(')
                        ba++;
                    else if(source[ed]==')')
                        ba--;
                    if(ba==0) {
                        break;
                    }
                    ed++;
                }
                while(ed<source.size() && source[ed]!='{') {
                    ed++;
                }
                if(ed==source.size()) {
                    return std::nullopt;
                }
                while(ed<source.size()) {
                    if(source[ed]=='{')
                        ba++;
                    else if(source[ed]=='}')
                        ba--;
                    if(ba==0) {
                        break;
                    }
                    ed++;
                }
                if(ed==source.size()) {
                    return std::nullopt;
                }
                return std::make_pair(iLoc, ed);
            }
        }
        iLoc = findKeyword(source, "int", iLoc+1);
    }
    return std::nullopt;
}

void processSourceCpp(const std::string &source, const std::string &o_path) {
    // Remove the comments and string and character literals
    auto [purifiedSource, mapping] = removeCommentsAndStrs(source);
    
    // Find all class definitions
    auto klasses = findKlassDefinitions(purifiedSource);
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
    auto solutionKlassBody = purifiedSource.substr(solutionKlass.st, solutionKlass.ct);

    // Find all functions of the Solution class
    auto funcs = findFunctionDefinitions(solutionKlassBody);

    if(funcs.size()==0) {
        throw std::runtime_error("Cannot find any public function in the Solution class.");
    }

    // Let the user select one function as the entry function.
    Function func;
    if(funcs.size() == 1)
        func = funcs[0];
    else {
        std::vector<std::string> stred_funcs(funcs.size());
        for(int i=0;i<funcs.size();i++) {
            stred_funcs[i] = funcs[i].retType + ' ' + funcs[i].name + '(';
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

    // std::cout << "Generating IO code for function \"" << func.name << "\" ..." << std::endl;

    // Generate the main function
    std::string newMainFunc = generateMainFunction(func);
    std::string newSource = source + "\n";

    // Remove the original main function
    auto mainFuncLoc = findMainFunction(purifiedSource);
    if(mainFuncLoc.has_value()) {
        auto [st, ed] = *mainFuncLoc;
        newSource.erase(mapping[st], mapping[ed] - mapping[st] + 1);
    }

    // Insert the necessary headers.
    insertHeadersIfNotIncluded(newSource, {"string", "vector", "iostream", "stdexcept", "queue"});

    // Insert the generated main function at the end of the source.
    newSource.append(newMainFunc);

    // Write to file.
    writeStringToFile(o_path, newSource);
    std::wcout << "New code has been written to " << std::filesystem::canonical(o_path).c_str() << "." << std::endl;
}