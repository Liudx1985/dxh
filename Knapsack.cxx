#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
bool solution[10]; // Pick knap-i

int weight[10] = {4, 54, 1, 6, 7, 4, 5, 23, 11, 32};
int cost[10] = {3, 3, 11, 12, 5, 8, 15, 20, 15, 23};

const int maxW = 100; // Sum (Wi * Ni) =  100
int maxC = 0;         // get Max Wi * Ci * Ni

std::vector<int> ans;
/*
A(j, Y)的?推?系?：
A(0, Y) = 0
A(j, 0) = 0
如果wj > Y,??j, A(j, Y) = A(j - 1, Y) 
如果wj ? Y,放入j, A(j, Y) = max { A(j - 1, Y), pj + A(j - 1, Y - wj)}
-}
*/

int ks(int j, int Y)
{
    if (j == -1 || Y == 0)
        return 0;
    if (weight[j] > Y)
    {
        solution[j] = 0;
        return ks(j - 1, Y);
    }
    solution[j] = 1;
    int a = ks(j - 1, Y);
    int b = cost[j] + ks(j - 1, Y - weight[j]);

    return max(a, b);
}

void knapsack(int n, int w, int c)
{
    if (w > maxW)return;
    if (n == 10)    {
        if (c > maxC) {
            maxC = c;
            // store solution
            ans.assign(solution, solution + 10);
        }
        return;
    }

    // put
    if (w + weight[n] < maxW) {
        solution[n] = true;
        knapsack(n + 1, w + weight[n], c + cost[n]);
    }
    //
    solution[n] = false;
    knapsack(n + 1, w, c);
}

int main() {
    knapsack(0, 0, 0);
    cout << maxC << endl;
    for (auto k : ans)
    {
        cout << int(k) << " ";
    }

    //--------------
    cout << "\n"
         << ks(9, maxW)
         << "\n";
    for (auto k : solution)
    {
        cout << int(k) << " ";
    }
}
