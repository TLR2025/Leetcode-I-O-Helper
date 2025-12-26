#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Solution {
public:
    void prefixesDivBy5(vector<vector<string>>& nums) {
        for(int i=0;i<nums.size();i++) {
            for(int j=0;j<nums.size();j++) {
                cout << nums[i][j] << " ";
            }
            cout << endl;
        }
        
    }
};
int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const std::string&str,char c,bool allowEmpty){std::string t="";std::vector<std::string>result;for(int i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_2d_string_=[&](const string&str){vector<vector<string>>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos||st>=ed)return result;for(size_t i=st+1;i<ed;++i){if(str[i]=='['){auto rpos=str.find(']',i);if(rpos==string::npos)break;string row=str.substr(i+1,rpos-i-1);auto row_data=split_(row,',',false);result.emplace_back();for(auto&res_str:row_data){result.back().push_back(res_str.substr(res_str.find('\"')+1,res_str.rfind('\"')-res_str.find('\"')-1));}i=rpos;}}return result;};
	vector<vector<string>> nums;
	std::cout << "nums: ";
	std::string nums_str;
	std::getline(std::cin, nums_str);
	nums = input_2d_string_(nums_str);
	return 0;
}