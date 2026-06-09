#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n, num; 
    unordered_set<int> set;
    set.reserve(n * 2); //for no reallocation 
    cin >> n; 

    for(int i = 0; i < n; ++i){
        cin >> num; 
        set.insert(num);
    }

    cout << set.size();
}