#include <condition_variable>
#include <mutex>
#include <thread> 

//three main components 
int* resource = nullptr; //resource
std::mutex resmut; //mutex
std::condition_variable cond; //cond variable 

//waking up threads costs -> needed syscall so microseconds 

/*
spurious wake ups - the standard allows wait(lk) to return even if nobody called notify
*/

int use(int* val) {
    //....
    return *val; 
}

void producer() {
    //better to notify without lock. in this case 2nd thread will get lock and will not wake up second time 
    {
        std::unique_lock lock(resmut); 
        resource = new int(5);
    }
    cond.notify_one(); //wake up all wating  
}

void consumer() {
    std::unique_lock lock(resmut); //should has lock before wait, because lock returns after wake up 
    cond.wait(lock, []{  return resource != nullptr; }); //wait until resource != nullptr. predicate: while(!pred()) cond.wait();
    use(resource); 
}



int main() {

}