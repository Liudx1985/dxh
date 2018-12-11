#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <initializer_list>
using namespace std;

// Acc
int acl(std::vector<int> const &bars, int l, int h){	
	int sum = 0;
	int top = std::min(bars[l], bars[h]);
	for (int i = l; i < h; ++i){
		if (top > bars[i])
			sum += top - bars[i];
	}
	return sum;
}
// sorted.
int Collect(std::vector<int> const &bars, std::vector<std::pair<int, int> > &p) {
	// get head and collect.
	if (p.size() < 2){
		return 0;
	}
	// split at max & second max;f
	int l = p[1].second; // pos
	int h = p[0].second;
	if (l > h) {
		std::swap(l, h);
	}

	int sum = acl(bars, l, h);
	std::vector<std::pair<int, int> > lp;
	std::vector<std::pair<int, int> > rp;
	// split. discard pos between (l,h)
	for (auto i = p.begin(); i != p.end(); ++i){
		if ((*i).second <= l){
			lp.push_back(*i);
		}
		if ((*i).second >= h) {
			rp.push_back(*i);
		}
	}
	
	sum += Collect(bars, lp);
	sum += Collect(bars, rp);
	return sum;
}

int Collect(std::vector<int> const &bars) {
	if (bars.size() < 3) { // MUST  >= 3
		return 0;
	}	

	std::vector<std::pair<int, int> > p(bars.size());
	int i = 0;
	for (auto &x : p){
		x.second = i;
		x.first = bars[i++];
	}
	std::sort(p.begin(), p.end(), [](std::pair<int, int> x, std::pair<int, int> y){
		return x.first > y.first;
	});
	return Collect(bars, p);
	
}
//
int main() {
	clock_t beg = clock();	
	cout << Collect({ 3, 1, 2}) << endl;
	cout << Collect({ 0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1 }) << endl;
	cout << "clock " << clock() - beg << endl;
}