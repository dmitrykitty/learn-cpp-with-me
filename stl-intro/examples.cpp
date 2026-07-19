#include <bits/stdc++.h>    

void explain_pair() {
    std::pair<int, int> p = {1, 2};
    std::cout << p.first << " " << p.second << std::endl; 

    std::pair<int, int> arr[]  = {{1, 2}, {3, 4}};
    std::cout << arr[0].second << std::endl; 

    auto [first, second] = arr[0]; 
    std::cout << first << second << std::endl; 

    auto& [ref1, ref2] = arr[1]; 
    ref1 = 8; 
    std::cout << arr[1].first << std::endl;
}

void explain_vector() {
    std::vector<int> v; 
    v.push_back(1); 
    v.emplace_back(2); 

    std::vector<std::pair<int, int>> vec; 
    vec.push_back({1, 2}); 
    vec.emplace_back(1, 2); //usually faster

    std::vector<int> v(5, 100); //size=5, values = 100 
    std::vector<int> v2(v);
    
    std::vector<int>::iterator it = v.begin(); 
    it++; 

    std::cout << *(it) << " "; //v[1]

    it += 2; 
    std::cout << *(it) << " "; //v[3]

    for(auto it = v.begin(); it != v.end(); it++) {
        std::cout << *(it); 
    }

    //rbegin, rend 
    //     [1, 2, 3, 4, 5]
    //  |               |
    // rend            rbegin
    //for rbegin ++ is actually -- 

    for(auto it = v.rbegin(); it != v.rend(); it++) {
        std::cout << *(it); 
    }
}

int main() {

}