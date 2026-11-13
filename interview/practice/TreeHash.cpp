#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <string>
#include <deque>
#include <cassert>
#include <algorithm>

using namespace std; 

struct Node {
    int value;
    vector<Node*> children; 
};

class TreeHasher {
    Node* root;
    unordered_map<const Node*, size_t> hashes; 
    unordered_map<size_t, vector<const Node*>> groups; 
    vector<vector<const Node*>> duplicates; 
    
public:
    explicit TreeHasher(Node* node): root(node) {
        if(!node) {
            throw invalid_argument("empty tree is not allowed");
        }

        compute_hash(root); 
        for(const auto& [hash, group] : groups) {
            if(group.size() > 1) {
                duplicates.push_back(group); 
            }
        }
    }
    size_t hash(const Node* node) const {
        auto it = hashes.find(node); 
        if(it == hashes.end()) {
            throw invalid_argument("invalid node"); 
        }
        return it->second; 
    }

    const vector<vector<const Node*>>& duplicates_groups() const {
        return duplicates; 
    }

private: 
//format of the string node_value(child_hash1, child_hash2, ...)
    size_t compute_hash(const Node* node) {
        string s = to_string(node->value) + "("; 

        for(int i = 0 ; i < node->children.size(); ++i) {
            size_t child_hash = compute_hash(node->children[i]); 
            s += to_string(child_hash);
            if(i < node->children.size() - 1) {
                s += ","; 
            }
        }
        s += ")";
        size_t hash = std::hash<string>{}(s); 
        groups[hash].push_back(node); 
        return hashes[node] = hash; 
    }
};

struct TreeBuilder {
    deque<Node> pool;
    Node* make(int v, vector<Node*> children = {}) {
        pool.push_back(Node{v, std::move(children)});
        return &pool.back();
    }
};

template<typename E, typename F>
void expect_error(F f) {
    try {
        f();
    } catch(const E&) {
        return; 
    }
    assert(false && "exception wasn't thrown"); 
}

void compare_groups(vector<vector<const Node*>>& a, vector<vector<const Node*>>& b) {
    assert(a.size() == b.size()); 
    for(auto& group : a) {
        sort(group.begin(), group.end()); 
    }

    for(auto& group : b) {
        sort(group.begin(), group.end()); 
    }

    sort(a.begin(), a.end());
    sort(b.begin(), b.end());

    for(int i = 0; i < a.size(); ++i) {
        assert(a[i] == b[i]);
    }
}

int main() {
    TreeBuilder t;

    // Example from the statement
    Node* e = t.make(4);
    Node* f = t.make(4);
    Node* g = t.make(5);
    Node* b = t.make(2, {e});
    Node* c = t.make(2, {f});
    Node* d = t.make(2, {g});
    Node* a = t.make(1, {b, c, d});

    Node* xx = t.make(999);

    TreeHasher h(a); 
    assert(h.hash(b) == h.hash(c));
    assert(h.hash(e) == h.hash(f));
    assert(h.hash(b) != h.hash(d));
    assert(h.hash(e) != h.hash(g));
    for (Node* n : {b, c, d, e, f, g}) assert(h.hash(a) != h.hash(n));
    auto real = h.duplicates_groups(); 
    auto target = vector<vector<const Node*>>{{b, c}, {e,f}}; 
    compare_groups(real, target);

    Node* x = t.make(1, {t.make(2), t.make(3)});
    Node* y = t.make(1, {t.make(2, {t.make(3)})});
    assert(TreeHasher(x).hash(x) != TreeHasher(y).hash(y));

    Node* p = t.make(12);
    Node* q = t.make(1, {t.make(2)});
    assert(TreeHasher(p).hash(p) != TreeHasher(q).hash(q));

    Node* r = t.make(1, {t.make(3), t.make(2)});
    assert(TreeHasher(x).hash(x) != TreeHasher(r).hash(r));

    expect_error<invalid_argument>([] {TreeHasher(nullptr); }); 
    expect_error<invalid_argument>([&] {h.hash(xx); });

    Node* leaf = t.make(7);
    TreeHasher(leaf).hash(leaf);                         // a single-node tree works

    Node* x2 = t.make(1, {t.make(2), t.make(3)});        // same shape as x, separate nodes
    assert(TreeHasher(x).hash(x) == TreeHasher(x2).hash(x2));
}