#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    int longestSubarray(vector<int>& nums, int limit) {
        //window size = end - start + 1
        //window state to be extended -> decr.front() - incr.front() <= limit
        deque<int> incr; 
        deque<int> decr; 

        incr.push_back(nums[0]);
        decr.push_back(nums[0]); 

        int res = 1, start = 0, n = nums.size(); 
        for(int end = 1; end < n; ++end) {
            //update monotonic deques
            int cur = nums[end];
            while(!decr.empty() && cur > decr.back()) {
                decr.pop_back(); 
            }
            while(!incr.empty() && cur < incr.back()) {
                incr.pop_back(); 
            }
            decr.push_back(cur);
            incr.push_back(cur);

            //update window size after breaking window state condition
            while(start < end && decr.front() - incr.front() > limit) {
                if(nums[start] == incr.front()) {
                    incr.pop_front();
                }
                if(nums[start] == decr.front()) {
                    decr.pop_front();
                }
                start++; 
            }

            //get best result
            res = max(res, end - start + 1);
        }

        return res; 
    }
};