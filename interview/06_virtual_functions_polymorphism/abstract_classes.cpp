#include <iostream>
#include <vector>

//abstract if has at least one pure virtual method
struct Shape {
    //pure virtual method
    virtual double area() const = 0; 
    virtual double perimeter() const = 0;
    virtual ~Shape() = default;
};

double Shape::area() const { 
    std::cout << "parameter fallback";
    return 0.0;
}

double Shape::perimeter() const { 
    std::cout << "parameter fallback";
    return 0.0;
}

struct Square: Shape {
    double a; 
    Square(double a): a(a) {}

    double area() const override {
        return Shape::area();
    }

    double perimeter() const override {
        return a * 4; 
    }

    ~Square() {
        std::cout << "square";
    }
};

struct Circle: Shape {
    double radious;
    Circle(double rad): radious(rad) {}
    double area() const override {
        return 3.1415926535 * radious;
    }

    double perimeter() const {
        return Shape::perimeter();
    }

    ~Circle() {
        std::cout << "circle";
    }
};


int main() {
    Circle* c = new Circle(5); 
    Square* s = new Square(6);

    std::vector<Shape*> v;
    v.reserve(2);
    v.push_back(c);
    v.push_back(s);
    for(auto* sh: v) {
        std::cout << sh->area() << " " << sh->perimeter();
        delete sh;
        std::cout << "\n---------------------------\n";
    }
}