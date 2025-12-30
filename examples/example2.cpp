#include <bits/stdc++.h>
using namespace std;
class Solution {
public:
    int test(vector<string>&arr) {
        
    }
};

int main(int argc, char* argv[]) {
	Solution solution;
	auto input_1d_string_=[](const string&s){vector<string>res;string temp;bool in_str=false;for(size_t i=0;i<s.size();++i){char c=s[i];if(c!='\\'){if(c=='"'){if(in_str){res.push_back(temp);temp.clear();}in_str=!in_str;}else if(in_str){temp.push_back(c);}continue;}if(++i>=s.size()||!in_str){throw runtime_error("Dangling backslash");}switch(s[i]){case '"':temp.push_back('"');break;case '\\':temp.push_back('\\');break;case '/':temp.push_back('/');break;case 'b':temp.push_back('\b');break;case 'f':temp.push_back('\f');break;case 'n':temp.push_back('\n');break;case 'r':temp.push_back('\r');break;case 't':temp.push_back('\t');break;default:throw runtime_error("Invalid escape");}}return res;};
	vector<string> arr;
	cout << "arr: ";
	string arr_str;
	getline(std::cin, arr_str);
	arr = input_1d_string_(arr_str);
	auto result_ = solution.test(arr);
	cout << "result: " << endl;	cout << result_ << endl;
	return 0;
}