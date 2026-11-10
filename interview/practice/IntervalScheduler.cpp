#include <cassert>
#include <map>
#include <stdexcept>
#include <optional>
#include <unordered_map>
#include <vector>

using namespace std; 

using map_iterator = map<int, int>::iterator; 

struct Booking {
    long long id; 
    int room; 

    bool operator==(const Booking&) const = default;
}; 


class MeetingScheduler {
    vector<map<int, int>> rooms;
    unordered_map<long long, pair<int, map_iterator>> meetings; 
    long long next_id{0};  

public: 
    MeetingScheduler(int K): rooms(validate_room_count(K)) {}

    optional<Booking> book(int start, int end) {
        validate_interval(start, end);

        for(int room = 0; room < rooms.size(); room++) {
            auto maybe_id = book_for_room(start, end, room); 
            if(maybe_id) {
                return *maybe_id; 
            }
        }
        return nullopt; 
    }

    bool cancel(long long id) {
        auto it = meetings.find(id); 
        if(it == meetings.end()) {
            return false; 
        }

        auto& meetings_times = rooms[it->second.first]; 
        meetings_times.erase(it->second.second); 
        meetings.erase(it); 

        return true; 
    }

private: 
    optional<Booking> book_for_room(int start, int end, int room_id) {
        auto& meetings_times = rooms[room_id]; 
        auto it = meetings_times.lower_bound(start); 
        if(it != meetings_times.end() && end > it->first) {
            return nullopt; 
        }

        if(it != meetings_times.begin() && prev(it)->second > start) {
            return nullopt;
        }

        //place node where it pointed. save one lookup
        auto inserted_it = meetings_times.emplace_hint(it, start, end); 
        long long cur_id = next_id++;
        meetings[cur_id] = {room_id, inserted_it}; 

        return Booking{cur_id, room_id}; 
    }

    void validate_interval(int start, int end) {
        if(start >= end) {
            throw invalid_argument("invalid interval start and end time");
        }
    }

    int validate_room_count(int K) {
        if(K <= 0) {
            throw invalid_argument("wrong amount of rooms"); 
        }
        return K; 
    }
};


int main() {
    MeetingScheduler ms(3); 

    //invalid throw
    bool thrown = false; 
    try {
        ms.book(5, 3);
    } catch(invalid_argument&) {
        thrown = true; 
    }
    assert(thrown); 

    thrown = false; 
    try {
        ms.book(5, 5);
    } catch(invalid_argument&) {
        thrown = true; 
    }
    assert(thrown); 

    assert((ms.book(10, 20) == Booking{0, 0}));
    assert((ms.book(10, 15) == Booking{1, 1}));
    assert((ms.book(14, 25) == Booking{2, 2})); 
    assert((!ms.book(13, 20))); 
    assert((ms.book(15, 25) == Booking{3, 1})); 
    assert((ms.book(20, 25) == Booking{4, 0})); 
    assert((ms.book(25, 30) == Booking{5, 0})); 

    assert((ms.book(40, 60) == Booking{6, 0})); //smallest room id

    assert(!ms.cancel(7)); //not existing 
    assert(ms.cancel(0)); //10, 20 removed
    assert(!ms.cancel(0)); //double cancel is impossible
    assert((ms.book(10, 15) == Booking{7, 0})); 

    thrown = false; 
    try {
        MeetingScheduler(-5);
    } catch(invalid_argument&) {
        thrown = true; 
    }
    assert(thrown); 

    thrown = false; 
    try {
        MeetingScheduler(0);
    } catch(invalid_argument&) {
        thrown = true; 
    }
    assert(thrown); 
}


