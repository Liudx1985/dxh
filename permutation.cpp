#include <vector>
#include <iostream>
#include <algorithm>
#include "P:/WorkSpace/Dxh/combinatric.hpp"
using namespace std;

typedef std::vector<int>  vec;
typedef std::vector<vec> vvec;

//you can use std::next_permutation instead of this.

/*def perm(lst):
	if len(lst) <= 1:
        return [lst]
    r = []
    for i in range(len(lst)):
        s = lst[:i] + lst[i+1:]
        p = perm(s)
        for x in p:
            r.append(lst[i:i+1] + x)
    return r
	-
permutation (ls@(x:xs)) 
	| 0 == length xs = [[x]]
	| otherwise = foldr (\a r -> r ++ (map (a:) $permutation [y | y<- ls, y /= a]))
		[] ls
*/

vvec perm(vec const &v){
	vvec r;
	if (v.size() == 1) {
		r.push_back({v.front()});
		return r;
	}

	for (int i = 0; i < v.size(); ++i){
		vec vt = v;
		vt.erase(vt.begin() + i);
		vvec rT = perm(vt);
		for (auto x : rT){
			x.push_back(v[i]);
			r.push_back(x);
		}
	}
	return r;
}


int main (){
	vec a = {1,2,3};	
	for (auto k : perm(a)){
		for (auto i : k)
		 	cout << i << " ";
		 cout << endl;
	};	
}