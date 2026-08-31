#include <iostream>
#include <vector>
#include <memory>

class CalculatingStrategy {
    char unit; 
public:
    CalculatingStrategy(char unit): unit(unit) {}
    virtual double calculate_price(double, double) const = 0;
    virtual ~CalculatingStrategy() {
        std::cout << "Base strategy destroyed\n";
    }
    char get_unit() const noexcept {
        return unit;
    }

    
};

class FixedPriceStrategy final: public CalculatingStrategy {
public:
    FixedPriceStrategy(): CalculatingStrategy('$') {}
    double calculate_price(double cost, double price) const {
        return price - cost; 
    }

    ~FixedPriceStrategy() override {
        std::cout << "FixedPriceStrategy strategy destroyed\n";
    }
};

class PercentageMarkupStrategy final: public CalculatingStrategy {
public:
PercentageMarkupStrategy(): CalculatingStrategy('%') {}
    double calculate_price(double cost, double price) const {
        return ((price - cost) / cost) * 100.0;
    }
    ~PercentageMarkupStrategy() override {
        std::cout << "PercentageMarkupStrategy strategy destroyed\n";
    }
};

class DiscountPriceStrategy final: public CalculatingStrategy {
    std::vector<double> discount_list;
public:
    DiscountPriceStrategy(std::vector<double> disc): CalculatingStrategy('$'), discount_list(std::move(disc)) {}
    double calculate_price(double cost, double price) const {
        double new_price = calculate_price_after_discount(price);
        return  new_price - cost; 
    }
    ~DiscountPriceStrategy() override{
        std::cout << "DiscountPriceStrategy strategy destroyed\n";
    }
private: 
    double calculate_price_after_discount(double price) const {
        for(auto discount : discount_list) {
            price -= price * discount;
        }
        return price;
    }
};

class PriceExecutor {
public:
    double get_metric(double cost, double price, const CalculatingStrategy& strategy) const {
        return strategy.calculate_price(cost, price);
    }
};

int main() {
    PriceExecutor pc;

    std::vector<std::unique_ptr<CalculatingStrategy>> strategies; 
    strategies.push_back(std::make_unique<FixedPriceStrategy>());
    strategies.push_back(std::make_unique<PercentageMarkupStrategy>());
    strategies.push_back(std::make_unique<DiscountPriceStrategy>(std::vector<double>{0.25, 0.13, 0.11}));

    for(const auto& strategy: strategies) {
        std::cout << pc.get_metric(15, 30, *strategy) << strategy->get_unit() << '\n';
        std::cout << "----------------------------\n";
    }
}