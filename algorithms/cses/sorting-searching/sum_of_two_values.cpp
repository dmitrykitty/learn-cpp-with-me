#include <bits/stdc++.h>
using namespace std; 

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    int n, target, num; 

    cin >> n >> target;
    unordered_map<int, int> umap;
    umap.reserve(n);
    
    for(int i = 0; i < n; ++i){
        cin >> num; 
        if(auto it = umap.find(num); it != umap.end()){
            cout << it->second + 1 << " " << i + 1;
            return 0; 
        }
        umap[target - num] = i;
    }

    cout << "IMPOSSIBLE";
}