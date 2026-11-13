#include <climits>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector> 
#include <queue>
#include <cassert>
#include <iostream>

using namespace std; 

struct Link {
    string from; 
    string to; 
    int cost; 
};

struct NodeInfo {
    long long cost; 
    int parent; 
}; 

class GraphTraversing {
    //pair<node, dist>
    vector<vector<pair<int, int>>> adj_;
    unordered_map<string, int> name_to_id_;  
    vector<vector<NodeInfo>> cache_; 
    vector<string> id_to_name_; 
public: 
    GraphTraversing(vector<Link> links) {
        int next_id = 0; 
        for(const auto& link : links) {
            if(link.cost < 0) {
                throw invalid_argument("negative cost is invalid"); 
            }
            int id_from = get_or_add_id(link.from, next_id);
            int id_to = get_or_add_id(link.to, next_id); 

            adj_[id_from].push_back({id_to, link.cost}); 
        }
        cache_ = vector<vector<NodeInfo>>(adj_.size()); 
    }

    bool reachable(const string& source, const string& target, long long max_cost) {
        if(max_cost < 0) {
            throw invalid_argument("cost should be non negative");
        }

        auto it_from = name_to_id_.find(source); 
        auto it_to = name_to_id_.find(target); 

        if(it_from == name_to_id_.end() || it_to == name_to_id_.end()) {
            throw invalid_argument("not existing target or source"); 
        }
        if(it_from == it_to) {
            return true; 
        }

        int s = it_from->second; 
        int t = it_to->second; 
        shortest_path_from(s);
        long long cost = cache_[s][t].cost;
        return cost != INF && cost <= max_cost; 
    }

    vector<string> cheapest_path(const string& source, const string& target, long long max_cost) {
        if(!reachable(source, target, max_cost)) {
            return {}; 
        }

        int t = name_to_id_[target]; 
        int s = name_to_id_[source];
        vector<int> path{t}; 
        
        for(int p = cache_[s][t].parent; p != -1; p = cache_[s][p].parent) {
            path.push_back(p); 
        }

        vector<string> res; 
        res.reserve(path.size());
        for(int id = path.size() - 1; id >= 0; id--) {
            res.push_back(id_to_name_[path[id]]);
        }

        return res;
    }

private:
    static constexpr long long INF = LONG_LONG_MAX; 

    void shortest_path_from(int source) {
        if(!cache_[source].empty()) {
            return;
        }
        vector<NodeInfo> dists(adj_.size(), {INF, -2}); 
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> heap; 

        heap.push({0, source}); 
        dists[source] = {0, -1}; 

        while(!heap.empty()) {
            auto [dist_to, cur] = heap.top(); 
            heap.pop(); 
            if(dists[cur].cost < dist_to) {
                continue; 
            }

            for(const auto& [node, dist] : adj_[cur]) {
                long long new_dist = dist_to + dist; 
                if(new_dist < dists[node].cost) {
                    heap.push({new_dist, node}); 
                    dists[node] = {new_dist, cur}; 
                }
            }
        }
        cache_[source] = std::move(dists); 
    }

    int get_or_add_id(const string& name, int& id) {
        auto [it, inserted] = name_to_id_.try_emplace(name, id); 
        if(inserted) {
            id_to_name_.push_back(name);
            adj_.push_back({}); 
            id++; 
        }
        return it->second; 
    }
};

template< typename E, typename F>
void expect_error(F f) {
    try {
        f();
    } catch(const E&) {
        return; 
    }
    assert(false && "expect error"); 
}

void print_path(const vector<string>& path) {
    for(const auto& point : path) {
        cout << point << " "; 
    }
}
 
int main() {
    GraphTraversing g({{"A", "B", 3}, {"B", "C", 4}, {"A", "C", 10}, {"C", "A", 1}}); 

    assert(g.reachable("A", "C", 7));
    assert(!g.reachable("A", "C", 6));
    assert(g.reachable("C", "B", 4));
    assert(!g.reachable("B", "A", 4));
    assert(g.reachable("B", "A", 5));

    assert(g.reachable("A", "C", 100));
    assert(!g.reachable("A", "C", 6));
    assert(g.reachable("C", "A", 1));

    vector<string> res{"A", "B", "C"}; 
    print_path(g.cheapest_path("A", "C", 7));

    expect_error<invalid_argument>([&]{ g.reachable("E", "A", 1); }); 
    expect_error<invalid_argument>([&]{ g.reachable("A", "E", 1); }); 
    expect_error<invalid_argument>([&]{ g.reachable("A", "B", -1); }); 
    expect_error<invalid_argument>([]{ 
        GraphTraversing({{"A", "B", 1}, {"B", "C", -2}}); 
    }); 

    GraphTraversing g2({{"A", "B", 1}});
    assert(!g2.reachable("B", "A", LLONG_MAX));
}