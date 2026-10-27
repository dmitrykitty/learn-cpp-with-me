#include <bits/stdc++.h>

using namespace std;
    
    struct DisjoinSet {
        vector<int> parents_; 
        vector<int> sizes_;

        DisjoinSet(int num): parents_(num), sizes_(num, 1) {
            iota(parents_.begin(), parents_.end(), 0);
        }

        int find(int x) {
            while(x != parents_[x]) {
                parents_[x] = parents_[parents_[x]];
                x = parents_[x];
            }
            return x;
        }

        bool merge(int a, int b) {
            a = find(a);
            b = find(b);

            if(a == b) {
                return false;
            }

            if(sizes_[a] < sizes_[b]) {
                swap(a, b);
            }

            parents_[b] = a; 
            sizes_[a] += sizes_[b];
            return true;
        }
    };

    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        DisjoinSet dsj(edges.size());
        vector<int> res(2); 
        for(auto& edge: edges) {
            if(!dsj.merge(edge[0] - 1, edge[1] - 1)) {
                res[0] = edge[0];
                res[1] = edge[1];
            }
        }
        return res; 
    }