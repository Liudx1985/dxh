#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <bitset>
#include <windows.h>

#define LL __int64
#define MAXP 65536
int Eratosthenes(std::vector<int> &primes) {
  std::bitset<MAXP> notprime;
  primes.push_back(0);
   notprime[0] = true;
   for(int i = 2; i < MAXP; i++) {
         if( !notprime[i] ) {
              primes.push_back(i);
              //需要注意i*i超出整型后变成负数的问题，所以转化成 __int64
              for(LL j = (LL)i*i; j < MAXP; j += i) {
                   notprime[j] = true;
              }
         }
    }
    return primes.size();
  }

template <class Function, typename R = typename std::result_of<Function()>::type>
R time_call(Function&& f)
{
    __int64 begin = GetTickCount();
    R r =  f();
    std::cout << (GetTickCount() - begin) << "ms\n";
    return r;
}

int main()
{
  std::vector<int> primes;
  time_call ([&] {
        return Eratosthenes(primes);
  });
  std::cout << "\n";
  for (auto i : primes) {
    if (i > 1000) 
      break;
    std::cout << i << ",";
  }
  return 0;
}