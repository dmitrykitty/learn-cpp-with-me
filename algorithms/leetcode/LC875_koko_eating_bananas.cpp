#include <bits/stdc++.h>

using namespace std; 

class Solution {
public:
    int minEatingSpeed(vector<int>& piles, int h) {
        //n piles where piles[i] - number of bananas on ith pile
        //window - h hours

        int right = 0; 
        for(int pile: piles) {
            right = max(right, pile); 
        }

        //So now we will make log speed checks for n piles -> O(log speed * n)
        int left = 1; 

        //[left, right)
        while(left < right) {
            //if our limit more then 1.25 * 10^9: right + left -> overflow  
            int mid = left + (right - left) / 2; 
            if(is_enough_time(piles, mid, h)) {
                right = mid; 
            } else {
                left = mid + 1; 
            }
        }
        return right; 
    }

/*
Input: piles = [30,11,23,4,20], h = 5
Output: 30

speed 30 -> log 30 * size
[1...30] mid = 15. 2 + 1 + 2 + 1 + 2 = 8 > 5
[16...30] mid = 23. 2 + 1 + 1 + 1 + 1 = 6 > 5
[24...30] mid = 27. 6 > 5
[28...30] mid = 29.  6 > 5
[30...30] 
*/

private: 
    bool is_enough_time(vector<int>& piles, int speed, int limit) {
        long long needed = 0; 
        for(auto pile: piles) {
            needed += (pile / speed + (pile % speed != 0));

            if(needed > limit) {
                return false; 
            }
        }
        return true; 
    }
};