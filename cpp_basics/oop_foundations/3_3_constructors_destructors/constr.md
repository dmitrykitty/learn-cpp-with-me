# Constructors and Destructors — notes

## 1. Basic constructor example

```cpp
class Complex {
    double re = 0.0;
    double im = 0.0;

public:
    Complex(double re) : re(re) {}
    Complex(double re, double im) : re(re), im(im) {}

    Complex();
};

Complex::Complex() {
    std::cout << "Default constructor\n";
}
```

### Member initializer list

Bad style:

```cpp
Complex(double real) {
    this->re = real;
}
```

Field `re` is first initialized with default value `0.0`, then assigned inside constructor body.

For primitive types it is not critical, but for objects it matters:

```cpp
struct X {
    std::string s;

    X(const std::string& value) {
        s = value; // default construction + assignment
    }
};
```

Better:

```cpp
struct X {
    std::string s;

    X(const std::string& value) : s(value) {} // direct construction
};
```

Constructor body runs **after** fields are already initialized.

Important rule:

```text
Members are initialized in declaration order, not in initializer-list order.
```

So always write initializer list in the same order as fields.

---

## 2. Default member initializers

```cpp
double re = 0.0;
double im = 0.0;
```

These are used only if the constructor does not explicitly initialize the field.

```cpp
Complex(double re) : re(re) {}
```

Here:

```text
this->re is initialized from constructor argument
this->im uses default value 0.0
```

The default value for `re` is ignored in this constructor.

---

## 3. Constructors defined outside class body

Constructor is a special member function, so it can be defined outside the class:

```cpp
class Complex {
public:
    Complex();
};

Complex::Complex() {
    std::cout << "Default constructor\n";
}
```

Same idea as normal methods:

```cpp
void C::f() {}
```

---

## 4. Initialization syntax

```cpp
Complex cmx(5.0);     // direct initialization
Complex cmx2 = 6.0;   // copy-initialization
Complex cmx3{7.0};    // direct-list initialization
Complex cmx4 = {8.0}; // copy-list initialization
```

Correction: this one is **copy-initialization**, not value initialization:

```cpp
Complex cmx2 = 6.0;
```

It works because `Complex(double)` can be used as implicit conversion from `double` to `Complex`.

Often single-argument constructors should be marked `explicit`:

```cpp
explicit Complex(double re) : re(re) {}
```

Then:

```cpp
Complex cmx(5.0);  // OK
Complex cmx2 = 6.0; // error
Complex cmx3{7.0}; // OK
```

---

# 5. Practical `String` example

```cpp
class String {
    char* arr = nullptr;
    size_t sz = 0;
    size_t cap = 0;

    String(size_t n) : arr(new char[n + 1]), sz(n), cap(n + 1) {
        arr[sz] = '\0';
    }

public:
    String() = default;

    String(size_t n, char c) : String(n) {
        memset(arr, c, sz);
        std::cout << "String(sz, ch)\n";
    }

    String(std::initializer_list<char> list) : String(list.size()) {
        std::copy(list.begin(), list.end(), arr);
        std::cout << "String(init_list)\n";
    }

    String(const String& other) : String(other.sz) {
        memcpy(arr, other.arr, sz + 1);
    }

    ~String() {
        delete[] arr;
    }
};
```

---

## 6. Delegating constructor

```cpp
String(size_t n, char c) : String(n) {
    memset(arr, c, sz);
}
```

`String(size_t n, char c)` delegates allocation/init logic to:

```cpp
String(size_t n)
```

Important rule:

```text
If constructor delegates to another constructor,
the initializer list can contain only that delegating constructor call.
```

So this is invalid:

```cpp
String(size_t n, char c) : String(n), sz(n) {} // error
```

The private constructor:

```cpp
String(size_t n)
```

is an internal helper. External user cannot call it, but other constructors can.

---

## 7. `memset`, `std::fill`, `memcpy`, `memmove`

For `char*`, this is okay:

```cpp
memset(arr, c, sz);
```

because we write raw bytes.

Equivalent high-level version:

```cpp
std::fill(arr, arr + sz, c);
```

For `char`, both are fine. For real C++ objects, prefer `std::fill`, because it respects assignment semantics.

---

### `memcpy`

```cpp
memcpy(arr, other.arr, sz + 1);
```

`memcpy` copies raw bytes.

It is okay for trivial byte-like data, such as `char`.

It is not okay for objects like `std::string`, because it bypasses constructors, destructors, invariants, and ownership rules.

Bad:

```cpp
std::string a[10];
std::string b[10];

memcpy(a, b, sizeof(a)); // UB
```

Use:

```cpp
std::copy(b, b + 10, a);
```

---

### `memcpy` vs `memmove`

`memcpy(dst, src, n)` assumes ranges do **not** overlap.

```cpp
memcpy(arr + 1, arr, 5); // UB if ranges overlap
```

`memmove(dst, src, n)` handles overlap correctly.

```cpp
memmove(arr + 1, arr, 5); // OK
```

Conceptually, `memmove` chooses safe direction:

```text
dst < src  -> copy forward
dst > src  -> copy backward
```

Rule:

```text
No overlap       -> memcpy
Possible overlap -> memmove
C++ objects      -> std::copy / std::move / algorithms
```

---

## 8. `std::initializer_list`

```cpp
String(std::initializer_list<char> list) : String(list.size()) {
    std::copy(list.begin(), list.end(), arr);
}
```

When we write:

```cpp
String v1{'a', 'b', 'c'};
```

compiler creates a temporary backing array of `const char`:

```text
'a', 'b', 'c'
```

and `std::initializer_list<char>` is basically a lightweight view:

```cpp
template<class T>
class initializer_list {
    const T* begin_;
    size_t size_;
};
```

So it is usually:

```text
pointer + size
```

Important consequences:

```text
Elements are const.
initializer_list does not own data.
Do not store list.begin() inside your object.
Copy values from it during constructor.
```

Your constructor does it correctly:

```cpp
std::copy(list.begin(), list.end(), arr);
```

---

## 9. Initializer-list priority

Given:

```cpp
String(size_t n, char c);
String(std::initializer_list<char> list);
```

These calls behave like this:

```cpp
String v1{'a', 'b', 'c'}; // String(init_list)
String v2{2, 'a'};        // String(init_list), because 2 can become char
String v3{'a', 2};        // String(init_list)
String v4(2, 'a');        // String(size_t, char)
```

Main rule:

```text
If class has initializer_list constructor and {} is used,
C++ first tries initializer_list constructors.
```

This is why `{}` may call a different constructor than expected.

Classic example:

```cpp
std::vector<int> a(5, 10); // five elements: 10 10 10 10 10
std::vector<int> b{5, 10}; // two elements: 5 10
```

---

## 10. Copy constructor

Default copy constructor copies fields member by member.

For this class:

```cpp
class String {
    char* arr;
    size_t sz;
    size_t cap;
};
```

default copy would do:

```text
new.arr = old.arr
new.sz  = old.sz
new.cap = old.cap
```

So two `String` objects would point to the same dynamic memory.

Then both destructors do:

```cpp
delete[] arr;
```

Result:

```text
double free / undefined behavior
```

So we need deep copy:

```cpp
String(const String& other) : String(other.sz) {
    memcpy(arr, other.arr, sz + 1);
}
```

In your old version:

```cpp
memcpy(arr, other.arr, sz);
```

it copies only characters, not `'\0'`.

Because `String(other.sz)` already writes:

```cpp
arr[sz] = '\0';
```

it still works, but `sz + 1` is clearer for null-terminated buffer.

---

## 11. Copy-on-write note

Copy-on-write means:

```text
Several objects share the same buffer.
Actual copy happens only when one object wants to modify data.
```

It needs:

```text
reference counter
shared buffer representation
detach-on-write logic
careful thread-safety decisions
```

Modern `std::string` generally does not use COW because it interacts badly with multithreading and iterator/reference invalidation rules.

For your own learning `String`, deep copy is the right approach.


## 12. Destructor

```cpp
~String() {
    delete[] arr;
}
```

Destructor releases resource owned by object.

```cpp
String v;
```

is safe because:

```cpp
arr = nullptr;
```

and:

```cpp
delete[] nullptr;
```

is valid and does nothing.

---

## 14. Manual destructor call

```cpp
v4.~String(); // bad here
```

For normal stack objects this is UB in practice, because destructor will be called again automatically at scope exit.

Result:

```text
first destructor call  -> delete[] arr
second destructor call -> delete[] same arr again
double free
```

Manual destructor calls are only for special low-level cases with placement new.

---

## 15. Reference and const members

```cpp
struct C {
    int& r;
    const int c;
};
```

This type cannot be default-constructed automatically.

Reason:

```text
reference must be bound immediately
const member must be initialized immediately
```

Correct:

```cpp
struct C {
    int& r;
    const int c;

    C(int& ref, int value) : r(ref), c(value) {}
};
```

They cannot be assigned later in constructor body.

---

## 16. Reference member initialized with global

```cpp
int glob = 0;

struct B {
    int& x = glob;

    B(int y) {
        x = y;
    }
};
```

Here `x` is initialized before constructor body.

So:

```text
x refers to glob
```

Then:

```cpp
x = y;
```

means:

```text
glob = y
```

This is valid.

Dangerous version:

```cpp
struct B {
    int& x;

    B(int y) : x(y) {}
};
```

This compiles because `y` is an lvalue inside constructor, but after constructor ends, `y` dies.

Then `x` is dangling.

---

## 17. Dangling reference member

```cpp
struct G {
    const int& x;

    G(int y) : x(y) {}
};
```

This is UB later.

`x` binds to constructor parameter `y`.

`y` exists only during constructor call.

After constructor finishes:

```text
x refers to destroyed parameter
```

Lifetime extension does not save this case.

Safe version:

```cpp
struct G {
    int x;

    G(int y) : x(y) {}
};
```

Or store reference only if caller guarantees lifetime:

```cpp
struct G {
    const int& x;

    G(const int& y) : x(y) {}
};
```

But then this is still dangerous:

```cpp
G g(5); // temporary dies, reference dangles
```

Better for ownership/value semantics:

```cpp
struct G {
    int x;
};
```

---

## 18. `String s5 = s5`

```cpp
String s5 = s5;
```

This is invalid logic.

You try to initialize object from itself before it has been constructed.

The copy constructor receives reference to object whose valid lifetime/state is not established.

Treat as UB / never write this.

Compilers may warn with flags like:

```bash
-Winit-self
-Wuninitialized
```

---

## 19. Aggregate/list initialization note

If class has no user-provided constructors and public fields, then `{}` can mean aggregate initialization.

Example:

```cpp
struct P {
    int x;
    int y;
};

P p{1, 2}; // aggregate initialization
```

But for `String`, because constructors exist, `{}` selects constructors, especially `initializer_list` if viable.

---

## 20. Construction/destruction order

For class without inheritance:

```text
1. fields are constructed in declaration order
2. constructor body runs
3. destructor body runs
4. fields are destroyed in reverse declaration order
```

So inside destructor body, fields are still alive.

After destructor body finishes, fields are destroyed automatically.

---

## 21. Corrected compact `String`

```cpp
class String {
    char* arr = nullptr;
    size_t sz = 0;
    size_t cap = 0;

    String(size_t n) : arr(new char[n + 1]), sz(n), cap(n + 1) {
        arr[sz] = '\0';
    }

public:
    String() = default;

    String(size_t n, char c) : String(n) {
        std::fill(arr, arr + sz, c);
        std::cout << "String(sz, ch)\n";
    }

    String(std::initializer_list<char> list) : String(list.size()) {
        std::copy(list.begin(), list.end(), arr);
        std::cout << "String(init_list)\n";
    }

    String(const String& other) : String(other.sz) {
        std::memcpy(arr, other.arr, sz + 1);
    }

    String& operator=(const String& other) {
        if (this == &other) {
            return *this;
        }

        char* new_arr = new char[other.sz + 1];
        std::memcpy(new_arr, other.arr, other.sz + 1);

        delete[] arr;

        arr = new_arr;
        sz = other.sz;
        cap = other.sz + 1;

        return *this;
    }

    ~String() {
        delete[] arr;
    }
};
```

---

## 22. Short summary

Constructor initializes object.

Member initializer list initializes fields before constructor body.

Delegating constructor reuses another constructor of same class.

`initializer_list` is compiler-supported and usually implemented as pointer + size to const backing array.

With `{}`, initializer-list constructors have priority.

Default copy constructor is shallow memberwise copy.

For owning raw pointer, shallow copy causes double free.

Destructor releases owned resources.

If you define destructor for owning raw memory, remember Rule of Three.

Reference and const members must be initialized in initializer list.

Do not bind reference members to constructor parameters unless their lifetime is guaranteed.

Do not manually call destructor on normal stack objects.
