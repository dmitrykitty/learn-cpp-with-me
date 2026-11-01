#include <bits/stdc++.h>

using namespace std; 

int min_meeting_rooms(vector<vector<int>> intervals) {
    vector<pair<int, int>> events;
    events.reserve(intervals.size() * 2);  
    for(const auto& in: intervals) {
        events.push_back({in[0], 1}); 
        events.push_back({in[1], -1}); 
    }

    //sorted correctly ad {5,-1} is before then {5, 1}
    sort(events.begin(), events.end()); 

    int max_res = 0;
    int cur_res = 0;  
    for(const auto& event : events) {
        cur_res += event.second;
        max_res = max(max_res, cur_res); 
    }
    return max_res; 
}