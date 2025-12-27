#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Solution {
public:
    void prefixesDivBy5(vector<string>& nums) {
        for(int i=0;i<nums.size();i++) {
            cout << nums[i] << endl;
        }
    }
};



int main(int argc, char* argv[]) {
	Solution solution;
	auto input_1d_string_=[](const string&s){vector<string>res;string temp;bool in_str=false;for(size_t i=0;i<s.size();++i){char c=s[i];if(c!='\\'){if(c=='"'){if(in_str){res.push_back(temp);temp.clear();}in_str=!in_str;}else if(in_str){temp.push_back(c);}continue;}if(++i>=s.size()||!in_str){throw runtime_error("Dangling backslash");}switch(s[i]){case '"':temp.push_back('"');break;case '\\':temp.push_back('\\');break;case '/':temp.push_back('/');break;case 'b':temp.push_back('\b');break;case 'f':temp.push_back('\f');break;case 'n':temp.push_back('\n');break;case 'r':temp.push_back('\r');break;case 't':temp.push_back('\t');break;default:throw runtime_error("Invalid escape");}}return res;};
	vector<string> nums;
	std::cout << "nums: ";
	std::string nums_str;
	std::getline(std::cin, nums_str);
	nums = input_1d_string_(nums_str);
	return 0;
}