#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;
// Common solution.

const int QueenNum = 8;  //定义皇后数量
volatile int g = 0; // 解数目
void backtrack(int x);
void permutation();
// Main entry point.
int main()
{
    backtrack(0);
    cout<< "Answer for \"" <<QueenNum<< " Queen\" : " << g << " found!" << endl;
    permutation();
    system("pause");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// backtrack algo
const int mc = 2*QueenNum-1; // 对角线数目
bool my[QueenNum], md1[mc], md2[mc];   // 初始值都是 false
                                // x這條線可以不用檢查了
char solution[QueenNum]={0};
void backtrack(int x)   // 每次都換一排格子
{
    // it's a solution
    if (x == QueenNum)
    {
        for (auto k : solution) {
            cout << int(k) << " ";
        }
        cout << endl;
        ++g;
        return;
    }

    // 分別放置皇后在每一格，並各自遞迴下去。
    for (int y=0; y < QueenNum; ++y)
    {
        int d1 = (x+y) % mc, d2 = (x-y+mc) % mc;
        if (!my[y] && !md1[d1] && !md2[d2])
        {
            // 這隻皇后佔據了四條線，記得標記起來。
            my[y] = md1[d1] = md2[d2] = true;
            solution[x] = y;
            backtrack(x+1);
            // 遞迴結束，回復到原本的樣子，要記得取消標記。
            my[y] = md1[d1] = md2[d2] = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Optimized algorithm for n-QUeens Puzzle.

bool all_safe(int a[QueenNum], int Col = QueenNum) {
    for (int i = 0; i < Col; ++i) {
        for (int j = i + 1; j < Col; ++j) {
            if (a[i] == a[j] || (abs(a[i] - a[j]) == (j - i)))
            {
                return false;
            }
        }
    }
    return true;
}

//permutation takes o(N!))
void permutation() {
    int a[QueenNum];
	for (int i = 0; i < QueenNum; ++i)
		a[i] = i;

    g = 0;
    do {
        if (all_safe(a)) {
            for_each(a, a + QueenNum, [](int x){
                cout << x << " ";
            });
            cout << ";\n";
            ++g;
        }
    }
    while (next_permutation(a, a + QueenNum));
    cout<< "Answer for \"" <<QueenNum<< " Queen\" : " << g << " found!" << endl;
}
