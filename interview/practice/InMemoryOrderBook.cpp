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


class OrderBook {
    std::map<int, std::set<OrderId>> buy_orders_; 
    std::map<int, std::set<OrderId>> sell_orders_; 
    std::unordered_map<OrderId, Order> orders; 

public:
    void addOrder(const Order& order) {
        if(order.side == Side::Buy) {
            buy_orders_[order.price].insert(order.id);
        } else {
            sell_orders_[order.price].insert(order.id);
        }
        orders[order.id] = order;
    }

    std::optional<int> bestBid() const {
        if(!buy_orders_.empty()) {
            return buy_orders_.rbegin()->first;
        }
        return std::nullopt;
    }
    std::optional<int> bestAsk() const {
        if(!sell_orders_.empty()) {
            return sell_orders_.begin()->first;
        }
        return std::nullopt;
    }

    //O(logN
    bool cancel_order(OrderId id) {
        if(auto order_pos = orders.find(id); order_pos != orders.end()) {
            //assume if order present in orders -> present in buy/sell orders
            const Order& order = order_pos->second; 
            auto& order_map = order.side == Side::Buy ? buy_orders_ : sell_orders_; 

            auto price_pos = order_map.find(order.price);
            price_pos->second.erase(order.id); 

            if(price_pos->second.empty()) {
                order_map.erase(price_pos); 
            }
            orders.erase(order.id); 
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