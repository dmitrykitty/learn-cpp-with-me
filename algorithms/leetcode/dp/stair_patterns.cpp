#include <bits/stdc++.h>

int stairs(int number) {
    if(number <= 1) {
        return 1; 
    }

    return stairs(number - 1) + stairs(number - 2); 
}

int stairs_memo(int number, std::vector<int>& memo) {
    if(number <= 1) {
        return 1; 
    }

    if(memo[number] != -1) {
        return memo[number];
    }

    return memo[number] = stairs_memo(number - 1, memo) + stairs_memo(number - 2, memo); 
}