#include <bits/stdc++.h>

using namespace std;

class TimeMap {
    unordered_map<string, vector<pair<int, string>>> tabels; 
public:
    TimeMap() {}
    
    void set(string key, string value, int timestamp) {
        tabels[key].emplace_back(timestamp, value);
    }
    
    string get(string key, int timestamp) {
        auto it = tabels.find(key); 
        if(it == tabels.end()) {
            return ""; 
        }

        const auto& records = it->second; 
        int idx = find_upper_bound(records, timestamp);
        if(idx == 0) {
            return "";
        }
        --idx;
        return records[idx].second; 
    }

private:
    int find_upper_bound(const vector<pair<int, string>>& pairs, int target) {
        int l = 0, r = pairs.size(); 

        while(l < r) {
            int mid = l + (r - l) / 2; 
            if(pairs[mid].first > target) {
                r = mid; 
            } else {
                l = mid + 1; 
            }
        }
        return r; 
    }
};
