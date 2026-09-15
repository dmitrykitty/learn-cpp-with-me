#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <optional>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <list>


using OrderId = std::uint64_t;

enum class Side {
    Buy,
    Sell
};

struct Order {
    OrderId id;
    Side side;
    int price;
    int quantity;
};

struct OrderMeta{
    Order order;
    std::list<OrderId>::iterator pos; 
};


class OrderBook {
    std::map<int, std::list<OrderId>> buy_orders_; 
    std::map<int, std::list<OrderId>> sell_orders_; 
    std::unordered_map<OrderId, OrderMeta> orders; 

    std::map<int, std::list<OrderId>>& get_orders_by_side(Side side) {
        return side == Side::Buy ? buy_orders_ : sell_orders_; 
    }

public:
    //O(logN)
    void addOrder(const Order& order) {
        auto& order_map = get_orders_by_side(order.side);
        auto& lst = order_map[order.price]; //O(logN)

        lst.push_back(order.id); //O(1)
        auto last_pos = lst.end();
        last_pos--; 

        orders.emplace(order.id, OrderMeta{.order = order, .pos = last_pos}); //O(1)
    }

    //O(1)
    std::optional<int> bestBid() const {
        if(!buy_orders_.empty()) {
            return buy_orders_.rbegin()->first;
        }
        return std::nullopt;
    }

    //O(1)
    std::optional<int> bestAsk() const {
        if(!sell_orders_.empty()) {
            return sell_orders_.begin()->first;
        }
        return std::nullopt;
    }

    //O(logN)
    bool cancel_order(OrderId id) {
        //O(1)
        if(auto order_pos = orders.find(id); order_pos != orders.end()) {
            //assume if order present in orders -> present in buy/sell orders
            const OrderMeta& order_meta = order_pos->second; 
            auto& order_map = order_meta.order.side == Side::Buy ? buy_orders_ : sell_orders_; 

            //O(logN)
            auto price_pos = order_map.find(order_meta.order.price);
            price_pos->second.erase(order_meta.pos); //O(1)

            //O(1)
            if(price_pos->second.empty()) {
                order_map.erase(price_pos); 
            }
            orders.erase(order_meta.order.id); //O(1)
            return true;
        } 
        return false;
    }
};

int main() {
    OrderBook book;

    book.addOrder(Order{1, Side::Buy, 100, 10});
    book.addOrder(Order{2, Side::Buy, 105, 20});
    book.addOrder(Order{3, Side::Sell, 110, 5});
    book.addOrder(Order{4, Side::Sell, 108, 7});

    if (auto bid = book.bestBid()) {
        std::cout << "Best bid: " << *bid << '\n';
    } else {
        std::cout << "No bids\n";
    }

    if (auto ask = book.bestAsk()) {
        std::cout << "Best ask: " << *ask << '\n';
    } else {
        std::cout << "No asks\n";
    }

    return 0;
}