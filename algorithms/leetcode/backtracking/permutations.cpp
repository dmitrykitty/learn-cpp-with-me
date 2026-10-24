/*
LC46, LC47
Piotr works as a bike courier for a small delivery company in Kraków. Every morning the dispatcher gives him a handful of parcels that must all be delivered before noon. Each parcel has a unique ID and goes to a different address. The addresses are spread across the city, and Piotr hasn’t decided in which order to visit them.

The company is trialling a new route-planning tool. Before it starts optimizing for distance and traffic, the product manager wants a simple first version that prints every possible order in which Piotr could visit the stops. Each stop must be visited exactly once, and two routes are different if the order of the stops differs. The first stop in the list is the first one he visits. The tool will be used only on small days: at most 6 parcels.

You’re given the parcel IDs as an array of distinct integers parcels. Return all possible visiting orders, in any order.
*/




#include <unordered_set>
#include <vector>

void backtrack(
    std::vector<std::vector<int>>& result, 
    std::vector<int>& path, 
    const std::vector<int>& parcels, 
    std::vector<bool>& added
) {
    if(path.size() == parcels.size()) {
        result.push_back(std::vector(path.begin(), path.end())); 
        return;
    }

    std::unordered_set<int> duplicates; 
    for(int i = 0; i < parcels.size(); ++i) {
        if(added[i] || duplicates.find(parcels[i]) != duplicates.end()) {
            continue;
        }
        duplicates.insert(parcels[i]);

        path.push_back(parcels[i]);
        added[i] = true;
        backtrack(result, path, parcels, added);
        added[i] = false; 
        path.pop_back();
    }
}

std::vector<std::vector<int>> parcels_path(const std::vector<int>& parcels) {
    std::vector<std::vector<int>> result; 
    std::vector<int> path; 
    std::vector<bool> added(parcels.size());

    backtrack(result, path, parcels, added);
    return result;
}
