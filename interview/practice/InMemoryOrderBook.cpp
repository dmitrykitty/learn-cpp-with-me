#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <optional>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>

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

enum class OrderBookError : uint8_t {
    NoOppositeLevelError
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



using level_map = std::map<int, std::list<OrderId>>;
using order_map = std::unordered_map<OrderId, OrderMeta>; 


class OrderBook {
    level_map buy_orders_; 
    level_map sell_orders_; 
    order_map orders; 

    level_map& get_opposite_levels(Side side) {
        return side == Side::Buy ? sell_orders_ : buy_orders_; 
    }

    level_map& get_by_side_levels(Side side) {
        return side == Side::Buy ? buy_orders_ : sell_orders_; 
    }

    std::optional<level_map::iterator> get_best_opposite_level(level_map& levels, Side side) {
        if(levels.empty()) {
            return std::nullopt; 
        }
        return side == Side::Buy ? levels.begin() : std::prev(levels.end());
    }

    bool is_wrong_price(int existing_price, int incoming_price, Side side) {
        return side == Side::Buy ? (existing_price > incoming_price) : (existing_price < incoming_price);
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
    std::vector<Trade> add_order(const Order& order) {
        std::vector<Trade> trades; 
        int quantity_left = order.quantity;
        auto& levels = get_opposite_levels(order.side);

        while (quantity_left != 0) {
            auto maybe_best_level = get_best_opposite_level(levels, order.side);
            if(!maybe_best_level) {
                break;
            }

            level_map::iterator it = *maybe_best_level;
            int price = it->first; 
            std::list<OrderId>& orders_ids = it->second; 

            if(is_wrong_price(price, order.price, order.side)) {
                break;
            }

            while(quantity_left != 0 && !orders_ids.empty()) {
                auto id = orders_ids.front(); 
                auto& order_meta = orders.at(id); 

                auto possible_quant = std::min(quantity_left, order_meta.order.quantity); 
                trades.push_back(Trade {
                    order.side == Side::Buy ? order.id : order_meta.order.id, 
                    order.side == Side::Buy ? order_meta.order.id : order.id, 
                    price, 
                    possible_quant 
                }); 
                quantity_left -= possible_quant; 
                order_meta.order.quantity -= possible_quant;

                if(order_meta.order.quantity == 0) {
                    orders_ids.pop_front(); 
                    orders.erase(id);
                }
            }

            if(orders_ids.empty()) {
                //returns next after erased
                levels.erase(it); 
            }
        }

        if(quantity_left != 0) {
            Order order_to_add{order.id, order.side, order.price, quantity_left};
            add_order_buy_side(order_to_add, get_by_side_levels(order.side));
        }

        return trades;
    }

    void add_order_buy_side(const Order& order, level_map& levels) {
        auto& lst_orders = levels[order.price]; 
        lst_orders.push_back(order.id); 
        auto last_added = lst_orders.end();
        last_added--;
        orders.emplace(order.id, OrderMeta{order, last_added}); 
    } 

    //O(1)
    std::optional<int> best_bid() const {
        if(!buy_orders_.empty()) {
            return buy_orders_.rbegin()->first;
        }
        return std::nullopt;
    }

    //O(1)
    std::optional<int> best_ask() const {
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
            auto& levels = get_by_side_levels(order_meta.order.side); 

            //O(logN)
            auto price_pos = levels.find(order_meta.order.price);
            price_pos->second.erase(order_meta.pos); //O(1)

            //O(1)
            if(price_pos->second.empty()) {
                levels.erase(price_pos); 
            }
            orders.erase(order_meta.order.id); //O(1)
            return true;
        } 
        return false;
    }
};

int main() {
    OrderBook book;

    book.add_order(Order{1, Side::Buy, 100, 10});
    book.add_order(Order{2, Side::Buy, 105, 20});
    book.add_order(Order{3, Side::Sell, 110, 5});
    book.add_order(Order{4, Side::Sell, 108, 7});

    if (auto bid = book.best_bid()) {
        std::cout << "Best bid: " << *bid << '\n';
    } else {
        std::cout << "No bids\n";
    }

    if (auto ask = book.best_ask()) {
        std::cout << "Best ask: " << *ask << '\n';
    } else {
        std::cout << "No asks\n";
    }

    return 0;
}