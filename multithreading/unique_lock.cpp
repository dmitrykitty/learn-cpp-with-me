#include <mutex>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>
#include <utility>
#include <cassert>

/*
std::unique_lock owns a mutex
has bool owns flag inside 
can be locked, unlocked multiple times, moved
possible flags defer_lock, try_to_lock, adopt_lock
*/

//Used to lock and unlock and then locvk again 

std::vector<int> pending; 
std::mutex m; 
long processed = 0; 

void process_batch() {
    std::vector<int> batch;

    std::unique_lock lock(m);
    batch.swap(pending); 
    lock.unlock(); 

    for(auto& o : batch) {
        //slow validation of the batch 
    }

    lock.lock(); 
    processed += batch.size(); 
    //unlock in destructor 
}

//try lock without waiting
void maybe_flush() {
    std::unique_lock lock(m, std::try_to_lock);   // calls m.try_lock()
    if (!lock.owns_lock()) {
        return;              // another thread is flushing: skip instead of waiting
    }
    //flush()
}  

//move lock 
std::mutex book_mutex; 
std::unique_lock<std::mutex> lock_book() {
    std::unique_lock lock(book_mutex);
    //load_if_needed();
    return lock;                       // ownership moves to the caller, mutex stays locked
}
void reader() {
    auto lock = lock_book(); // the caller now owns the lock
    //do something
}


using Mutex = std::mutex; 
class my_unique_lock {
    Mutex* m_ = nullptr; 
    bool owns_ = false; 
public: 
    explicit my_unique_lock(Mutex& m): m_(&m), owns_(true) {
        m_->lock(); 
    }
    my_unique_lock(Mutex& m, std::adopt_lock_t) noexcept: m_(&m),owns_(true) {}
    my_unique_lock(Mutex& m, std::try_to_lock_t): m_(&m), owns_(m.try_lock()) {}
    my_unique_lock(Mutex& m, std::defer_lock_t) noexcept: m_(&m), owns_(false) {}

    my_unique_lock(const my_unique_lock&) = delete; 
    my_unique_lock& operator=(const my_unique_lock&) = delete;

    my_unique_lock(my_unique_lock&& o) noexcept : 
        m_(std::exchange(o.m_, nullptr)), 
        owns_(std::exchange(o.owns_, false)) {}

    my_unique_lock& operator=(my_unique_lock&& o) noexcept {
        if(this == &o) return *this; 

        if(owns_) m_->unlock(); 
        m_ = std::exchange(o.m_, nullptr);
        owns_ = std::exchange(o.owns_, false);
        return *this; 
    }


    Mutex* mutex() const noexcept {
        return m_; 
    }

    ~my_unique_lock() {
        if(owns_) m_->unlock(); 
    }

    void lock() {
        if (!m_) throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
        if(owns_) throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur));

        m_->lock(); 
        owns_ = true; 
    }

    void unlock() {
        if(!owns_) throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
        
        m_->unlock(); 
        owns_ = false; 
    }

    bool try_lock() {
        if (!m_) throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
        if (owns_) throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur));
        owns_ = m_->try_lock(); 
        return owns_; 
    }

    bool owns_lock() const noexcept {
        return owns_; 
    }
};

struct Account { std::mutex m; long balance = 0; };

void transfer(Account& from, Account& to, long x) {
    my_unique_lock lf(from.m, std::defer_lock);
    my_unique_lock lt(to.m, std::defer_lock);
    std::lock(lf, lt);              // needs lock/unlock/try_lock on your class
    from.balance -= x;
    to.balance += x;
}

my_unique_lock lock_and_return(std::mutex& m) {
    my_unique_lock lk(m);
    return lk;                      // needs the move constructor
}


int main() {
    Account a, b; 
    std::thread t1(transfer, std::ref(a), std::ref(b), 10); 
    std::thread t2(transfer, std::ref(b), std::ref(a), 10); 
    std::thread t3(transfer, std::ref(a), std::ref(b), 10); 

    t1.join(); t2.join(); t3.join(); 
    assert(a.balance == -10 && b.balance == 10); 
}