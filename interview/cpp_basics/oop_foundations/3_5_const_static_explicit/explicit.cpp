struct Latitude {
    double value; 
    explicit Latitude(double val): value(val) {}
}; 

Latitude operator""_LTD(long double x) {
    return Latitude(x); 
}

struct Longitude {
    double value; 
    //usually code style required 1arg constreuctor to make explicit 
    explicit Longitude(double val): value(val) {}

    //operator implicit cast to double 
    //return type is double
    //we can make this operator explicit too 
    explicit operator double() const{
        return value; 
    }

    //we can also return double&
    explicit operator double&(){
        //...
    }
};

//we can also define own literal operators
Longitude operator""_LNG(long double x) {
    return Longitude(x); 
}

struct Point {
private: 
    Latitude lat; 
    Longitude lng; 

public: 
    Point(Latitude lat, Longitude lng): lat(lat), lng(lng) {}
    //it works because of operator double()
    
    double get_lng() const {
        return static_cast<double>(lng);
    }
};

int main() {
    //without explicit it possible
    //Point p(12, 15); 
    float a = 1.0f;

    //with explicit needed ti create object explicitly 
    Point pt(Latitude(12), Longitude(13)); 

    //because of overriding literals 
    Point pt2(15.4_LTD, 12.2_LNG);
}

//tell about contextual conversion and why my own type should be explicitly converted to boolean 