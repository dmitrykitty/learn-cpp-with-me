#include <iostream> 
#include <deque>
#include <vector>
#include <assert.h>


//a - array of integers of size n 
// increase any subarray maximum 5 times
// return maximum possible frequency of any value after k increses 

/*
for each subarray - target = max(a_l ... a_r) - number to which we increses others 
so, for [2, 1, 3] - target = 3 
Operation 1:
Increment [0, 1]
               [3, 2, 3]

Operation 2:
Increment [1, 1]
               [3, 3, 3]

Ok, now we can translate to needed array = [1, 2, 0] - descrivbes amount of needed increments for a_i
A = [2, 2, 2] → 2 operations
B = [2, 1, 2] → 3 operations
C = [2, 3, 2] → 3 operations

So, needed[0] + std::max(needed[i + 1] - needed[i], 0)
needed[0] = target - A[0]
needed[i + 1] - needed[i] = target - A[i + 1] - (target - A[i]) = A[i] - A[i + 1] 
min_cost(l, r) = target(l, r) - A[l] + sum(max(A[l] - A[l + 1], 0))

Invariant is following: 
    - by moving right we can increase or do not change min_cost 
    - by moving left we can decrease or do not change mon_cost 
    - min_cost(l, r) = target(l, r) - A[l] + sum(max(A[l] - A[l + 1], 0)) where only forst part depends on the maximum, and sum it just sequence of diffs 
    - so extending to the right only increaes sum by max(A[r] - A[r + 1], 0) and srinking from left decrease sum by max(A[l] - A[l + 1], 0)

--------------------------------------------------------------------
Monotonic deque used per window
a = [3, 8, 5, 2, 7]
    [3]
    [8   ]
    [8, 5   ]
    [8, 5, 2   ]
    [8, 7         ]
*/

int max_frequency_after_increasing_max_k_times(const std::vector<int>& a, int k) {
    if(a.size() < 2) {
        return a.size(); 
    }

    std::deque<int> md; 
    md.push_back(a[0]);

    int start = 0, res = 1;
    long long drops = 0, total_cost = 0; 
    for(int end = 1; end < a.size(); ++end) {
        drops += std::max(a[end - 1] - a[end], 0); //update drops

        //update md
        while(!md.empty() && md.back() < a[end]) {
            md.pop_back(); 
        }
        md.push_back(a[end]); 
        
        //update cost
        total_cost = (md.front() - a[start]) + drops;  
        std::cout << "[" << start << " " << end << "]: ts = " <<   md.front() - a[start] << " + " << drops << '\n'; 

        while(start < end && total_cost > k) {
            if(md.front() == a[start]) {
                md.pop_front(); 
            }
            start++; 
            drops -= std::max(0, a[start - 1] - a[start]); 
            total_cost = (md.front() - a[start]) + drops;  
        }
        res = std::max(res, end - start + 1); 
    }
    return res; 
}

int main() {
    // 1. Single element and equal elements
    assert(max_frequency_after_increasing_max_k_times({5}, 0) == 1);
    assert(max_frequency_after_increasing_max_k_times({5}, 100) == 1);
    assert(max_frequency_after_increasing_max_k_times({3, 3, 3, 3}, 0) == 4);
    assert(max_frequency_after_increasing_max_k_times({3, 3, 3, 3}, 5) == 4);

    // 2. Strictly increasing
    assert(max_frequency_after_increasing_max_k_times({1, 2, 3, 4}, 0) == 1);
    assert(max_frequency_after_increasing_max_k_times({1, 2, 3, 4}, 1) == 2);
    assert(max_frequency_after_increasing_max_k_times({1, 2, 3, 4}, 2) == 3);
    assert(max_frequency_after_increasing_max_k_times({1, 2, 3, 4}, 3) == 4);

    // 3. Strictly decreasing
    assert(max_frequency_after_increasing_max_k_times({4, 3, 2, 1}, 0) == 1);
    assert(max_frequency_after_increasing_max_k_times({4, 3, 2, 1}, 1) == 2);
    assert(max_frequency_after_increasing_max_k_times({4, 3, 2, 1}, 2) == 3);
    assert(max_frequency_after_increasing_max_k_times({4, 3, 2, 1}, 3) == 4);

    // 4. Our original example
    assert(max_frequency_after_increasing_max_k_times({2, 1, 3}, 0) == 1);
    assert(max_frequency_after_increasing_max_k_times({2, 1, 3}, 1) == 2);
    assert(max_frequency_after_increasing_max_k_times({2, 1, 3}, 2) == 3);

    // 5. Our sliding-window example
    assert(max_frequency_after_increasing_max_k_times({5, 3, 6, 2}, 2) == 2);
    assert(max_frequency_after_increasing_max_k_times({5, 3, 6, 2}, 3) == 3);
    assert(max_frequency_after_increasing_max_k_times({5, 3, 6, 2}, 6) == 3);
    assert(max_frequency_after_increasing_max_k_times({5, 3, 6, 2}, 7) == 4);

    // 6. Tricky: separated increments
    assert(max_frequency_after_increasing_max_k_times({0, 2, 0}, 2) == 2);
    assert(max_frequency_after_increasing_max_k_times({0, 2, 0}, 3) == 2);
    assert(max_frequency_after_increasing_max_k_times({0, 2, 0}, 4) == 3);

    assert(max_frequency_after_increasing_max_k_times({2, 0, 2}, 1) == 1);
    assert(max_frequency_after_increasing_max_k_times({2, 0, 2}, 2) == 3);

    // 7. Alternating values
    assert(max_frequency_after_increasing_max_k_times({1, 2, 1, 2, 1}, 1) == 3);
    assert(max_frequency_after_increasing_max_k_times({1, 2, 1, 2, 1}, 2) == 4);
    assert(max_frequency_after_increasing_max_k_times({1, 2, 1, 2, 1}, 3) == 5);

    // 8. Large values
    assert(max_frequency_after_increasing_max_k_times(
        {1000000000, 0, 1000000000}, 999999999) == 1);

    assert(max_frequency_after_increasing_max_k_times(
        {1000000000, 0, 1000000000}, 1000000000) == 3);

    // 9. Maximum in the middle of the window
    assert(max_frequency_after_increasing_max_k_times({1, 1, 5, 1, 1}, 3) == 2);
    assert(max_frequency_after_increasing_max_k_times({1, 1, 5, 1, 1}, 4) == 3);
    assert(max_frequency_after_increasing_max_k_times({1, 1, 5, 1, 1}, 8) == 5);

    // 10. Optional, if empty input is allowed
    assert(max_frequency_after_increasing_max_k_times({}, 0) == 0);
}