#include <bits/stdc++.h>
using namespace std; 

class Solution {
    //O(n log n + n log(D + 1)) where D - max possible distance 
public:
    int smallestDistancePair(vector<int>& nums, int k) {
        //O(log n) memory capacity 
        sort(nums.begin(), nums.end()); 

        int left = 0; 
        int right = nums.back() - nums.front();

        while(left < right) {
            int mid_distance = left + (right - left) / 2; 

            if(enourgh_pairs_with_lq_distance(nums, mid_distance, k)) {
                right = mid_distance; 
            } else {
                left = mid_distance + 1;
            }
        }
        return right; 
    }

private: 
    bool enourgh_pairs_with_lq_distance(const vector<int>& nums, int dist, int limit) {
        int found_pairs = 0; 

        int start = 0; 
        for(int end = 1; end < nums.size(); ++end) {
            while(nums[end] - nums[start] > dist) {
                start++; 
            }
            found_pairs += end - start; 
            if(found_pairs >= limit) {
                return true;
            }
        }
        
        return false; 
    }
};