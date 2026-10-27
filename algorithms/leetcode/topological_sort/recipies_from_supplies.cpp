#include <bits/stdc++.h>

using namespace std; 

vector<string> findAllRecipes(vector<string>& recipes, vector<vector<string>>& ingredients, vector<string>& supplies) {
        unordered_map<string, vector<string>> adj; 
        unordered_map<string, int> indegree;

        for(int i = 0; i < recipes.size(); i++) {
            indegree[recipes[i]] = ingredients[i].size();
            for(auto& ingr : ingredients[i]) {
                adj[ingr].push_back(recipes[i]);
            }
        }

        queue<string> q(supplies.begin(), supplies.end()); 
        vector<string> res; 

        while(!q.empty()) {
            string sup = q.front(); q.pop(); 
            for(auto& neigh: adj[sup]) {
                int& degree = indegree[neigh]; 
                degree--; 
                if(degree == 0) {
                    res.push_back(neigh);
                    q.push(neigh);
                }
                
            }
        }
        return res;
    }