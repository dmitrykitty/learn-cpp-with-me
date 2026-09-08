#include <iostream>
#include <queue>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <limits>
#include <unordered_map>
#include <cmath>
#include <iomanip>

class MarketStateHandler;

using Timestamp = std::chrono::system_clock::time_point;

enum class Side : uint8_t {
    Sell, 
    Buy
};

enum class Symbol : uint8_t {
    NVDA
};

constexpr std::string_view to_string(Symbol symbol) {
    switch (symbol)
        case Symbol::NVDA:
            return "Nvidia";

    return "Unknown";
}

constexpr std::string_view to_string(Side side) {
    return side == Side::Buy ? "BUY" : "SELL";

}

std::string format_timestamp(const Timestamp& ts) {
    auto time = std::chrono::system_clock::to_time_t(ts);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        ts.time_since_epoch()
    ) % 1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss
        << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.'
        << std::setfill('0')
        << std::setw(3)
        << ms.count();

    return oss.str();
}

class Message {
public:
    long sequence; 
    Timestamp timestamp;
    Symbol symbol;
    std::string name;


    Message(long seq, Timestamp ts, Symbol sb, std::string nm)
        : sequence(seq)
        , timestamp(ts)
        , symbol(sb)
        , name(std::move(nm)) {}
    virtual ~Message() = default; 
};

class Trade final: public Message {
public:
    double price{}; 
    long quantity{};
    Trade(long seq, Timestamp ts, Symbol sb, std::string nm, double pr, long q)
        : Message(seq, ts, sb, nm)
        , price(pr)
        , quantity(q) {}

    ~Trade() override = default; 
};

class Quote final: public Message {
public:
    double bid_price{};
    double ask_price{};
    long bid_quantity{}; 
    long ask_quantity{};

    Quote(long seq, Timestamp ts, Symbol sb, std::string nm,
        double bpr,
        double apr,
        long bq, 
        long aq)
        : Message(seq, ts, sb, nm)
        , bid_price(bpr)
        , ask_price(apr)
        , bid_quantity(bq)
        , ask_quantity(aq) {}


    ~Quote() override = default; 
};

class OrderAdded final: public Message {
public:
    long order_id{};
    Side side{}; 
    double price{};
    long quantity{};
    OrderAdded(long seq, Timestamp ts, Symbol sb, std::string nm,
        long id,
        Side sd,
        double pr,
        long q)
        : Message(seq, ts, sb, nm)
        , order_id(id)
        , side(sd)
        , price(pr)
        , quantity(q) {}

    ~OrderAdded() override = default; 
};

class MessageProducer {
    inline static long sequence{0};
    static long generate_next() {
        return sequence++;
    }
public:
    static std::unique_ptr<Message> create_trade(Symbol symbol, double price, long quantity) {
        return std::make_unique<Trade>(
            generate_next(),
            std::chrono::system_clock::now(),
            symbol,
            "TRADE",
            price,
            quantity
        );
    }

    static std::unique_ptr<Message> create_quote(
        Symbol symbol,
        double bid_price,
        double ask_price,
        long bid_quantity, 
        long ask_quantity
    ) {
        return std::make_unique<Quote>(
            generate_next(),
            std::chrono::system_clock::now(),
            symbol,
            "QUOTE",
            bid_price,
            ask_price,
            bid_quantity,
            ask_quantity
        );

    }

    static std::unique_ptr<Message> create_order_added(
        Symbol symbol,
        Side side,
        double price,
        long quantity
    ) {
        return std::make_unique<OrderAdded>(
            generate_next(),
            std::chrono::system_clock::now(),
            symbol, 
            "ORDER ADDED", 
            rand(),
            side,
            price,
            quantity
        );

    }
};


class MessageLogger {  
    MessageLogger() = default;

    void log(const long sequence,const Timestamp& ts, std::string_view name) {
        std::cout << "[" << sequence << "] " << name  << "  created at [" <<  format_timestamp(ts) << "]\n"; 
    }

public:
    MessageLogger(const MessageLogger&) = delete; 
    MessageLogger& operator=(const MessageLogger&) = delete; 

    static MessageLogger& logger() {
        static MessageLogger logger_; 
        return logger_;
    }

    void log(const Trade& trade) {
        log(trade.sequence, trade.timestamp, trade.name); 
        std::cout << to_string(trade.symbol) << " price=" << trade.price << " qty=" << trade.quantity << std::endl; 
    }

    void log(const Quote& quote) {
        log(quote.sequence, quote.timestamp, quote.name); 
        std::cout 
            << to_string(quote.symbol) 
            << " bid=" << quote.bid_price << "x" << quote.bid_quantity 
            << " ask=" << quote.ask_price << "x" << quote.ask_quantity << std::endl; 
    }

    void log(const OrderAdded& order) {
        log(order.sequence, order.timestamp, order.name);
        std::cout 
            << order.order_id << " -> " << to_string(order.side) << " " 
            << order.quantity << " @ " << order.price << std::endl; 
    }
};


class MarketStateHandler {
    MessageLogger& logger = MessageLogger::logger();
    
    void handle_trade(const Trade& trade) {
        last_trade_price = trade.price;
        total_volume += trade.quantity;
        total_notional += trade.price * trade.quantity;
        logger.log(trade);
    }

    void handle_quote(const Quote& quote) {
        bid_price = quote.bid_price;
        bid_quantity = quote.bid_quantity;
        ask_price = quote.ask_price;
        ask_quantity = quote.ask_quantity;
        logger.log(quote);
    }

    void handle_order_added(const OrderAdded& order) {
        if(order.side == Side::Buy) {
            best_bid = std::max(order.price, best_bid); 
        } else {
            best_ask = std::min(order.price, best_ask); 
        }
        active_orders.insert_or_assign(order.order_id, order);
        logger.log(order);
    }

public:
    double last_trade_price{-1}; 
    long total_volume{0}; 
    double total_notional{0}; 

    double best_bid{-1};
    double best_ask{std::numeric_limits<double>().max()};
    double bid_price{-1};
    long bid_quantity{0}; 
    double ask_price{-1};
    long ask_quantity{0}; 

    std::unordered_map<long, OrderAdded> active_orders; 

    void handle_message(const Message& message) {
        
        if(auto trade = dynamic_cast<const Trade*>(&message); trade != nullptr) {
            handle_trade(*trade);
        } else if(auto order_added = dynamic_cast<const OrderAdded*>(&message); order_added != nullptr) {
            handle_order_added(*order_added);
        } else if(auto quote = dynamic_cast<const Quote*>(&message); quote != nullptr) {
            handle_quote(*quote);
        }
    }
};

int main() {
    std::srand(42); // deterministic-ish IDs for testing

    MarketStateHandler handler;

    std::vector<std::unique_ptr<Message>> messages;

    // Someone wants to buy 100 NVDA @ 120.00
    messages.push_back(
        MessageProducer::create_order_added(
            Symbol::NVDA,
            Side::Buy,
            120.00,
            100
        )
    );

    // Someone wants to sell 50 NVDA @ 121.00
    messages.push_back(
        MessageProducer::create_order_added(
            Symbol::NVDA,
            Side::Sell,
            121.00,
            50
        )
    );

    // Current top-of-book quote
    messages.push_back(
        MessageProducer::create_quote(
            Symbol::NVDA,
            120.00,  // bid price
            121.00,  // ask price
            100,     // bid quantity
            50       // ask quantity
        )
    );

    // 20 shares actually trade @ 121.00
    messages.push_back(
        MessageProducer::create_trade(
            Symbol::NVDA,
            121.00,
            20
        )
    );

    // Another 30 shares trade @ 120.80
    messages.push_back(
        MessageProducer::create_trade(
            Symbol::NVDA,
            120.80,
            30
        )
    );

    std::cout << "\n========== PROCESSING ==========\n\n";

    for (const auto& message : messages) {
        handler.handle_message(*message);
        std::cout << '\n';
    }

    std::cout << "\n========== FINAL STATE ==========\n";

    std::cout << "Last trade price: "
              << handler.last_trade_price << '\n';

    std::cout << "Total volume: "
              << handler.total_volume << '\n';

    std::cout << "Total notional: "
              << handler.total_notional << '\n';

    std::cout << "\nQuote:\n";

    std::cout << "Bid: "
              << handler.bid_price
              << " x "
              << handler.bid_quantity
              << '\n';

    std::cout << "Ask: "
              << handler.ask_price
              << " x "
              << handler.ask_quantity
              << '\n';

    std::cout << "\nBest prices derived from orders:\n";

    std::cout << "Best bid: "
              << handler.best_bid << '\n';

    std::cout << "Best ask: "
              << handler.best_ask << '\n';

    std::cout << "\nActive orders:\n";

    for (const auto& [id, order] : handler.active_orders) {
        std::cout
            << id
            << " -> "
            << to_string(order.side)
            << " "
            << order.quantity
            << " @ "
            << order.price
            << '\n';
    }
}