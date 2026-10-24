/*
LC78, LC90
Sunrise Coffee is a small café in Kraków that is launching a “build your own tasting flight” menu. The owner, Marta, has a list of distinct single-origin coffees she can serve, for example Ethiopia, Colombia, Kenya and Brazil. A flight is any group of these coffees, from just one coffee up to all of them. The order inside a flight doesn’t matter, so “Kenya + Brazil” and “Brazil + Kenya” are the same flight. Marta also wants to allow an empty flight, which is just a glass of water.

Before the opening day, Marta wants a complete printed list of every possible flight so that the barista can check it against the menu. She doesn’t want any flight missing, and she doesn’t want any flight listed twice.

You’re given the coffees as an array of integers nums, where each integer is a unique ID for one coffee. All IDs are different. Return all possible flights in any order. The café has at most 10 coffees in stock.
*/

#include <algorithm>
#include <vector>

void backtrack(std::vector<std::vector<int>>& result, std::vector<int>& path, std::vector<int>& coffes, int start) {
    result.push_back(std::vector<int>(path));

    //reduntant becuase loop stops by itself
    // if(start >= coffes.size()) {
    //     return;
    // }

    for(int i = start; i < coffes.size(); ++i) {

        if(i > start && coffes[i - 1] == coffes[i]) {
            continue;
        }

        path.push_back(coffes[i]);
        backtrack(result, path, coffes, i + 1);
        path.pop_back();
    }
}

std::vector<std::vector<int>> get_possible_flights(std::vector<int> coffes) {
    std::vector<std::vector<int>> result; 
    std::vector<int> path;
    std::sort(coffes.begin(), coffes.end());

    backtrack(result, path, coffes, 0);
    return result;
}