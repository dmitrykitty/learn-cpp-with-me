#include <iostream>
#include <vector>

class Solution {
public:
    std::vector<int> separateDigits(std::vector<int>& nums) {
        std::vector<int> v;

        for(int i = 0; i < nums.size(); ++i){
            std::string cur = std::to_string(nums[i]);
            for(int j = 0; j < cur.size(); ++j){
                v.push_back(cur[j] - '0');
            }
        }

        return v;
    }
};