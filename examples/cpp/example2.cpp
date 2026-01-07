#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;
// P1
class Solution {
public:
    vector<int> twoSum(vector<int> &nums, int target) {
        unordered_map<int, int> hash;
        for (int i = 0; i < nums.size(); ++i) {
            int complement = target - nums[i];
            if (hash.find(complement) != hash.end()) {
                return {hash[complement], i};
            }
            hash[nums[i]] = i;
        }
        // Return an empty vector if no solution is found
        return {};
    }
};
int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_1d_int_=[&](const string&str){vector<int>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return result;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);for(auto&it:res_str){result.push_back(stoi(it));}return result;};
	vector<int> nums;
	cout << "nums: ";
	string nums_str;
	getline(std::cin, nums_str);
	nums = input_1d_int_(nums_str);
	int target;
	cout << "target: ";
	cin >> target;
	auto result_ = solution.twoSum(nums, target);
	cout << "result: " << endl;
	if(result_.size()==0)cout<<"[]"<<endl;else{cout<<"["<<result_[0];for(size_t i=1;i<result_.size();i++){cout<<", "<<result_[i];}cout<<"]"<<endl;}
	return 0;
}