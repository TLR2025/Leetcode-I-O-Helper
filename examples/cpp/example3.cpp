#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <queue>
using namespace std;
struct TreeNode {
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode() : val(0), left(nullptr), right(nullptr) {}
	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
	TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};
class Solution {
public:
    int maxLevelSum(TreeNode* root) {
        int maxSum = INT_MIN;
        int ans = 0, level = 0;

        queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            level++;
            int sumAtCurrentLevel = 0;
            // Iterate over all the nodes in the current level.
            for (int sz = q.size(); sz > 0; --sz) {
                TreeNode* node = q.front();
                q.pop();
                sumAtCurrentLevel += node->val;

                if (node->left != nullptr) {
                    q.push(node->left);
                }
                if (node->right != nullptr) {
                    q.push(node->right);
                }
            }

            if (maxSum < sumAtCurrentLevel) {
                maxSum = sumAtCurrentLevel;
                ans = level;
            }
        }

        return ans;
    }
};
int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_treenode_=[&](const string&str)->TreeNode* {vector<TreeNode*>treenode_arr;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos)return nullptr;vector<string>res_str=split_(str.substr(st+1,ed-st-1),',',false);if(res_str.size()==0)return nullptr;for(auto&it:res_str){if(it.find("null")!=string::npos){treenode_arr.push_back(nullptr);}else{TreeNode*nodep=new TreeNode(stoi(it));treenode_arr.push_back(nodep);}}queue<size_t>que;size_t ind=0;que.push(0);while(!que.empty()){size_t fr=que.front();que.pop();if(treenode_arr[fr]==nullptr)continue;++ind;if(ind>=treenode_arr.size())break;treenode_arr[fr]->left=treenode_arr[ind];que.push(ind);++ind;if(ind>=treenode_arr.size())break;treenode_arr[fr]->right=treenode_arr[ind];que.push(ind);}return treenode_arr[0];};
	TreeNode* root;
	cout << "root: ";
	string root_str;
	getline(std::cin, root_str);
	root = input_treenode_(root_str);
	auto result_ = solution.maxLevelSum(root);
	cout << "result: " << endl;
	cout << result_ << endl;
	return 0;
}