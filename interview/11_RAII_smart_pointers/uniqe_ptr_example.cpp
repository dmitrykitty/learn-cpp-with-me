#include <iostream>
#include <memory>

//unique_ptr the same fast and light as row pointer 
//not null uptr always own something
//moving transfer owning from source pointer to destination pointer 

template<typename T>
class UniquePtr {
    T* ptr; 
public: 
    UniquePtr(T* p = nullptr): ptr(p) {}
    UniquePtr(UniquePtr&& o) noexcept
        : ptr(o.ptr) {
        o.ptr = null;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    T* get() const {
        return ptr; 
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    void swap(UniquePtr& other) {
        std::swap(ptr, other.ptr);
    }

    ~UniquePtr() {
        delete ptr;
    }

};

/*
automatic storage                  dynamic storage

+------------------+              +------------------+
| p : Widget*      | -----------> |      Widget      |
+------------------+              +------------------+

   pointer object                     Widget object
*/
class Widget {

};

void widget_memory_leak() {
    Widget* p = new Widget(); 

    if(true) {
        //problem, memory leak 
        return; 
    }
    delete p; 
}

void widget_no_memory_leak() {
    std::unique_ptr<Widget> p(new Widget);
    //will be deleted after leaving the scope
}

int main() {
    

}