//memory_cache.cxx
#include <map>
#include <iostream>
#include <functional>
#include "d:/workspace/dxh/testult.h"
//#include "d:/workspace/dxh/bind_mem_fun.h"
using namespace std;

// traits declare for memory_cache.
// L1 cache in memory.
// @K : the map key-type
// @T: the map value_type
// @F : the type of function load from outside cache.
template <typename K, typename T, typename F = T *(*)(K const &)>
class memory_cache
{
	F _f;	
public:
	memory_cache(F f) :_f(f) {
	}

	~memory_cache() {

	}

	T *get(K id) {
		T *ptr_ret = 0;
		typename std::map<K, T*>::iterator itf = _cache_pool.find(id);
		if (itf != _cache_pool.end()) { /*found ,just return*/
			ptr_ret = (*itf).second;
		}
		else { /*not found*/
			// load from L2 cache.
			ptr_ret = _f(id);
			if (ptr_ret) {
				_cache_pool.insert(std::make_pair(id, ptr_ret));
				// insert into pool
			}
		}
		return ptr_ret;
	}
	/* data */
	std::map<K, T*> _cache_pool;
};


int *Gen_random(int const &) {
	return new int(Rand(0, 100));
}

// Test with Class adaptor
class CRandpool
{
public:
	int *Gen_random(int const &) {
		return new int(Rand(0, 100));
	}
};

int main()
{
	memory_cache<int, int> xmac(Gen_random);
	xmac.get(0);
	xmac.get(1);
	xmac.get(3);
	xmac.get(2);
	xmac.get(0);
	for (std::map<int, int *> ::iterator it = xmac._cache_pool.begin();
		it != xmac._cache_pool.end(); ++it) {
			std::pair <int, int*> x = *it;
 		cout  << (x.first) << ":"	<< (*(x.second)) << "\n";
 		delete x.second;
 	}
 	using namespace std::placeholders;  // for _1, _2, _3...
	CRandpool a;
	//typedef bind_mem_fun_t<int *(CRandpool::*)(int const&)> class_adaptor_type;
	//memory_cache<int, int, class_adaptor_type> xcx(bind_mem_fun(&a, &CRandpool::Gen_random));
	auto fn = std::bind(&CRandpool::Gen_random, &a, _1);	
	memory_cache<int, int, decltype(fn)> xcx(fn);
	cout << "Class adaptor.\n";
	xcx.get(0);
	xcx.get(1);
	xcx.get(3);
	xcx.get(2);
	xcx.get(0);
	for (std::map<int, int *> ::iterator it = xcx._cache_pool.begin();
		it != xcx._cache_pool.end(); ++it) {
			std::pair <int, int*> x = *it;
			cout  << (x.first) << ":"	<< (*(x.second)) << "\n";
			delete x.second;
	}

	return cin.get();
}
