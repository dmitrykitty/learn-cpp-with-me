#include <bits/stdc++.h>



using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int d, sum, minv, maxv, min_sum = 0, max_sum = 0; 
    cin >> d >> sum; 

    vector<pair<int, int>> v(d); 

    for(int i = 0; i < d; ++i) {
        cin >> minv >> maxv; 
        min_sum += minv; 
        max_sum += maxv; 
        v[i] = {minv, maxv}; 
    }

    if(sum < min_sum || sum > max_sum) {
        cout << "NO"; 
        return 0; 
    }

    cout << "YES\n"; 

    int expected_diff = max_sum - sum; 
    for(auto& [mn, mx] : v){
        int cur_diff = mx - mn; 
        int real_diff = min(cur_diff, expected_diff); 

        mx -= real_diff; 

        mx -= real_diff;
        expected_diff -= real_diff;

        if (expected_diff == 0) {
            break;
        }
    }

    for(const auto& p : v) {
        cout << p.second << " ";
    }
}