# Const methods, mutable and static members

```cpp
#include <iostream>
#include <cstddef>
```

---

## 1. `const` object and `const` methods

```cpp
struct S {
    void f() {
        std::cout << "Hi";
    }
};

void tests() {
    const S s;
    // s.f(); // CTE
}
```

`f()` is non-const method. Non-const method expects non-const `this`.

Conceptually:

```cpp
void f(S* this);
```

But for:

```cpp
const S s;
```

the object gives only `const S*`.

So calling non-const method would require removing constness. Compiler blocks it.

Correct:

```cpp
struct S {
    void f() const {
        std::cout << "Hi";
    }
};
```

For a `const` member function, `this` is conceptually:

```cpp
const S* this;
```

So inside `const` method you cannot modify normal fields.

---

## 2. What happens with attributes of const object?

```cpp
struct S2 {
    char arr[10];
};
```

For:

```cpp
const S2 s2;
```

`arr` is treated as part of const object. You cannot do:

```cpp
// s2.arr[0] = 'a'; // CTE
```

But important distinction:

```cpp
struct X {
    char* arr;
};
```

For:

```cpp
const X x;
```

the pointer member behaves like:

```cpp
char* const arr;
```

Meaning:

```text
the pointer itself cannot be changed,
but the characters pointed to are not automatically const.
```

So if `arr` points to mutable memory, this can still modify that memory:

```cpp
const X x{some_buffer};
x.arr[0] = 'a'; // possible, because pointee is char, not const char
```

Constness of object applies to the member variable itself. For pointer members, the pointer becomes const, not necessarily the pointee.

---

## 3. `mutable`

```cpp
struct S2 {
    mutable int p = 0;

    void f() const {
        p++; // OK
    }
};
```

`mutable` means:

```text
this field may be modified even inside const methods
```

Use cases:

```text
cached values
lazy computation
statistics/debug counters
mutexes inside logically const methods
```

Example:

```cpp
struct Cache {
    int value = 0;
    mutable bool calculated = false;
    mutable int cached_result = 0;

    int get() const {
        if (!calculated) {
            cached_result = value * 2;
            calculated = true;
        }
        return cached_result;
    }
};
```

Idea:

```text
const means logical state should not change.
mutable is for internal technical state.
```

Do not use `mutable` just to bypass const correctness.

---

## 4. Const and non-const `operator[]`

```cpp
struct S2 {
    char arr[10];

    const char& operator[](size_t index) const {
        return arr[index];
    }

    char& operator[](size_t index) {
        return arr[index];
    }
};
```

For non-const object:

```cpp
S2 s;
s[0] = 'a'; // calls char& operator[]
```

For const object:

```cpp
const S2 s;
char c = s[0]; // calls const char& operator[] const
// s[0] = 'a'; // CTE
```

Why return `const char&`, not just `char`?

For `char`, returning by value would also be fine:

```cpp
char operator[](size_t index) const;
```

because `char` is tiny.

But containers usually return reference because for general `T` copying may be expensive:

```cpp
const T& operator[](size_t index) const;
T& operator[](size_t index);
```

Also reference means:

```text
we access the real element inside the object,
not a copy
```

For `char` specifically, both styles are acceptable. For `std::vector<T>`-like containers, `const T&` is the standard pattern.

---

## 5. Reference members inside const methods

```cpp
struct S2 {
    int x = 0;
    int& xr = x;

    void f2(int y) const {
        // x++;  // CTE
        xr++;   // compiles
    }
};
```

Why does `xr++` compile?

Because `xr` is a reference member. Constness of the object does not “rebind” or const-qualify the referenced object in the same way as normal fields.

A reference is already bound to some object. Accessing `xr` gives `int&`.

So compiler sees:

```cpp
xr++; // modifies referred int
```

But is it safe?

Depends on the real object.

### Case 1: object is really non-const

```cpp
S2 s;
const S2& cs = s;

cs.f2(1); // xr++ modifies s.x
```

This is okay, because the original object is non-const. We only access it through a const reference.

### Case 2: object is really const

```cpp
const S2 s;
s.f2(1); // xr++ tries to modify subobject of const object
```

This is undefined behavior.

So this trick compiles, but it is bad design. If you really want to modify a member in const method, use `mutable`:

```cpp
mutable int p = 0;
```

Do not hide mutation through reference members.

---

## 6. Static data members

```cpp
struct StaticExample {
    static double PI;
    inline static double pi = 5;

    // const static double PI_INIT = 3.1415926535; // not valid as written for double in classic C++
};
```

`static` data member belongs to the class, not to each object.

So:

```cpp
StaticExample a;
StaticExample b;
```

do not contain separate `PI`.

There is one shared variable:

```cpp
StaticExample::PI
```

Static members do not affect `sizeof(StaticExample)`.

---

## 7. Version 1: normal static member

```cpp
struct StaticExample {
    static double PI;
};

double StaticExample::PI = 3.1415;
```

Inside class we have only declaration.

Outside class we provide definition and initialization.

Where is it stored?

Usually:

```text
initialized non-zero static/global variable -> .data
zero-initialized static/global variable     -> .bss
```

So:

```cpp
double StaticExample::PI = 3.1415;
```

will usually be in `.data`.

If it was:

```cpp
double StaticExample::PI = 0.0;
```

it would usually be in `.bss`.

---

## 8. Version 2: `inline static`

```cpp
struct StaticExample {
    inline static double pi = 5;
};
```

Since C++17, `inline static` allows defining and initializing static member directly inside class.

Useful for header-only code.

No separate `.cpp` definition needed.

```cpp
std::cout << StaticExample::pi;
```

This is still one shared variable, not per-object.

---

## 9. Version 3: compile-time constant

Your code:

```cpp
const static double PI_INIT = 3.1415926535;
```

For `double`, better write:

```cpp
static constexpr double PI_INIT = 3.1415926535;
```

or since C++17:

```cpp
inline static const double PI_INIT = 3.1415926535;
```

Difference:

```text
static double PI
    modifiable shared variable, needs out-of-class definition

inline static double pi
    modifiable shared variable, definition inside class, C++17

static constexpr double PI_INIT
    compile-time constant, usable in constant expressions
```

If compiler needs actual storage for `PI_INIT`, it will usually go to read-only data section like `.rodata`.

But if used only as compile-time constant, it may not exist as a real memory object at all.

---

## 10. Static methods

```cpp
struct StaticExample {
    static void increment(int& x) {
        ++x;
    }
};
```

Static method has no `this`.

So it cannot access non-static fields directly.

It is called through class:

```cpp
int x = 1;
StaticExample::increment(x);
```

Static member functions are basically normal functions placed in class namespace.

They are useful when function is logically connected to class but does not need object state.

---

## 11. Corrected compact code

```cpp
#include <iostream>
#include <cstddef>

struct S {
    void f() {
        std::cout << "Hi";
    }
};

struct S2 {
    char arr[10]{};

    mutable int p = 0;

    void f() const {
        std::cout << "Hi";
    }

    const char& operator[](size_t index) const {
        return arr[index];
    }

    char& operator[](size_t index) {
        return arr[index];
    }

    int x = 0;
    int& xr = x;

    void f2(int) const {
        // x++;  // CTE
        xr++;   // compiles, but dangerous if object is truly const
    }
};

struct StaticExample {
    static double PI;
    inline static double pi = 5; // C++17

    static constexpr double PI_INIT = 3.1415926535;

    static void increment(int& x) {
        ++x;
    }
};

double StaticExample::PI = 3.1415;

void tests() {
    const S s;
    // s.f(); // CTE: f is not const

    const S2 s2;
    s2.f(); // OK

    // s2[0] = 'a'; // CTE
}

void test_static() {
    int x = 1;

    StaticExample::increment(x);

    std::cout << x << '\n';
    std::cout << StaticExample::PI << '\n';
    std::cout << StaticExample::pi << '\n';
    std::cout << StaticExample::PI_INIT << '\n';
}
```

---

## 12. Short summary

`const` method means `this` points to const object.

Non-const methods cannot be called on const objects.

For pointer members, const object makes the pointer const, not necessarily the pointee.

`mutable` allows modifying selected fields inside const methods.

Reference members can bypass compile-time const checks, but modifying a truly const object through them is UB.

Static data members belong to the class, not to object instances.

Normal static data members need out-of-class definition.

`inline static` allows in-class definition since C++17.

`static constexpr` is best for compile-time constants like mathematical constants.

Static methods have no `this`.
