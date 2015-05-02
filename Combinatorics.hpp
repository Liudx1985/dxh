#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <ctime>
using namespace std;


template <typename T, typename F>
void enum_subset(vector<T> const & v, F f){
	int indicate = 0;
	while (indicate < pow(2, v.size())){ // C(1,N) + C(2,N) +.. + C(N, N) = 2 ^ (N)
		vector<T> r;
		for (int j = 0; j < v.size(); ++j){
			if (indicate & (1 << j)){
				r.insert(r.begin(),v[j]);
			}
		}
		f(r);
		indicate++;
	}
		
}


// 保存当前的一组索引，每迭代把最前的索引加一，循环归零，然后使下一个索引加一。
// 就像是数数目字般，个位数从0至9，满了就归零，然后十进位加一
template <typename T, typename F>
int product(vector<vector<T> > const &s, F f){
    vector<size_t> indices(s.size()); // 当前每个子数组的索引
    size_t i = 0;
    while (i < s.size()) {
        vector<T> r;
        for (size_t j = 0; j < s.size(); j++)
            r.push_back(s[j][indices[j]]);
        f(r);
        i = 0; // rest the changing vector
        while (i < s.size() && ++indices[i] == s[i].size())
            indices[i++] = 0;
    }
}

// Math enum permutations pair for cont. spec for vector, list.etc
// _PredBin is a functor implement the operator()(*it1,*it2) .
template <typename _Int, typename _PredBin>
int enum_permutations(_Int itBegin, _Int itEnd, _PredBin proc)
{
    int iCount = 0;
    _Int _itFirst = itBegin;
    while (_itFirst != itEnd)
    {
        _Int _itSecond = _itFirst;
        advance(_itSecond, 1);
        for (; _itSecond != itEnd; ++_itSecond)
        {
            if (!proc(*_itFirst, *_itSecond))
            {
                return iCount;
            }
            ++iCount;
        }
        ++_itFirst;
    }
    return iCount;
}

// enum combinations pair , ie the first and second passed to $proc has order
template <typename _Int, typename _PredBin>
inline
int enum_combinations(_Int itBegin, _Int itEnd, _PredBin proc)
{
    int iCount = 0;
    _Int _itFirst = itBegin;
    _Int _itSecond = itBegin;
    std::advance(_itSecond, 1);
    while (_itSecond != itEnd)
    {
        if (!proc(*_itFirst, *_itSecond))
        {
            break;
        }
        ++iCount;
        _itFirst = _itSecond++;
    }
    return iCount;
}
// TODO : Static Algorithms using template .

#ifdef _TEST_
int main()
{
	vector<vector<int> > v = {
        { 1, 2},
        { 3, 4},
        { 5, 6},
    };
    clock_t beg = clock();
    product(v, [&](vector<int> const &c){
            for (auto k : c)
                cout << k << ",";
            cout << endl;       
        }
    );
    clock_t end = clock();
}
#endif // def _TEST_
