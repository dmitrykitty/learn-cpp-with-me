#include <iostream>
#include <queue>
#include <memory>
#include <chrono>

using Timestamp = std::chrono::system_clock::time_point;

enum class Side : uint8_t {
    Sell, 
    Buy
};

enum class Symbol : uint8_t {
    NVDIA
};

class Message {
    long sequence; 
    Timestamp timespamp;
    Symbol symbol;

public:
    virtual ~Message() = default; 
};

class Trade final: public Message {
    double price{}; 
    long quantity{};

    ~Trade() override = default; 
};

class Quote final: public Message {
    double bid_price{};
    double ask_price{};
    long bid_quantity{}; 
    long asl_quantity{};

    ~Quote() override = default; 
};

class OrderAdded final: public Message {
    long order_id{};
    Side side{}; 
    double price{};
    long qiantity{};
};

class MessageProducer {
    inline static long sequence{0};
    static long generate_next() {
        return sequence++;
    }
public:
    static std::unique_ptr<Message> create_trade(double price, long quantity) {
        return std::make_unique<Trade>(
            generate_next(),
            std::chrono::system_clock::now(),
            price,
            quantity
        );
    }

    static std::unique_ptr<Message> create_quote() {

    }

    static std::unique_ptr<Message> create_order_added() {

    }
};
