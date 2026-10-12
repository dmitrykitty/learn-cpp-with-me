#include <bits/stdc++.h>

using namespace std;



class Solution {
public:
    int sumSubarrayMinsBF(vector<int>& arr) {
        deque<int> ms; //indeces of previous and next mins 
        long long res = 0; 
        constexpr long long MOD = 1000000007; 
        int n = arr.size(), mid = 0, left = 0, right = 0;

        for(int cur = 0; cur < n; cur++) {
            while(!ms.empty() && arr[ms.back()] > arr[cur]) {
                right = cur; 
                mid = ms.back();
                ms.pop_back();
                left = ms.empty() ? -1 : ms.back();  

                res = (res + (long long)((mid - left) * (right - mid)) * arr[mid]) % MOD; 
            }
            ms.push_back(cur); 
        }

        while(!ms.empty()) {
            mid = ms.back(); 
            ms.pop_back(); 
            left = ms.empty() ? -1 : ms.back();  
            res = (res + (long long)((mid - left) * (n - mid)) * arr[mid]) % MOD; 
        }

        return res; 
    }
};