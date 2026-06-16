# C++ Structs, Classes, Layout, Methods, `this`, Nested Types

## 1. `class` vs `struct`

```cpp
class C;
struct S;
```

`class` and `struct` are almost the same in C++. The main difference is default visibility:

```cpp
struct A {
    int x; // public by default
};

class B {
    int x; // private by default
};
```

So `struct` is usually used for simple data-like types, and `class` for types with stronger encapsulation.

---

## 2. Default member initializers

```cpp
struct SS {
    int x;
    int y = 1;
};
```

`y` has a default member initializer, but `x` does not.

So:

```cpp
SS s;
std::cout << s.x; // UB: x is uninitialized
std::cout << s.y; // OK: y == 1
```

Important: default initialization of an automatic object does not zero-initialize primitive fields unless you explicitly ask for it.

Better:

```cpp
SS s{}; // x = 0, y = 1
```

---

## 3. Size of struct = fields + padding

```cpp
struct A {
    int x;
    double y = 1;
};
```

Naively:

```text
sizeof(int)    = 4
sizeof(double) = 8
sum            = 12
```

But usually:

```cpp
sizeof(A) == 16
```

because of **alignment**.

A `double` usually needs to start at an address divisible by `8`.

Memory layout:

```text
offset 0: int x      -> 4 bytes
offset 4: padding    -> 4 bytes
offset 8: double y   -> 8 bytes
total: 16 bytes
```

Padding exists so the CPU can access fields efficiently and correctly according to ABI alignment rules.

---

## 4. Empty struct has size 1

```cpp
struct Empty {};
```

Usually:

```cpp
sizeof(Empty) == 1
```

Why not `0`?

Because every object in C++ must have a unique address.

Example:

```cpp
Empty arr[10];
```

If `sizeof(Empty)` were `0`, then all elements would have the same address. That would break pointer arithmetic and object identity.

So C++ gives empty objects minimal size: `1`.

---

## 5. What can be inside a struct/class?

```cpp
struct B {
    int x;

    using str = std::string; // OK

    void print_incremented_x() {
        std::cout << x + 1;
    }
};
```

Inside a class/struct we can define:

```text
fields
methods
type aliases
nested types
static members
constructors/destructors
operators
```

This is invalid:

```cpp
using std::cout; // error inside class
```

because this kind of `using` declaration imports a namespace member, and class scope does not work like namespace/block scope here. But type aliases are OK:

```cpp
using str = std::string;
```

---

## 6. Defining methods outside the class

```cpp
struct C {
    std::string name = "name";
    void f();
};

void C::f() {
    std::cout << "Hello " << name;
}
```

Inside the class, we declare the method.

Outside, we define it using:

```cpp
C::f
```

`C::` means:

```text
function f belongs to class/struct C
```

This is common in real C++ projects:

```text
.hpp -> declarations
.cpp -> definitions
```

For example:

```cpp
// C.hpp
struct C {
    void f();
};

// C.cpp
void C::f() {
    // implementation
}
```

---

## 7. `this` pointer

```cpp
struct D {
    std::string name = "name";

    D& f() {
        return *this;
    }
};
```

Inside a non-static member function, C++ gives you a hidden pointer:

```cpp
this
```

For `D::f`, `this` points to the current object.

Conceptually:

```cpp
D& f(D* this) {
    return *this;
}
```

So:

```cpp
return *this;
```

returns a reference to the current object.

This is used for method chaining:

```cpp
d.f().f().f();
```

Returning `D&` means:

```text
do not copy object;
return another access path to the same object
```

---

## 8. Nested struct

```cpp
struct E {
    int x;
    double y;

    struct IE {
        char c;
    };
};
```

`IE` is a type declared inside `E`.

It does not create a field inside `E`.

So:

```cpp
sizeof(E)
```

depends only on:

```text
int x
double y
padding
```

Usually:

```cpp
sizeof(E) == 16
```

because `int + padding + double`.

To create object of nested type:

```cpp
E::IE ie{'c'};
```

Important correction:

```cpp
E::IE ie{'ch'}; // bad idea
```

`'ch'` is not a normal character literal. It is a multi-character literal with implementation-defined value. Use:

```cpp
E::IE ie{'c'};
```

You can also make alias:

```cpp
using IE = E::IE;
IE ie2{'x'};
```

---

## 9. Nested type vs nested object

This:

```cpp
struct E {
    struct IE {
        char c;
    };
};
```

declares only a type.

This:

```cpp
struct E {
    struct {
        char c;
    } obj;
};
```

declares a field named `obj` whose type is an unnamed struct.

Now `obj` affects `sizeof(E)` because an actual object exists inside `E`.

---

## 10. Aggregate initialization

```cpp
struct A {
    int x;
    double y = 1;
};

A a{1, 2.5}; // x = 1, y = 2.5
A b{1};      // x = 1, y = 1
```

This is aggregate initialization.

It works because `A` is an aggregate-like simple type: no user-provided constructors, public data fields, no complicated initialization logic.

Default member initializers are used for fields not explicitly initialized:

```cpp
A b{1}; // y uses default value 1
```

In modern C++, aggregate rules changed slightly between standards, but the practical idea is:

```text
simple public data type can be initialized field-by-field with braces
```

---

## 11. Local struct

```cpp
int main() {
    struct S {
        int x = 1;
        int y = 2;
    };

    S s;
}
```

This `S` exists only inside `main`.

It shadows any outer/global `S`.

Useful for very local helper types, but in production code local structs are less common unless the type is really only needed in one small scope.

---

## 12. Corrected compact code

```cpp
#include <iostream>
#include <string>

struct SS {
    int x;
    int y = 1;
};

struct A {
    int x;
    double y = 1;
};

struct B {
    int x;

    using str = std::string;

    void print_incremented_x() {
        std::cout << x + 1;
    }
};

struct C {
    std::string name = "name";
    void f();
};

void C::f() {
    std::cout << "Hello " << name;
}

struct D {
    std::string name = "name";

    D& f() {
        return *this;
    }
};

struct E {
    int x;
    double y;

    struct IE {
        char c;
    };
};

int main() {
    SS s{}; // x = 0, y = 1

    std::cout << sizeof(SS) << '\n';
    std::cout << sizeof(s) << '\n';

    std::cout << s.x << '\n';
    std::cout << s.y << '\n';

    A a{1, 2.5};
    A b{1};

    std::cout << sizeof(A) << '\n';
    std::cout << sizeof(E) << '\n';

    E::IE ie{'c'};

    using IE = E::IE;
    IE ie2{'x'};

    struct LocalS {
        int x = 1;
        int y = 2;
    };

    LocalS local;
}
```

---

## 13. Interview-style summary

### Why does struct have padding?

Because fields must satisfy alignment requirements. Padding ensures that fields like `double` start at properly aligned addresses.

### Why does empty struct have size 1?

Because distinct objects must have distinct addresses, and arrays need non-zero step size.

### What is `this`?

`this` is a hidden pointer to the current object inside non-static member functions.

### Does nested struct increase outer struct size?

No. A nested struct declaration creates a type, not an object. Only actual data members affect object size.

### What is aggregate initialization?

Brace initialization of simple public data types field by field.

### Why is reading `s.x` UB?

Because `s.x` is an uninitialized automatic `int`. Reading an indeterminate value is undefined behavior.
