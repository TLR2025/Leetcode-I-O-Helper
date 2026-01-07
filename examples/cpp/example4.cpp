#include <queue>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* dummyHead = new ListNode(0);
        ListNode* curr = dummyHead;
        int carry = 0;
        while (l1 != NULL || l2 != NULL || carry != 0) {
            int x = l1 ? l1->val : 0;
            int y = l2 ? l2->val : 0;
            int sum = carry + x + y;
            carry = sum / 10;
            curr->next = new ListNode(sum % 10);
            curr = curr->next;
            l1 = l1 ? l1->next : nullptr;
            l2 = l2 ? l2->next : nullptr;
        }
        ListNode* result = dummyHead->next;
        delete dummyHead;  // Freeing the memory allocated for dummyHead
        return result;
    }
};

int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_1d_int_=[&](const string&str){vector<int>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return result;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);for(auto&it:res_str){result.push_back(stoi(it));}return result;};
	auto input_listnode_=[&](const string&str)->ListNode* {ListNode*root=new ListNode();ListNode*now=root;vector<int>arr=input_1d_int_(str);if(arr.size()==0)return nullptr;for(size_t i=0;i<arr.size()-1;i++)now->val=arr[i],now->next=new ListNode(),now=now->next;now->val=arr[arr.size()-1];return root;};
	ListNode* l1;
	cout << "l1: ";
	string l1_str;
	getline(std::cin, l1_str);
	l1 = input_listnode_(l1_str);
	ListNode* l2;
	cout << "l2: ";
	string l2_str;
	getline(std::cin, l2_str);
	l2 = input_listnode_(l2_str);
	auto result_ = solution.addTwoNumbers(l1, l2);
	cout << "result: " << endl;
	if(result_==nullptr)cout<<"[]"<<endl;else{cout<<"["<<result_->val;result_=result_->next;while(result_!=nullptr){cout<<", "<<result_->val;result_=result_->next;}cout<<"]";}
	return 0;
}