#include <bits/stdc++.h>
#include <mutex>
#include <optional>


//Find problems and fix it 
class PriceCache {
    std::unordered_map<std::string, double> prices_;
    //mutable
    std::mutex m_;
public:
    //return reference, no const
    const double& get(const std::string& sym) {
        std::lock_guard lock(m_);
        //[] create new key if wasn't present 
        return prices_[sym];
    }

    void update(const std::string& sym, double p) {
        m_.lock();
        //early return without unlock
        if (p <= 0) return;
        prices_[sym] = p;
        m_.unlock();
    }
    //no lock, no const
    //has and get should be unioned
    bool has(const std::string& sym) {
        return prices_.count(sym) > 0;
    }
};



class PriceCacheFixed {
    std::unordered_map<std::string, double> prices_;
    mutable std::mutex m_;
public:
    std::optional<double> get_if_present(const std::string& sym) const {
        std::lock_guard lock(m_);
        auto it = prices_.find(sym); 
        if(it == prices_.end()) return std::nullopt; 

        return it->second; 
    }

    bool update(const std::string& sym, double p) {
        if (p <= 0) return false;
        std::lock_guard lock(m_);
        prices_[sym] = p;
        return true; 
    }
};