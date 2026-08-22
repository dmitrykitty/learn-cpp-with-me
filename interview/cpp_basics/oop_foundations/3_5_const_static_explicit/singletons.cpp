//explain the difference between two below singletons and how does it work 
class SingletonObject {
    SingletonObject(){}
    static SingletonObject* INSTANCE;

    SingletonObject(const SingletonObject&) = delete;
    SingletonObject& operator=(const SingletonObject&) = delete; 

public: 
    static SingletonObject& get_instance() { 
        if(INSTANCE == nullptr){
            INSTANCE = new SingletonObject();
        }
        return *INSTANCE; 
    }
};

SingletonObject* SingletonObject::INSTANCE = nullptr; 

class SingletonObject2 {
private:
    SingletonObject2() = default;

public:
    SingletonObject2(const SingletonObject2&) = delete;
    SingletonObject2& operator=(const SingletonObject2&) = delete;

    static SingletonObject2& get_instance() {
        //what static mean here? when it will be destroyed? 
        static SingletonObject2 instance;
        return instance;
    }
};