#include <iostream>
#include <map>
#include <set>
#include <assert.h>


class UniqueTracker {
public:
    void add(int x) {
        int& count = nums[x]; 
        if(nums[x] > 0) {
            singles.erase(x); 
        } else {
            singles.insert(x);
        }
        count++; 
    }
    //assume at least 1 x is presented
    void remove(int x) {
        auto it = nums.find(x);
        if(it->second == 1) {
            nums.erase(it);
            singles.erase(x); 
        } else {
            it->second--; 
            if(it->second == 1) {
                singles.insert(x);
            }
        }
    }
    int query() const {
        if(singles.empty()) {
            return -1; 
        }
        return *singles.begin(); 
    }

private: 
    std::map<int, int> nums; 
    std::set<int> singles; 
};

int main() {
    //-----------------------------------------------------
    UniqueTracker tr; 
    tr.add(1); 
    assert(tr.query() == 1); 
    tr.add(0); 
    assert(tr.query() == 0); 
    tr.add(0); 
    assert(tr.query() == 1); 
    tr.remove(1); 
    assert(tr.query() == -1); 
    tr.add(5); 
    tr.add(5); 
    tr.add(3); 
    tr.add(3); 
    assert(tr.query() == -1); 
    tr.remove(5); 
    assert(tr.query() == 5); 
    tr.remove(3); 
    assert(tr.query() == 3); 
    tr.remove(3); 
    assert(tr.query() == 5); 
    //-------------------------------------------------------
    std::cout << "OK\n";



    UniqueTracker tracker;

    std::string command;

    while (std::cin >> command) {
        if (command == "ADD") {
            int x;
            std::cin >> x;
            tracker.add(x);
        } else if (command == "REMOVE") {
            int x;
            std::cin >> x;
            tracker.remove(x);
        } else if (command == "QUERY") {
            std::cout << tracker.query() << '\n';
        } else if(command == "END") {
            break; 
        }
    }

    

    return 0;
}