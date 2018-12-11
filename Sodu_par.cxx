#include <cstdio>
#include <iostream>
#include <map>
#include <cstring>
#include <bitset>
#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <cmath>
#include <thread>
#include <stack>
#include <utility>
#include <vector>
#include <future>
#include <thread>
#include "../Dxh/combinatric.hpp"

using namespace std;

template<int N>
class Sodu
{
public:
    typedef short (Board)[N][N];

    Sodu(short *c = 0)
    {
        memset(_board, 0, sizeof(_board));
        if (c)
        {
            memcpy(&_board, c, sizeof(_board));
        }

        for (int i = 0; i < N; ++i) // set empty pos to -1
        {
            for (int j = 0; j < N; ++j)
            {
                --_board[i][j];
            }
        }
    }
    typedef std::bitset<N> Flags;
    void print()
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                cout <<_board[i][j] + 1 << " ";
            }
            cout <<endl;
        }
        cout <<endl;
    }

    // brute-force search.only work for simple mode.
    bool solve()
    {
        int iRec = initUpdateBoard(_board);
        if (iRec == 1)
        {
            return true;
        }
        if (iRec == -1)
        {
            return false;
        }

        vector<vector<int> > vecRec;
        std::vector<std::pair<int, int> > vecPos;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (_board[i][j] >= 0)
                {
                    continue;
                }
                vecPos.push_back(make_pair(i, j));
                std::bitset<N> flag = getAvl(_board, i, j);

                vector<int> vecIdle;
                for (int k = 0; k < N; ++k)
                {
                    if (flag[k])
                        vecIdle.push_back(k);
                }
                vecRec.push_back(vecIdle);
            }
        }

		struct check_loop{
			Sodu *pThis;
			std::vector<std::pair<int, int> > &vecPos;
			check_loop(Sodu *pThis_,
				std::vector<std::pair<int, int> > &vecPos_
				) : vecPos(vecPos_), pThis(pThis_){}
			inline bool operator ()(vector<int> &r)
			{
				for (int k = 0; k < r.size(); ++k)
				{
					int i = vecPos[k].first;
					int j = vecPos[k].second;
					pThis->_board[i][j] = r[k];
				}
				// check.
				return (pThis->validBoard(pThis->_board));
			}

		};
		check_loop loopfn(this, vecPos);
		return product(vecRec, loopfn);
    }

    bool solve_par(){
         return solve2(_board);
    }

    // x..
    bool solve2(Board &board){
        std::pair<int, int> prMinPos; // get the min Pos
        int iRec = initUpdateBoard(board, &prMinPos);
        if (iRec == 1)
        {
            return true;
        }
        if (iRec == -1)
        {
            return false;
        }
        // get min pos
        std::bitset<N> flag = getAvl(board, prMinPos.first, prMinPos.second);
        int cx = flag.count();
        std::vector<std::future<bool> > ft(cx);
        // async test each.
        int i = 0;
        Board *pSubBoards = new Board[cx]; // new sub branchs.
        for (int k = 0; k < N; ++k)
        {
            if (flag[k])
            {
                // copy board,set to-x.
                Board &subBoard = pSubBoards[i];
                memcpy(&subBoard, &board, sizeof(Board));
                subBoard[prMinPos.first][prMinPos.second] = short(k);
                // async
                ft[i++] = std::async(std::launch::async, solve2, (this), std::ref(subBoard)); // @!!!!
            }
        }
        // get result
        bool bGot = false;
        for (i = 0; i < cx; ++i)
        {
           Board &_ret = pSubBoards[i];
           if (ft[i].get()){
                // copy back.
                memcpy(&board, &_ret, sizeof(Board));
                bGot = true;
                break;
           }
        }
        delete []pSubBoards;
        // if all
        return bGot;
    }

	Board _board;
protected:
    // base 0 index.
    inline Flags getAvl(Board &board, int i, int j)
    {
        Flags bst;
		bst.flip();
        int Q = (int)sqrtf(N);
        // n=-bock
		//#pragma omp parallel for shared(board)
		for (int m = i / Q * Q; m != (i + Q) / Q * Q; ++m)
        {
			if (i != m)
			for (int n = j / Q * Q; n != (j + Q) / Q * Q; ++n)
            {
				if (j == n)continue;
                if (board[m][n] >= 0)
                {
                    bst.reset(board[m][n]);
                }
            }
        }
		//#pragma omp parallel for shared(bst, i, j, board)
        for (int n = 0; n < N; ++n)
        {
            if (n != i && board[n][j] >= 0)
            {
                bst.reset(board[n][j]);
            }
            if (n != j && board[i][n] >= 0)
            {
                bst.reset(board[i][n]);
            }
        }
        return bst;
    };

    bool validBoard(short board[N][N]){
		bool bOK = true;
		//#pragma omp parallel for shared(bOK, board)
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                std::bitset<N> flag = getAvl(board, i,  j);
                if (flag.count() != 1
                    || board[i][j] != log2(flag.to_ulong()))
                {
                    bOK = false;
                }


            }
        }
        return bOK;
    }

    // return 1: finished; 0 :no error; -1:error.
    int initUpdateBoard(Board &board, std::pair<int, int> *pMinPos = 0)
    {
		//init flags.
		int iMinAvl = N;
		std::pair<int, int> prMinPos(N, N); // get the min Pos
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				if (board[i][j] >= 0)
				{
					continue;
				}
				bool bUpdate = false;
				Flags flag = getAvl(board, i, j);
				if (1 < flag.count() && flag.count() < iMinAvl) {
                    iMinAvl = flag.count();
                    prMinPos = make_pair(i, j);
				}
				switch (flag.count())
				{
				default:
					break;
				case 0:
					return -1;
				case 1:
					{
						board[i][j] = log2(flag.to_ulong());
						bUpdate = true;
						break;
					}
				}
				if (bUpdate){
					i = j = -1;
					iMinAvl = N;
					break;
				}
			}
		}
		if (pMinPos)
            *pMinPos = prMinPos;
		return iMinAvl == N ?  1 : 0;
    }
};

#define _X9 // 9X9 mode. default is 4X4 test mode
#define Level 3// 1: simple;2:hard;3:very hard

int main()
{
	clock_t beg = clock();
#ifdef _X9
    short x[] =
    {
#if (Level == 1) // simple
        0,0,0,0,4,0,0,0,0,
        0,0,3,2,6,5,7,0,0,
        0,5,6,7,0,3,1,9,0,
        0,2,7,0,0,0,4,1,0,
        3,9,0,0,0,0,0,2,8,
        0,6,8,0,0,0,3,7,0,
        0,3,9,4,0,2,8,6,0,
        0,0,1,8,5,9,2,0,0,
        0,0,0,0,3,0,0,0,0,
#endif
#if (Level == 2) // hard
        0,0,0,0,2,0,3,0,6,
        0,0,2,1,0,0,0,0,0,
        6,8,0,5,0,0,0,0,1,
        0,0,4,0,6,0,0,8,3,
        9,0,0,0,0,0,0,0,7,
        8,7,0,0,9,0,4,0,0,
        7,0,0,0,0,3,0,6,4,
        0,0,0,0,0,8,1,0,0,
        4,0,1,0,0,7,0,0,0,
#endif
#if (Level == 3) // very hard
        3,0,0,0,0,0,0,0,9,
        0,7,0,0,4,0,0,3,0,
        0,0,6,1,0,3,5,0,0,
        0,0,7,0,3,0,8,0,0,
        0,8,0,2,0,4,0,1,0,
        0,0,5,0,7,0,6,0,0,
        0,0,2,3,0,7,4,0,0,
        0,1,0,0,6,0,0,2,0,
        8,0,0,0,0,0,0,0,7,
#endif
    };

    Sodu<9> x9(x);
    x9.print();
    x9.solve_par();
    x9.print();
#else // test 4X4 mode
    short a[]=
    {
		1,0,0,3,
        0,0,0,1,
        0,0,0,2,
        2,0,0,4,

		/*1,2,4,3,
        4,3,2,1,
        3,4,1,2,
        2,1,3,4,*/
    };
    Sodu<4> x4(a);
    x4.print();
    x4.solve_par();
    x4.print();
#endif
	clock_t end = clock();
	cout << "\nCOST:"<< (end - beg) << "MS";
}
