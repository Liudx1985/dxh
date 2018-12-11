#include <iostream>
#include <algorithm>
#include <functional>
#include <cstdlib>

template <typename T, typename C = std::less<T> >
class heap
{

	C _lt;
	T *_base;
	int _size;
public:
	explicit heap() : _base(0x0), _size(0){

	}

	template <std::size_t N>
	explicit heap(T(&p)[N]) : _size(N){
		_base = new T[N];
		memcpy(_base, p, sizeof(T)* N);
		__make_heap(_lt);
	}

	const T &top() const{
		return _base[0];
	}

	bool empty() const {
		return _size <= 0;
	}

	T pop() {
		// assert not empty;
		T x = top(); // copy for return
		std::swap(_base[0], _base[--_size]);
		realloc(_base, sizeof(T)* _size);
		if (_size > 0){
			__heapify(_base, 0, _size, _lt);
		}
		return x;
	}
	// insert to heap.
	void push(T key) {
		realloc(_base, sizeof(T)* (++_size));
		_base[_size - 1] = key;
		heap_fix(_base, _size - 1);
	}
	
	// sort result stored in p
	void heap_sort(T *p) {		
		while (_size){
			*p++ = pop();
		}
	}

	void inplace_heap_sort(){
		struct GT {
			inline bool operator ()(T const &a, T const &b) const {
				return !(C()(a, b));
			}
		}_gt; // define NOT.(less<T>)
		int n = _size;
		__make_heap(_gt);
		while (n > 1) {
			std::swap(_base[0], _base[--n]);
			__heapify(_base, 0, n, _gt);
		}
	}


	__forceinline int parent(int k){
		return ((k + 1) >> 1) - 1;
	}

	__forceinline int left(int k){
		return (k << 1) + 1;
	}

	__forceinline int right(int k){
		return (k + 1) << 1;
	}
	
protected:
	template <typename Cmp>
	void __heapify(T *_ary, int i, int n, Cmp lt){
		while (true) { // loop all children of i, pop the smallest to top.
			int _left = left(i);
			int _right = right(i);
			int _min = i; // smallest
			if (_left < n && lt(_ary[_left], _ary[i])){
				_min = _left;
			}
			if (_right < n && lt(_ary[_right], _ary[_min])){
				_min = _right;
			}
			if (_min != i){
				std::swap(_ary[i], _ary[_min]); // i <->_min
				i = _min;
			}
			else {
				return;
			}
		}
	}
	// Cmp may be less<T> or greater<T>
	template <typename Cmp>
	void __make_heap(Cmp lt){
		for (int i = (_size - 1) >> 1; i >= 0; --i){
			__heapify(_base, i, _size, lt);
		}
	}

	void heap_fix(T *a, int i) {
		while (i > 0 && _lt(a[i], a[parent(i)])) {
			std::swap(a[i], a[parent(i)]);
			i = parent(i);
		}
	}
	//- 
	void decrease_key(T *a, int i, T k) {
		if (lt(k, a[i])) {
			a[i] = k;
			heap_fix(a, i, _lt);
		}
	}
	// DATA SEGMENT ---------------------------------------------

};

int main()
{
	int ar[] = {16, 4, 10, 14, 7, 9, 3, 2, 8, 1};
	heap<int, std::less<int> > a(ar);
	int k = a.pop();
	a.push(k);	
	a.inplace_heap_sort();
	// a.heap_sort(ar);
	while (!a.empty()) {
		std::cout << a.top() << std::endl;
		a.pop();
	}
	return 0;
}
