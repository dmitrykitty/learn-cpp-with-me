/*
LC39, LC40 (backtrack call with i + 1)
Zofia runs a stall at the Christmas market in Kraków. Her regular customers get discount vouchers from a loyalty app, and the app offers several kinds of voucher. Each kind has a fixed value in zloty, and no two kinds have the same value, for example 2 zł, 3 zł, 6 zł and 7 zł. A customer may use as many vouchers of the same kind as they want in a single purchase, so using the 2 zł voucher three times is allowed.

At the end of the season, the app’s developers want to check the system against a list of edge cases. For a given target discount, they need every distinct way to reach exactly that amount by combining voucher kinds. Two ways are the same if they use the same kinds the same number of times, regardless of the order, so “2 + 2 + 3” and “3 + 2 + 2” count as one way. No way may appear twice in the list, and ways that do not sum to the target must not appear.

You’re given vouchers, an array of distinct positive integers (the values of the kinds), and an integer target. Return all distinct combinations, in any order. There are at most 30 kinds, each worth between 2 and 40 zł, and target is at most 40.
*/

#include <vector>
#include <algorithm>

void backtrack(
    std::vector<std::vector<int>>& result, 
    std::vector<int>& path, 
    const std::vector<int>& vouchers, 
    int sum, int target, int last_picked) {
    if(sum == target) {
        result.push_back(std::vector(path.begin(), path.end()));
        return;
    }

    for(int i = last_picked; i < vouchers.size(); ++i) {
        if(sum + vouchers[i] > target) {
            break;
        }

        path.push_back(vouchers[i]);
        backtrack(result, path, vouchers, sum + vouchers[i], target, i);
        path.pop_back();
    }
}

std::vector<std::vector<int>> get_vouchers_combinations(std::vector<int> vouchers, int target) {
    std::sort(vouchers.begin(), vouchers.end());
    std::vector<std::vector<int>> result; 
    std::vector<int> path; 

    backtrack(result, path, vouchers, 0, target,0);
    return result; 
}

