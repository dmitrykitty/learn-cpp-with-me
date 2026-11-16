#include <mutex>
#include <thread>
#include <vector>

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


int main() {

}