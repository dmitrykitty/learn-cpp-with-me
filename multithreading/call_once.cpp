#include <mutex>
#include <thread>

int* xptr = nullptr; 
std::mutex mtx; 

//works but to restrictive, each time blocking 
int f1() {
    {
        std::lock_guard lk(mtx); 
        if(!xptr) {
            xptr = new int{}; 
        }
    }
    return *xptr * *xptr;
}

//pattern double checked locking 
//it works, but it is UB - data race 
int f2() {
    if(!xptr) { //<---- T1
        std::lock_guard lk(mtx); 
        if(!xptr) {
            xptr = new int{}; //<---- T2
        }
    }
    return *xptr * *xptr;
}

//no data race 
std::once_flag res_flag; 
int f3() {
    std::call_once(res_flag, []{ xptr = new int{}; }); 
    return *xptr * *xptr; 
}

int main() {

}