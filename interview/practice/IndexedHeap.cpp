#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <assert.h>

/*
Requirements:
insert      O(log n)
update      O(log n)
erase       O(log n)
contains    O(1) average
top         O(1)

----------------------

invariant: pos_[heap_[i].id] == i
         a
      a     a
    a  a   a  a
   a aa a a aa a
*/

struct Node {
    int id{};
    int priority{};
};

class IndexedHeap {
    std::vector<Node> heap_;
    std::unordered_map<int, std::size_t> pos_;

    void validate_id_priority(int id, int priority) {
        if(id < 0) {
            throw std::invalid_argument("id shpuld be non-negative");
        }
        if(priority < 0) {
            throw std::invalid_argument("priority shpuld be non-negative");
        }
    }

    void swap_nodes(std::size_t a, std::size_t b) {
        pos_[heap_[a].id] = b; 
        pos_[heap_[b].id] = a; 
        std::swap(heap_[a].priority, heap_[b].priority); 
        std::swap(heap_[a].id, heap_[b].id);
    }

    void sift_up(std::size_t idx) {
        while(idx > 0) {
            std::size_t parent_idx = (idx - 1) / 2;

            if(heap_[idx].priority <= heap_[parent_idx].priority) {
                break;
            } 
            swap_nodes(parent_idx, idx); 
            idx = (idx - 1)/ 2;
        }    
    }

    void sift_down(std::size_t idx) {
        std::size_t limit = (heap_.size() - 1) / 2; 
        while(idx < limit) {
            std::size_t max = idx * 2 + 1; 
            std::size_t right = idx * 2 + 2; 

            if(right < heap_.size()) {
                max = heap_[max].priority > heap_[right].priority ? max : right;
            } 

            if(heap_[max].priority <= heap_[idx].priority) {
                break;
            }
            swap_nodes(idx, max);
            idx = max; 
        }
    }

public:
    void insert(int id, int priority) {
        validate_id_priority(id, priority);
        heap_.push_back({id, priority}); 
        pos_[id] = heap_.size() - 1;
        sift_up(heap_.size() - 1); 
    }

    void update(int id, int new_priority) {
        validate_id_priority(id, new_priority);

        auto it = pos_.find(id); 
        if(it == pos_.end()) {
            throw std::invalid_argument("no element with provided id exists"); 
        }

        auto idx = it->second; 
        auto old_priority = heap_[idx].priority; 
        heap_[idx].priority = new_priority; 

        if(new_priority > old_priority) {
            sift_up(idx);
        } else {
            sift_down(idx); 
        }
    }

    void erase(int id) {
        auto it = pos_.find(id); 
        if(it == pos_.end()) {
            throw std::invalid_argument("no element with provided id exists"); 
        }

        auto idx = it->second; 
        auto last_idx = heap_.size() - 1; 
        //last element 
        if(last_idx == idx) {
            heap_.pop_back(); 
            pos_.erase(id); 
            return; 
        }

        auto old_priority = heap_[idx].priority;
        auto new_priority = heap_[last_idx].priority;
        
        swap_nodes(idx, last_idx); 
        heap_.pop_back(); 
        pos_.erase(id); 
        
        if(new_priority > old_priority) {
            sift_up(idx);
        } else {
            sift_down(idx); 
        }
    }

    const Node& top() const {
        if(heap_.empty()) {
            throw std::invalid_argument("top call on empty heap");
        }
        return heap_[0]; 
    }

    std::size_t size() const {
        return heap_.size();
    }

    bool empty() const {
        return heap_.empty();
    }

    bool contains(int id) const {
        return pos_.find(id) != pos_.end();
    }
};


int main() {
    IndexedHeap heap; 
    heap.insert(1, 5);
    heap.insert(2, 7);
    heap.insert(3, 2);
    heap.insert(4, 10);
    assert(heap.contains(1)); 
    assert(heap.top().id == 4 && heap.top().priority == 10); 
    heap.erase(4); 
    assert(heap.top().id == 2 && heap.top().priority == 7); 
    heap.insert(5, 15);
    assert(heap.top().id == 5 && heap.top().priority == 15); 
}