#include <bits/stdc++.h>

using namespace std;

/*
Time: O(n log n + m log m)
Memory: O(n + m)
*/
int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int app, apart, diff, num; 
    cin >> app >> apart >> diff; 

    vector<int> expected, real; 
    expected.reserve(app); 
    real.reserve(apart);

    for(int i = 0; i < app; ++i){
        cin >> num;
        expected.push_back(num);
    }

    for(int i = 0; i < apart; ++i){
        cin >> num;
        real.push_back(num);
    }

    sort(expected.begin(), expected.end());
    sort(real.begin(), real.end());

    int ptr1 = 0, ptr2 = 0, res = 0;

    while(ptr1 < app && ptr2 < apart){
        if(expected[ptr1] < real[ptr2] - diff){
            ptr1++;
            continue;
        } else if (real[ptr2] + diff < expected[ptr1]) {
            ptr2++;
        } else {
            res++; 
            ptr1++; 
            ptr2++;
        }
    }

    cout << res;
}