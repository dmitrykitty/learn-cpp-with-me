#include <bits/stdc++.h>

using namespace std; 

vector<int> findOrder(int n, vector<vector<int>>& requirements) {
	vector<int> indegree(n, 0);
	vector<vector<int>> adj(n); 
    for(const auto& pair : requirements) {
        indegree[pair[0]]++;
        adj[pair[1]].push_back(pair[0]);
    }

    queue<int> q; 
    for(int i = 0; i < n; ++i) {
        if(indegree[i] == 0) {
            q.push(i); //add available module
        }
}

    vector<int> order; //result
    order.reserve(n);

    while(!q.empty()) {
        int module = q.front(); 
        q.pop(); 
        order.push_back(module);
        for(int next: adj[module]) {
            if(--indegree[next] == 0){
                q.push(next);
            }
        }
    }
    return order.size() != n ? vector<int>() : order; 
}
