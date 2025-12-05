#include <bits/stdc++.h>
using namespace std;
class Solution {
public:
	double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
		int m = nums1.size();
		int n = nums2.size();
		int o = m+n;
		vector<int> merged(o);
		for(int i=0,i1=0,i2=0;i<o;i++){
			if(i1>=m)
				merged[i] = nums2[i2++];
			else if(i2>=n)
				merged[i] = nums1[i1++];
			else if(nums1[i1]<nums2[i2])
				merged[i] = nums1[i1++];
			else
				merged[i] = nums2[i2++];
		}
		int mid = o/2;
		if(o&1)	return merged[mid];
		return (merged[mid]+merged[mid-1])/2.0;
	}
};