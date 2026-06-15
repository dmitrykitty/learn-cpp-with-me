#include <bits/stdc++.h>

using namespace std;


int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n, target, num, res = 0; 
    cin >> n >> target;
    vector<int> vi(n); 

    for(int i = 0; i < n; ++i){
        cin >> vi[i]; 
    }

    long long cur_sum = 0;
    int l = 0;
    for(int r = 0; r < vi.size(); ++r){
        cur_sum += vi[r]; 

        while(cur_sum > target){
            cur_sum -= vi[l];
            l++;
        }

        if(cur_sum == target){
            res++; 
        }
    }
    cout << res;
}