#include <bits/stdc++.h>

using namespace std;
    bool is_safe(int v, vector<vector<int>>& adj, vector<int>& state) {
        if(state[v] == 2) {
            state[v] = 0;
            return false;
        }

        if(state[v] != -1) {
            return state[v];
        }

        state[v] = 2;
        for(int next: adj[v]) {
            if(!is_safe(next, adj, state)) {
                state[v] = 0;
                return false;
            }
        }
        state[v] = 1;
        return true; 
    }

    vector<int> eventualSafeNodes(vector<vector<int>>& adj) {
        //graph = adj list, i -> adj[i]

        vector<int> state(adj.size(), -1); 
        vector<int> res; 
        //-1 - unvisited
        //1 - terminated / safe
        //0 - unsafe
        //2 - in dfs process (if 2 fo und -> cycle -> unsafe node)

        for(int v = 0; v < adj.size(); ++v) {
            if(is_safe(v, adj, state)) {
                res.push_back(v);
            }
        }
        return res;
    }

    