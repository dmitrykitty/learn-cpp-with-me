#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    int shipWithinDays(vector<int>& weights, int days) {
        //time interval - days 
        //ith package weight = weights[i]

        int right = 0; 
        int left = 0; 

        for(int w: weights) {
            right += w; 
            left = max(left, w); 
        }


        while(left < right) {
            int mid = left + (right - left) / 2; 

            if(is_enough_capacity(weights, mid, days)) {
                right = mid; 
            } else {
                left = mid + 1; 
            }
        }

        return right; 
    }

private: 
    bool is_enough_capacity(vector<int>& weights, int cap, int limit) {
        int needed = 1;
        int cap_left = cap;
        for(int w: weights) {
            if(w > cap_left) {
                needed++;
                if(needed > limit) {
                    return false;
                }
                cap_left = cap; 
            }
            cap_left -= w; 
        }

        return true; 
    }
};