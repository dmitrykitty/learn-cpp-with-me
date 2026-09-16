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

struct Trade {
    OrderId buy_id;
    OrderId sell_id;
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
    //O(log P + T)
    //if incoming BUY.price >= bestAsk -> BUY
    //if SELL.prcie <= bestBid -> SELL
    //best price -> oldest order (price-time priority)
    //example
    /*
    inside book:
    SELL:
    100 -> [id=1 qty=5]
    101 -> [(id=2 qty=10), (id=3 qty=8)]

    incoming Order{4, Side::Buy, 101, 12}

    return {    Trade{buy=4, sell=1, price=100, quantity=5, 
                Trade{buy=4, sell=2, price=101, quantity=7}     }

    inside book:
    SELL:
    101 -> [(id=2 qty=3), (id=3 qty=8)]
    */
    std::vector<Trade> addOrder(const Order& order) {
        return order.side == Side::Buy ? complete_bs_trade(order) : complete_sb_trade(order);
    }

private:
    std::vector<Trade> complete_bs_trade(const Order& order) {
        std::vector<Trade> trades; 
        int quantity_left = order.quantity;

        for(auto it = sell_orders_.begin(); it != sell_orders_.end(); ) {
            int price = it->first; 
            std::list<OrderId>& orders_ids = it->second; 

            if(price > order.price) {
                break;
            }

            while(quantity_left != 0 && !orders_ids.empty()) {
                auto id = orders_ids.front(); 
                auto& order_meta = orders[id]; 
                if(order_meta.order.quantity >= quantity_left) {
                    trades.emplace_back(order.id, order_meta.order.id, price, quantity_left); 
                    if(order_meta.order.quantity != quantity_left) {
                        order_meta.order.quantity-= quantity_left; 
                    } else {
                        orders_ids.pop_front(); 
                        orders.erase(id);
                    }
                    quantity_left = 0;
                    break;
                } else {
                    trades.emplace_back(order.id, order_meta.order.id, price, order_meta.order.quantity);
                    quantity_left -= order_meta.order.quantity;
                    orders_ids.pop_front(); 
                    orders.erase(id);
                }
            }
            if(orders_ids.empty()) {
                //returns next after erased
                it = sell_orders_.erase(it); 
            } else {
                it++;
            }

            if(quantity_left == 0) {
                return trades;
            }
        }

        if(quantity_left != 0) {
            Order order_to_add{order.id, order.side, order.price, quantity_left};
            addOrderToMaps(order_to_add);
        }

        return trades;
    }

    void addOrderToMaps(const Order& order) {
        auto& order_map = order.side == Side::Buy ? buy_orders_ : sell_orders_; 
        auto& lst_orders = order_map[order.price]; 
        lst_orders.push_back(order.id); 
        auto last_added = lst_orders.end();
        last_added--;
        orders.emplace(order.id, OrderMeta{order, last_added}); 
    } 



public:

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