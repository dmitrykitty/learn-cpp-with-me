#include <bits/stdc++.h>

using namespace std; 
using LL = long long;

LL minv = LLONG_MAX, cnt;
LL sum = 0;

void min_search(vector<int>& weights, LL cur_sum, int idx){
    minv = min(abs(cur_sum + cur_sum - sum), minv);
    if(minv == 0 || 2 * cur_sum > sum){
        return;
    }

    for(int i = idx; i < cnt; ++i){
        min_search(weights, cur_sum + weights[i], i + 1);
    }
}

int main(){
    int weight; 

    cin >> cnt; 
    vector<int> weights(cnt);

    for(int i = 0; i < cnt; ++i){
        cin >> weight;
        weights[i] = weight;
        sum += weight;
    }
    min_search(weights, 0, 0);
    cout << minv;
}