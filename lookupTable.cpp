/*  look-up table,
 [Ai] is a list in range [u,v]
  construct a lookup table [u..v] record the occurance.
 */
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <iostream>

using namespace std;

int main()
{
    int array[5] = {3, 6, 9, 9, 1};
    int c[9 + 1] = {0};

    for (auto x : array)
    {
        ++c[x];
    }

    for (auto x : c)
    {
        cout << x << ends;
    }
    cout << endl;
    // sort table
    for (int j = 0, i = 0; j < 10 && i < 5; ++j)
    {
        cout << "check number " << j << " @ array:" << i << "=";
        while (c[j] > 0)
        {
            --c[j];         // reduct count
            array[i++] = j; // replace
        }
        for (auto x : array)
        {
            cout << x << ends;
        }
        cout << endl;
    }
    return 0;
}
