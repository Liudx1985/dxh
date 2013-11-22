#ifndef _INC_STR_LCS_HPP_
#define _INC_STR_LCS_HPP_ 1

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> findRow0(string const &strA, string const &strB)
{
	int m = strA.size();
	int n = strB.size();
	vector<int> K0;
	vector<int> K1(n + 1, 0);
	//# in PDF, this lien is 1:n, It may be wrong
	for( int i = 0; i < m; ++i)
	{
		K0 = K1;
		for(int j = 0; j < n; ++j)
		{
			if (strA[i] == strB[j])
				K1[j+1] = K0[j] +1;
			else
				K1[j+1] = max( K1[j], K0[j+1]);
		}
	}
	vector<int> LL = K1;
	return LL;
}

// 最长匹配子字符串算法(Modified from http://blog.csdn.net/pkrobbie/article/details/1818477)
string H_LCS0(string const &strA, string const &strB)
{
	int m = strA.size();
	int n = strB.size();
	string C;
	if (m == 1)
	{
		size_t result = strB.find(strA[0]);
		if  ( result != strB.npos)

			//if strA[0] in strB:
			C = string(1,  strA[0]);
		else
			C.clear(); 
	}
	else
	{
		int i = int(m / 2);
		//#step3 

		string strA1i(strA.begin(),strA.begin()+i);
		vector<int> L1 = findRow0(strA1i, strB);


		string strAnip1(strA.rbegin(), strA.rend()-i);

		string strBn1(strB.rbegin(), strB.rend());
		vector<int> L2 = findRow0(strAnip1, strBn1);

		//#step4
		int M = 0;
		int k = 0;

		for ( int j = 0; j <= n; ++j)
		{
			int tmp = L1[j] + L2[n-j];
			if (tmp > M)
			{
				M = tmp;
				k = j;
			}
		}

		//#step 5
		string strA0i(strA.begin(), strA.begin()+i);
		string strB0k(strB.begin(), strB.begin()+k);
		string C1 = H_LCS0(strA0i, strB0k); 

		string strAim(strA.begin() + i, strA.end());
		string strBkn(strB.begin() + k, strB.end());
		string C2 = H_LCS0(strAim, strBkn);


		//#step 6
		C = C1;
		C.insert(C.end(), C2.begin(), C2.end());
	}

	return C;
}

#endif // LCS_INCLUDE

#if 0
#include "d:/workspace/dxh/TestUlt.h"
int main(int argc, char* argv[])
{
	string strA;
	string strB;
	printf("Input two string to find LCS:");
	getline(cin, strA);
	getline(cin, strB);
	TEST_BEGIN
		string C = H_LCS0(strA, strB);
		printf(&C[0]);
	TEST_END
	
	getchar();
	return 0;
}
#endif 
