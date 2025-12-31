#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;
// P4
class Solution {
public:
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        if (nums1.size() > nums2.size()) {
            return findMedianSortedArrays(nums2, nums1);
        }

        int m = nums1.size(), n = nums2.size();
        int left = 0, right = m;

        while (left <= right) {
            int partitionA = (left + right) / 2;
            int partitionB = (m + n + 1) / 2 - partitionA;

            int maxLeftA = (partitionA == 0) ? INT_MIN : nums1[partitionA - 1];
            int minRightA = (partitionA == m) ? INT_MAX : nums1[partitionA];
            int maxLeftB = (partitionB == 0) ? INT_MIN : nums2[partitionB - 1];
            int minRightB = (partitionB == n) ? INT_MAX : nums2[partitionB];

            if (maxLeftA <= minRightB && maxLeftB <= minRightA) {
                if ((m + n) % 2 == 0) {
                    return (max(maxLeftA, maxLeftB) +
                            min(minRightA, minRightB)) /
                           2.0;
                } else {
                    return max(maxLeftA, maxLeftB);
                }
            } else if (maxLeftA > minRightB) {
                right = partitionA - 1;
            } else {
                left = partitionA + 1;
            }
        }

        return 0.0;
    }
};
int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_1d_int_=[&](const string&str){vector<int>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return result;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);for(auto&it:res_str){result.push_back(stoi(it));}return result;};
	vector<int> nums1;
	cout << "nums1: ";
	string nums1_str;
	getline(std::cin, nums1_str);
	nums1 = input_1d_int_(nums1_str);
	vector<int> nums2;
	cout << "nums2: ";
	string nums2_str;
	getline(std::cin, nums2_str);
	nums2 = input_1d_int_(nums2_str);
	auto result_ = solution.findMedianSortedArrays(nums1, nums2);
	cout << "result: " << endl;
	cout << result_ << endl;
	return 0;
}