#include <cstdint>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cassert>
#include <vector>

using namespace std; 

enum class Side : uint16_t {
    BUY,
    SELL
}; 

struct Order {
    int id; 
    Side side; 
    int price; 
    int quantity; 
}; 

using list_iterator = list<Order>::iterator; 


struct Level {
    list<Order> orders; 
    int quantity{}; 

    void add(const Order& order) {
        quantity += order.quantity; 
        orders.push_back(order); 
    }

    void erase(list_iterator it) {
        quantity -= it->quantity; 
        orders.erase(it);
    }

    void reduce(int q, Order& order) {
        quantity -= q; 
        order.quantity -= q; 
    }
};

struct LevelData {
    int orders_count; 
    int quantity; 
    int price; 
};

using level_map = map<int, Level>; 

struct Trade {
    int buy_id; 
    int sell_id; 
    int price; 
    int quantity; 
}; 

class OrderBook {
    unordered_map<int, list_iterator> orders_; 
    level_map buy_orders_; 
    level_map sell_orders_; 

    level_map& get_levels_by_side(Side side) {
        return side == Side::SELL ? sell_orders_ : buy_orders_; 
    } 
    level_map& get_opposite_levels_by_side(Side side) {
        return side == Side::BUY ? sell_orders_ : buy_orders_;
    }

    level_map::iterator get_best_opposite_level(level_map& levels, Side side) {
        if(levels.empty()) {
            return levels.end(); 
        }
        return side == Side::BUY ? levels.begin() : prev(levels.end()); 
    }

    level_map::iterator get_best_level(level_map& levels, Side side) {
        if(levels.empty()) {
            return levels.end(); 
        }
        return side == Side::BUY ? prev(levels.end()) : levels.begin(); 
    }

    level_map::iterator next_by_side(Side side, level_map::iterator it) {
        return side == Side::BUY ? prev(it) : next(it); 
    }

    bool crosses(int price, Side side, int level_price) {
        return side == Side::BUY ? level_price <= price : level_price >= price; 
    }

public: 
    vector<Trade> add_order(const Order& new_order) {
        if(orders_.contains(new_order.id) || new_order.price <= 0 || new_order.quantity <= 0) {
            return {};
        }

        int quantity = new_order.quantity; 
        auto& levels = get_opposite_levels_by_side(new_order.side); 
        vector<Trade> trades; 

        while(quantity != 0) {
            auto it = get_best_opposite_level(levels, new_order.side); 
            if(it == levels.end()) {
                break;
            }

            int level_price = it->first; 
            auto& level = it->second; 
            if(!crosses(new_order.price, new_order.side, level_price)) {
                break; 
            }
            
            while(quantity != 0 && !level.orders.empty()) {
                auto& order = level.orders.front(); 
                int minq = min(quantity, order.quantity); 

                quantity -= minq; 
                level.reduce(minq, order); 

                trades.push_back(Trade{
                    new_order.side == Side::BUY ? new_order.id : order.id, 
                    new_order.side == Side::SELL ? new_order.id : order.id, 
                    level_price, 
                    minq
                }); 

                if(order.quantity == 0) {
                    orders_.erase(order.id); 
                    level.orders.erase(level.orders.begin()); 
                }
            }

            if(level.orders.empty()) {
                levels.erase(it); 
            }
        }

        if(quantity != 0) {
            auto& levels = get_levels_by_side(new_order.side); 
            auto& level = levels[new_order.price]; 
            level.add(Order{new_order.id, new_order.side, new_order.price, quantity}); 
            orders_[new_order.id] = prev(level.orders.end()); 
        }

        return trades;
    }

    bool cancel_order(int id) {
        auto it = orders_.find(id); 
        if(it == orders_.end()) {
            return false;
        }

        auto order_it = it->second; 
        orders_.erase(it); 

        auto& levels = get_levels_by_side(order_it->side); 
        auto level_it = levels.find(order_it->price);
        auto& level = level_it->second; 
        level.erase(order_it); 
        if(level_it->second.orders.size() == 0) {
            levels.erase(level_it); 
        } 
        return true;
    }

    vector<LevelData> depth(int n, Side side) {
        auto& levels = get_levels_by_side(side); 
        vector<LevelData> res; 

        if(n <= 0) {
            return res; 
        }

        int res_length = min(n, (int)levels.size()); 
        res.reserve(res_length); 

        auto it = get_best_level(levels, side); 
        for(int i = 0; i < res_length; ++i) {
            if (i > 0) it = next_by_side(side, it);

            auto& level = it->second; 
            res.push_back(LevelData{(int)level.orders.size(), level.quantity, it->first}); 
        }
        return res; 
    }

    bool reduce(int id, int qty) {
        auto it = orders_.find(id); 
        if(qty <= 0 || it == orders_.end()) {
            return false;
        }


        auto& order = *it->second; 
        if(order.quantity <= qty) {
            cancel_order(id);
        } else {
            get_levels_by_side(order.side).at(order.price).reduce(qty, order); 
        }
        return true;
    }


    int best_bid() const {
        if(buy_orders_.empty()) {
            throw invalid_argument("no buy orders is present"); 
        }
        return prev(buy_orders_.end())->first; 
    }

    int best_ask() const {
        if(sell_orders_.empty()) {
            throw invalid_argument("no buy orders is present"); 
        }
        return sell_orders_.begin()->first; 
    }
};

int main() {
    OrderBook ob1; 

    //should throw if now orders are present
    bool thrown = false; 
    try {
        ob1.best_ask();
    } catch(exception& e) {
        thrown = true; 
    }
    assert(thrown);

 
    ob1.add_order(Order{1, Side::BUY, 5, 5}); 
    ob1.add_order(Order{2, Side::BUY, 10, 5}); 
    // empty vector of trades expected
    assert(ob1.add_order(Order{3, Side::BUY, 15, 5}).empty()); 
    assert(ob1.best_bid() == 15);
    auto ld1 = ob1.depth(3, Side::BUY);
    assert(ld1[0].orders_count == 1 && ld1[0].price == 15 && ld1[0].quantity == 5 &&
            ld1[1].orders_count == 1 && ld1[1].price == 10 && ld1[1].quantity == 5 && 
            ld1[2].orders_count == 1 && ld1[2].price == 5 && ld1[0].quantity == 5
    ); 

    //full cover trade 
    auto trade1 = ob1.add_order(Order{4, Side::SELL, 5, 5}); 
    assert(trade1.size() == 1 && trade1[0].price == 15 && trade1[0].sell_id == 4 && trade1[0].buy_id == 3 && trade1[0].quantity == 5); 
    assert(!ob1.cancel_order(3)); //should be removed by previous operation 
    assert(ob1.best_bid() == 10);
    
     //not fully covered
    auto trade2 = ob1.add_order(Order{5, Side::SELL, 10, 7}); 
    assert(trade2.size() == 1 && trade2[0].price == 10 && trade2[0].sell_id == 5 && trade2[0].buy_id == 2 && trade2[0].quantity == 5); 
    assert(!ob1.cancel_order(2)); //should be removed by previous operation 
    assert(ob1.best_bid() == 5);
    assert(ob1.best_ask() == 10); 

    //cancel
    assert(ob1.add_order(Order{6, Side::BUY, 1, 1}).empty()); 
    assert(ob1.cancel_order(6)); 
    assert(!ob1.cancel_order(6)); 

    //multiple orders cover 
    OrderBook ob2; 
    ob2.add_order({1, Side::BUY, 15, 5});
    ob2.add_order({2, Side::BUY, 15, 5});
    ob2.add_order({3, Side::BUY, 15, 5});

    auto ld2 = ob2.depth(1, Side::BUY);
    assert(ld2[0].orders_count == 3 && ld2[0].price == 15 && ld2[0].quantity == 15); 

    auto trade3 = ob2.add_order({4, Side::SELL, 10, 12}); 
    assert(trade3.size() == 3 && 
        trade3[0].quantity == 5 && trade3[0].buy_id == 1 &&
        trade3[1].quantity == 5 && trade3[1].buy_id == 2 && 
        trade3[2].quantity == 2 && trade3[2].buy_id == 3
    ); 
    assert(ob2.best_bid() == 15); 

    //multiple levels cover 
    OrderBook ob3; 
    ob3.add_order({1, Side::BUY, 10, 5});
    ob3.add_order({2, Side::BUY, 15, 5});
    ob3.add_order({3, Side::BUY, 20, 5});

    auto trade4 = ob3.add_order({4, Side::SELL, 10, 12});
    assert(trade4.size() == 3 && 
        trade4[0].quantity == 5 && trade4[0].buy_id == 3 && trade4[0].price == 20 &&
        trade4[1].quantity == 5 && trade4[1].buy_id == 2 && trade4[1].price == 15 &&
        trade4[2].quantity == 2 && trade4[2].buy_id == 1 && trade4[2].price == 10
    ); 
    assert(ob3.best_bid() == 10); 

    OrderBook ob4; 
    ob4.add_order({1, Side::SELL, 10, 1});
    ob4.add_order({2, Side::SELL, 15, 2});
    ob4.add_order({3, Side::SELL, 15, 3});
    ob4.add_order({4, Side::SELL, 20, 4});
    ob4.add_order({5, Side::SELL, 20, 5});
    ob4.add_order({6, Side::SELL, 20, 6});

    auto ld3 = ob4.depth(5, Side::SELL);
    assert(ld3.size() == 3); 
    assert(ld3[0].orders_count == 1 && ld3[0].price == 10 && ld3[0].quantity == 1 &&
            ld3[1].orders_count == 2 && ld3[1].price == 15 && ld3[1].quantity == 5 && 
            ld3[2].orders_count == 3 && ld3[2].price == 20 && ld3[2].quantity == 15
    );
}