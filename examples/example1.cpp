// Leetcode P3625
#include <bits/stdc++.h>
using namespace std;
typedef vector<int> Point;
typedef pair<Point*, Point*> Side;
struct SdWithB {
	Side side;
	long long b; // llround(real_b * K)
};
const double INF = 2e3+10;
const double K = 2e8;
class Solution {
private:
	double slope(const Side& side) {
		// Calcular el pendiente.
		if(side.first->at(0)<side.second->at(0)) {
			return 1.0*(side.second->at(1)-side.first->at(1))/(side.second->at(0)-side.first->at(0));
		} else if(side.first->at(0)>side.second->at(0)) {
			return 1.0*(side.first->at(1)-side.second->at(1))/(side.first->at(0)-side.second->at(0));
		} else {
			return INF;
		}
	}
	double yoffs(const Side& side) {
		// Calcular el b.
		if(side.first->at(0)!=side.second->at(0)) {
			return 1.0*side.first->at(1)-side.first->at(0)*slope(side);
		} else {
			return INT_MIN;
		}
	}

	const int order[3][3] = {
		{0,1,2},
		{0,2,1},
		{1,2,0},
	};
	array<Point, 3> fourthPoint(const vector<Point>& points) {
		// 6 paralelogramos posibles
		array<Point, 3> res;
		for(int i=0;i<3;i++) {
			res[i] = {
				points[order[i][0]][0] + points[order[i][1]][0] - points[order[i][2]][0],
				points[order[i][0]][1] + points[order[i][1]][1] - points[order[i][2]][1]
			};
		}
		return res;
	}
	// 判断3点是否共线
	bool sameLine(Point& a, Point& b, Point& c) {
		return ((a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]))==0;
	}
	
	// 统计平行四边形的数量
// 以下代码是瓶颈，常数巨大且复杂度约为O(n^3 * log(n))，会TLE很多点，下面有更快速的实现。
//	int countRepeat(vector<vector<int>>& points) {
//		int result = 0;
//		// The set of the points, but with the original order.
//		set<pair<Point, int>> pst;
//		for(int i=0;i<points.size();i++) {
//			pst.insert({points[i],i});
//		}
//		for(int i=0;i<points.size();i++) {
//			for(int j=i+1;j<points.size();j++) {
//				for(int k=j+1;k<points.size();k++) {
//					// 特判3点是否共线（若共线则不能统计，否则就多减了）
//					if(sameLine(points[i], points[j], points[k]))
//						continue;
//					
//					array<Point, 3> posiblePoints = fourthPoint({points[i], points[j], points[k]});
//					for(int h=0;h<3;h++) {
//						auto point = pst.lower_bound({posiblePoints[h], 0});
//						if((point == pst.end()) || (point->first != posiblePoints[h]))
//							continue;
//						// 找到一个重复的paralelogramo，检查是否是第k个点后面的点（如果不是，表明之前统计过）
//						if(point->second>k)
//							result++;
//					}
//				}
//			}
//		}
//		return result;
//	}
	
	// C(n, 2)
	int combination2(int n) {
		return n*(n-1)/2;
	}

	// 统计平行四边形的数量
	int countRepeat(vector<vector<int>>& points) {
		/*
		定理：若两条线段中点相同，则这4个点（2条线段的所有顶点）可以构成一个平行四边形
		因此可以双重循环，统计所有“两点”中每个中点被标记的次数，如果有n个线段共享同一个中点
		则这n个线段中任取两个都可以构成平行四边形，即贡献C(n, 2)个平行四边形
		至于计算中点，我们没必要计算真正的中点，而是计算中点的两倍，即简单的A+B
		*/
		int result = 0;
		map<Point, pair<int, map<pair<long long,long long>, int>>> puntosMedios; //定义成这样是为了处理退化情况。除了标记数量以外还要统计每一个中点上对于每种可能得直线，有多少线段，防止以后统计到同直线的。
		for(int i=0;i<points.size();i++) {
			for(int j=i+1;j<points.size();j++) {
				Side side = {&points[i], &points[j]};
				double k = slope(side);
				double b = yoffs(side);
				long long lk = llround(k * K);
				long long lb = llround(b * K);
				Point puntoMedio = {points[i][0]+points[j][0], points[i][1]+points[j][1]};
				auto pr = make_pair(lk, lb);
				(puntosMedios[puntoMedio].second)[pr]++;
				puntosMedios[puntoMedio].first++;
			}
		}
		for(auto i=puntosMedios.begin();i!=puntosMedios.end();i++) {
			result += combination2(i->second.first);
			// 要减去所有退化情况，即几个线段共直线且中点相同的情况
			for(auto j=i->second.second.begin();j!=i->second.second.end();j++) {
				result -= combination2(j->second);
			}
		}
		return result;
	}
public:
	int countTrapezoids(vector<vector<int>>& points) {
		// 计算平行四边形的数量（因为根据以下算法，所有平行四边形都会被统计两次）
		int repeatCnt = countRepeat(points);
		// 每个斜率的边数组，mp[key] 表示斜率（的LL映射）为k的所有线段（边）
		// 同时我们记录每条边的"b"值，代表它的y偏移量，目的是找到相同的b值（且斜率相同）的线段
		map<long long, vector<SdWithB>> mp;
		// cnt统计所有梯形的数量
		int cnt = 0;
		// 双重循环遍历所有可能的线段
		for(int i=0;i<points.size();i++) {
			for(int j=i+1;j<points.size();j++) {
				SdWithB sdWithB;
				Side side = {&(points[i]), &(points[j])};
				// pendiente（斜率k）
				double sl = slope(side);
				// 把它映射为long long类型方便处理精度问题
				long long lsl = llround(sl * K);
				// 记录与y轴的交点的y坐标(y=kx+b中b的值)（的LL映射） 
				long long b;
				if(lsl > (2e3+5)*K) {
					lsl = LLONG_MAX;
					b = (LLONG_MIN>>1) + points[i][0]; // 防止所有垂直的线都被判断为互相共线，要把x坐标当偏移量加上去
				} else {
					b = llround((points[i][1] - points[i][0] * sl) * K);
				}
				sdWithB = {side, b};
				mp[lsl].push_back(sdWithB);
			}
		}
		// 遍历每种斜率的情况
		for(auto iter=mp.begin();iter!=mp.end();iter++) {
			auto psides = &(iter->second); // psides: the pointer of the the sides(with value b) array
			// 按照偏移量的值对当前斜率中的所有线段进行排序，为了之后统计每“层”（即一些共线的线段所位于的直线）里的线段数量
			sort(psides->begin(), psides->end(), [](const SdWithB& a, const SdWithB& b){
				return a.b < b.b;
			});
			// 每“层”的线段数量
			vector<int> numLines={1};
			for(int i=1;i<psides->size();i++) {
				if(psides->at(i).b == psides->at(i-1).b) {
					// b值和上一个相同：把numLines的最后一个值+1
					numLines[numLines.size()-1]++;
				} else {
					// b值不同：把 1 push进去
					numLines.push_back(1);
				}
			}
			// 每“层”的线段数量的数组的前缀和，方便之后计算组合数
			vector<int> sumNumLines = {numLines[0]};
			sumNumLines.resize(numLines.size());
			for(int i=1;i<numLines.size();i++) {
				sumNumLines[i] = sumNumLines[i-1] + numLines[i];
			}
			// 统计组合数量
			for(int i=1;i<numLines.size();i++) {
				cnt += sumNumLines[i-1] * numLines[i];
			}
		}
		// return 真正的数量（即cnt减去计算了2次的的平行四边形的数量）
		return cnt - repeatCnt;
	}
};
/*
5
-3 2
3 0
2 3
3 2
2 -3
*/

int main(int argc, char* argv[]) {
	Solution solution;
	auto split_=[](const string&str,char c,bool allowEmpty){string t="";std::vector<string>result;for(size_t i=0;i<str.size();i++){if(str[i]!=c)t.push_back(str[i]);else{if(allowEmpty||t!="")result.push_back(t);t="";}}if(allowEmpty||t!=""){result.push_back(t);}return result;};
	auto input_2d_int_=[&](const string&str){vector<vector<int>>result;auto st=str.find('[');auto ed=str.rfind(']');if(st==string::npos||ed==string::npos||st>=ed)return result;for(size_t i=st+1;i<ed;++i){if(str[i]=='['){auto rpos=str.find(']',i);if(rpos==string::npos)break;string row=str.substr(i+1,rpos-i-1);auto row_data=split_(row,',',false);result.emplace_back();for(auto&num_str:row_data){result.back().push_back(stoi(num_str));}i=rpos;}}return result;};
	vector<vector<int>> points;
	cout << "points: ";
	string points_str;
	getline(std::cin, points_str);
	points = input_2d_int_(points_str);
	auto result_ = solution.countTrapezoids(points);
	cout << "result: " << endl;	cout << result_ << endl;
	return 0;
}