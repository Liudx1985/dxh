/*
 *tail recursion
 */
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "dxh/testult.h"
using namespace std;

int64_t fib(int64_t x){
    if (x > 1)
        return fib(x - 1) + fib(x - 2);
    return 1;
}
// Tail-recurse fib
static int64_t fibnocci(int64_t n)
{
    struct fib_iter
    {
        int64_t f0_,f1_,cnt_;
        fib_iter(int64_t f0 = 1, int64_t f1 = 1) :f0_(f0), f1_(f1), cnt_(0)
        {}
        int64_t operator ()(int64_t count){
            cnt_ = count;
            if (0 >= --cnt_){
                return f1_;
            }
            int64_t tmp = f1_;
            f1_ = f0_ + f1_;
            f0_ = tmp; // f0 := f1
            return operator()(cnt_);
        }
    }  fib_i;
    return fib_i(n);
}


// f(0) = 1
// f (x) = f(x-1)+f(x-2)
struct fibonacci{
    static int eval(int x){
        return fibonacci()(fibonacci(), x);
    }

    template <typename T>
    int operator ()(T f, int x){
        //std::cout << __PRETTY_FUNCTION__<< x << std::endl;
        if (x > 1) return f(f, x - 1) + f(f, x - 2);
        return 1;
    }
};

int64_t fib_optimized(int n) {
    if (n < 2) return 1;
    int64_t f0 = 1, f1 = 1;
    for (int i = 1; i < n; ++i){
        int64_t f = f0 + f1;
        f0 = f1;
        f1 = f;
    }
    return f1;
}

int main()
{
    int n = 1;
    cin >> n;
    cout << time_call(fibonacci::eval, n)  << endl;
    cout << time_call(fib_optimized, n)<< endl;
    cout << time_call(fibnocci, n) ;
   return 0;
}
