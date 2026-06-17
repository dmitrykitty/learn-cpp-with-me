#include <iostream>
//PRIVATE AND PUBLIC 
//protected will be discussed later during inheritance 
class C {
    int x = 5;
};

class A {
public: 
    int x; 
private:
    int y; 
};

//=================================================
//friends functions - not members of the class, but get access to the class members 
class C {
private:
    int x{3};
public:
    //could be everywhere doesnt matter 
    friend void g2(const C& c, int y);

    //but we can define function
    //so it's external function for class C but defined inside it 
    friend void g3(const C& c, int y){
        std::cout << c.x + y;
    }

    //we can also define classes or structures 
    friend class CC; 

};

#if 0
void g(const C& c, int y){
    std::cout << c.x + y + 1; //CE, because x private 
}
#endif

void g2(const C& c, int y){
    std::cout << c.x + y + 1;
}

//IMPORTANT NOTE - FRIENDNESS IS BAD 
//friend it's like reinterpret cast - we have it but it's better to not use it 

//==========================================================
//weird examples 
class B {
private:
    class Inner {
    public:
        int x = 1; 
    private: 
        int y = 2; 
    }; 

public: 
    Inner f(){
        return Inner();
    }
};

void test1(){
    B b; 
    std::cout << b.f().x; //CE or 1? I think 1
    //because we call public function and then get access to public field of Inner instance 
    //so private above class only mean that we can't call class name outside this class 
    //so to use for example instance of inner we need to use auto: auto inner = c.f(); 
}
//==========================================================
class G {
private: 
    void f(int){
        std::cout << 1; 
    }
public: 
    void f(float) {
        std::cout << 2; 
    }
}; 

void test2(){
    G g; 
    g.f(0); //CE or 1? CE, but why? Selection of overloading function occured before checking for private modifiers, so firstly selected f(int) and only then we got CE because of access error 
    g.f(3.14); //CE or 2? CE because two times we have similary bad conversion double -> float nad double -> int so CE because of more the one instance of the functions 
    g.f(3.14f); //perfect matching -> it's ok 
}


int main(){
    C c; 
    // c.x CE - privacy checked during runtime  

    //it;s weird but we ca nget access to private field simply using c-style or reinterpret cast 
    int b = reinterpret_cast<int&>(c); 
    std::cout << b; //5
}