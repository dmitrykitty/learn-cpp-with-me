#include <vector>

/*


void backtrack(
    const std::vector<int>& nums, 
    int start, 
    std::vector<int>& path, 
    std::vector<std::vector<int>>& answers) {
    if(end){
        answers.push_back(path);
        return;
    }

    for(int i = start; i < nums.size(); ++i) {
        if(not_correct(nums[i])) {
            continue;
        }

        path.push_back(nums[i]);
        backtrack(nums, i + 1, path, answers)
        path.pop_back();
    }
}

std::vector<std::vector<int>> func(std::vector<int> data, int expexted_state) {
    prepare storage for result 
    backtrack_func(data, result, prev_state, expected_state)
}

*/