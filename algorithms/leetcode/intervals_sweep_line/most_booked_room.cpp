#include <bits/stdc++.h>

using namespace std; 

int mostBooked(int n, vector<vector<int>>& meetings) {
        vector<int> rooms(n); 
        for(int i = 0; i < n; ++i) {
            rooms[i] = i; 
        }

        // by {end_time, room_id}
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> busy; 
        // by room_id
        priority_queue<int, vector<int>, greater<>> free(rooms.begin(), rooms.end());
        sort(meetings.begin(), meetings.end()); 
        rooms = vector<int>(n, 0); 

        for(const auto& meeting: meetings) {
            //finish all possible previous meetings
            while(!busy.empty() && busy.top().first <= meeting[0]) {
                free.push(busy.top().second); 
                busy.pop(); 
            }


            if(!free.empty()) {
                int next_room_id = free.top(); 
                free.pop(); 

                busy.push({meeting[1], next_room_id});
                rooms[next_room_id]++; 
            } else {
                auto [delayed_end_time, delayed_room_id] = busy.top(); 
                busy. pop(); 

                long long delay_time = delayed_end_time - meeting[0]; 
                busy.push({(long long)meeting[1] + delay_time, delayed_room_id}); 
                rooms[delayed_room_id]++; 
            }
        }

        int max_meetings = 0; 
        int max_id = -1; 

        for(int i = 0; i < n; ++i) {
            if(rooms[i] > max_meetings) {
                max_meetings = rooms[i]; 
                max_id = i; 
            }
        }
        return max_id; 
    }