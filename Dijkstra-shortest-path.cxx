#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <queue>
#include <unordered_set>
#include <list>
#include "p:/workspace/dxh/matrixT.h"
#include "P:/WorkSpace/Dxh/combinatric.hpp"
#include <Windows.h>

#include "omp.h"

using namespace std;
using namespace ns_math;
const size_t infit = 0xFFFF;

bool Dijkstra(CMatrix<size_t> &m, std::vector<size_t> &d, int s){
	size_t i, j;
	m.GetSize(i, j);
	if (i != j || i < 2){
		return false;
	}
	
	std::set<int> S; // the shortest distances to which are not decided
	d.resize(j);
	for (i = 0; i < j; ++i){
		d[i] = m(s, i);
		S.insert(i);
	}
	d[s] = 0;
	S.erase(s);
	while (!S.empty()) {
		int min_dv = infit;
		int v = 0;
		// Find v in 𝑆 which has the smallest d(v), and erase it from 𝑆 
		for (auto i : S){
			if (d[i] < min_dv){
				v = i;
				min_dv = d[i];
			}
		} 
		S.erase(v); // remove v from S
		for (int u = 0; u < j; ++u){ // For each edge v → u of cost c:
			size_t c = m(v, u); // 
			if (d[u] > d[v] + c){
				d[u] = (d[v] + c); // Relax this edge: → u => → v → u
			}
		}
	}	
}
#if 0
// Bellman ford algorithm 
bool Bellman_Ford(CMatrix<size_t> &m, std::vector<size_t> &d, int s){
	// Initialize d𝑠 = 0 and Dv	 = ∞ for all v ≠ 𝑠 
	size_t i, j;
	m.GetSize(i, j);
	if (i != j || i < 2){
		return false;
	}
	d.resize(j);
	for (i = 0; i < j; ++i){
		d[i] = infit;
	}
	d[s] = 0;

#pragma omp parallel for shared(m, d) schedule(dynamic)
	for (int k = 1; k < j; ++k) 
	{
		// For each edge u → v of cost c: dv = min (dv, du + c)
		for (int u = 0; u < j; ++u)
		{
			for (int v = 0; v < j; ++v) 
			{
				size_t c = m(u, v);
				d[v] = min(d[u] + c, d[v]);
			}
		}
	}
	return 1;
	for (int u = 0; u < j; ++u){ // For each edge u → v of cost c: dv = min dv, du + c
		for (int v = 0; v < j; ++v){
			size_t c = m(u, v);
			if (d[v] > d[u] + c) // then the graph contains a negative-weight cycle 
				return false;
		}
	}
}
#endif // slow then edge visit-loop
// reduce route.
/* p, s, d
(path@x:xs) | x == src = path
			| else for each 
*/

// call f when find the final route.
void get_path(CMatrix<size_t> &m, const std::vector<size_t> &t, size_t src, size_t dst,
	std::list<std::vector<size_t> >  &routes)
{
	routes.clear();
	std::vector<size_t> route = { dst };
	routes.push_back({ route });
	size_t i, j;
	m.GetSize(i, j);
	// GET all shortest path backtrace.
	for (std::list<std::vector<size_t>>::iterator it = routes.begin();
		it != routes.end(); ) {
		auto &route = *it;
		auto head = route.front();
		if (head == src){
			++it;
			continue;
		}

		for (i = 0; i < j; ++i) {
			//GET u| d(u) + w(u, v) = d(v)
			if (t[i] + m(i, head) == t[head]) {
				auto nx = route; // copy & inset. spawn
				nx.insert(nx.begin(), i);
				routes.push_back(nx);
			}
		}
		it = routes.erase(it);
	}
	/*for (int i = 0; i < j; ++i){
		auto k = routes[i];
		if (k.back() != i)
			k.push(i);
		while (!k.empty()){
			cout << k.front() << "\t";
			k.pop();
		}
		cout << "\n";
	}*/
}

int main1() 
{
	const int n = 60; // has n * n elements. square placed.
	CMatrix<size_t> m(n * n, n * n);
	m.Fill(infit);
	for (int r = 0; r < n; ++r) {

		for (int c = 0; c < n; ++c) {
			if (c > 0){
				m(n * r + c - 1, n * r + c) = 1;
				m(n * r + c, n * r + c - 1) = 1;
			}
			if (r > 0) {
				m(n * (r - 1) + c, n * r + c) = 1;
				m(n * r + c, n * (r - 1) + c) = 1;
			}
		}
	}
	/*m(7, 12) = infit;
	m(11, 12) = infit;
	m(13, 12) = infit*/;

	std::list<std::vector<size_t> >  routes;

	std::vector<size_t> d;
	/*Dijkstra(m, d, 0);
	for (auto i : d){
		cout << i << "\t";
	}
	cout << "\n";*/
	//---
	__int64 begin = GetTickCount();
	Dijkstra(m, d, 0);
	cout << (GetTickCount() - begin) << "ms\n";
	
	get_path(m, d, 0, 74, routes);
	for (auto i : routes){
		for (auto r : i)
		{
			cout << r << "\t";
		}
		cout << "\n";
	}
	
	return 0;
}