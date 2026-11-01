#include <bits/stdc++.h>
using namespace std; 

vector<double> calcEquation(vector<vector<string>>& equations, vector<double>& values, vector<vector<string>>& queries) {
        unordered_map<string, vector<pair<string, double>>> adj; 
        for(int i = 0; i < values.size(); ++i) {
            string& from = equations[i][0];
            string& to = equations[i][1];
            double rate = values[i];
            
            adj[from].push_back({to, rate}); 
            adj[to].push_back({from, 1.0 / rate}); 
        }

        vector<double> res; 
        res.reserve(queries.size());

        for(const auto& query: queries) {
            const string& from = query[0];
            const string& to = query[1];

            if(!adj.contains(from) || !adj.contains(to)) {
                res.push_back(-1.0);
                continue;
            }

            if(from == to) {
                res.push_back(1.0);
                continue;
            }

            queue<pair<string, double>> q; 
            unordered_set<string> visited; 
            double ans = -1.0; 

            q.push({from, 1.0}); 
            visited.insert(from); 

            while(!q.empty() && ans == -1.0) {
                auto cur = q.front(); 
                q.pop(); 

                for(auto& neigh: adj[cur.first]) {
                    if(!visited.contains(neigh.first)) {
                        double new_rate = cur.second * neigh.second; 
                        if(to == neigh.first) {
                            ans = new_rate; 
                            break; 
                        }
                        q.push({neigh.first, new_rate}); 
                        visited.insert(neigh.first); 
                    }
                }
            }
            res.push_back(ans); 
        }
        return res;
    }