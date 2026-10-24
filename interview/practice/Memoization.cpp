
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <assert.h>
#include <functional>
#include <exception>
#include <list>


// ---------------------------------------------------------------
// Part 1
// We have a function:
//     int slow_f(int x);
// It is expensive to call (milliseconds to seconds per call), and the
// same arguments come up again and again.
// Write something that makes this faster.
// Ask clarifying questions first, sketch the types and interface,
// then implement and test it.
// ---------------------------------------------------------------
// Part 2
// This class now goes into a long-running service that stays up for
// weeks, and the callers' x values are spread over a very wide range.
// After a few days the process is out of memory. Fix it.
// ---------------------------------------------------------------
// Part 3
// The service has been running for a while. Monitoring shows a poor
// hit rate: some values of x are requested constantly, yet get evicted
// just because they were inserted a long time ago, while values requested
// once sit in the cache. Change the cache so that stops happening.
// ---------------------------------------------------------------
// Part 4
// Some callers want to cache a recursive function. 
// The callback itself calls the same Memo for smaller arguments, 
// for example computing a value for n from the values for n-1 and n-2.
//  Make that work.

int slow_f(int x) {
    return x * x; 
}

class Memo {
    std::list<int> cache; 
    std::unordered_map<int, std::pair<int, std::list<int>::iterator>> memo_;

    std::function<int (int)> callback_;
    size_t N_;
public:
    explicit Memo(std::function<int (int)> callback, int N): callback_(std::move(callback)) {
        if(N <= 0) {
            throw std::invalid_argument("N should be > 0");
        }
        N_ = N; 
    }

    int operator()(int x) {
        auto  [it, inserted] = memo_.try_emplace(x); // one lookup

        if(inserted) {
            try {
                int res = callback_(x); //thrown only here, so any changed bellow are not applied

                //if not throw -> update cache and map
                if(cache.size() == N_) {
                    memo_.erase(cache.back());
                    cache.pop_back(); 
                }
                cache.push_front(x);
                it->second = {res, cache.begin()};
            } catch(...) {
                memo_.erase(it);
                throw; 
            }
        } else {
            //move x to the front
            //target where to place, from where take, from which position
            cache.splice(cache.begin(), cache, it->second.second); 
        }
        return it->second.first; 
    }
};


int main() {
    int counter = 0; 
    Memo memo([&](int x){
        counter++;
        return x * x;
    }, 3); 
    

    memo(1); //1
    memo(2); //2 1
    memo(3); //3 2 1
    memo(2); //not called 2 3 1
    memo(1); //not called 1 2 3

    assert(counter == 3);

    memo(4); //new call -> limit -> 3 was removed as the least recently used element 4 1 2
    assert(counter == 4);
    memo(3);
    assert(counter == 5); //2 was removed 3 4 1
    memo(1); //nothing changed in counter, but 1 was moved 1 3 4
    assert(counter == 5);
    memo(2); //2 1 3 
    assert(counter == 6);
    memo(3); //3 2 1
    assert(counter == 6);
    memo(4); //4 3 2 -> one was removed
    assert(counter == 7);


    //======================================================
    int counter_exc = 0;
    bool throwed_once = false;
    Memo memo_exc([&](int x){
        if(!throwed_once && x == 5) {
            throw std::invalid_argument("exc");
        }

        if(x == 1) {
            throw std::invalid_argument("exc");
        }
        counter_exc++;
        return x*x; 
    }, 3); 

    memo_exc(2); //2
    memo_exc(3); //3 2
    memo_exc(4); //4 3 2
    assert(counter_exc == 3);

    try {
        memo_exc(1); //counter sould't be incremented and 2 still should be preset
    } catch(...) {
        assert(counter_exc == 3); //counter not incremented
    }

    memo_exc(2); //2 4 3 
    assert(counter_exc == 3); //2 wasnt removed but was moved to the front


    try { 
        memo_exc(5);
    } catch(const std::invalid_argument& e) {
        throwed_once = true;
        assert(counter_exc == 3); //nothing changed again, 5 wasn't added
    }

    assert(25 == memo_exc(5)); //5 was added -> 5 2 4
    assert(counter_exc == 4); //should be incremented and 3 removed
    assert(9 == memo_exc(3)); // 3 5 2 
    assert(5 == counter_exc); 
}