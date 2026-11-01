#include <bits/stdc++.h>

using namespace std;  
 
 int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        int sum_gas = 0; 
        int sum_cost = 0; 

        int balance = 0; 
        int last_start_station = 0; 
        for(int i = 0; i < gas.size(); i++) {
            sum_gas += gas[i]; 
            sum_cost += cost[i]; 

            if(balance < 0) {
                last_start_station = i; 
                balance = 0; 
            }
            balance += (gas[i] - cost[i]); 
        }

        if(sum_gas < sum_cost) {
            return -1; 
        }

        return last_start_station; 
    }