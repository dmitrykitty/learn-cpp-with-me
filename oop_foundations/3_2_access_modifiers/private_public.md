# C++ `public`, `private`, `friend`, nested private types

## 1. `class` vs `struct`

```cpp
class C {
    int x = 5;
};

struct S {
    int x = 5;
};
```

Main difference:

```text
class  -> private by default
struct -> public by default
```

So this:

```cpp
class C {
    int x = 5;
};
```

is equivalent to:

```cpp
class C {
private:
    int x = 5;
};
```

And this:

```cpp
struct S {
    int x = 5;
};
```

is equivalent to:

```cpp
struct S {
public:
    int x = 5;
};
```

In C++, `struct` and `class` are almost the same language mechanism. The difference is mostly convention and default visibility.

---

## 2. Access control is compile-time, not runtime

```cpp
class C {
private:
    int x = 5;
};

int main() {
    C c;
    // std::cout << c.x; // compile-time error
}
```

Important correction:

```text
Privacy is checked at compile time, not runtime.
```

There is no runtime protection here. `private` does not encrypt memory and does not add runtime checks.

It only tells the compiler:

```text
Code outside this class cannot name this private member directly.
```

The object still contains `x` in memory.

---

## 3. `public` and `private` sections

```cpp
class A {
public:
    int x;

private:
    int y;
};
```

Outside code can access:

```cpp
A a;
a.x = 10; // OK
```

But cannot access:

```cpp
a.y = 20; // compile-time error
```

Inside member functions of `A`, both are accessible:

```cpp
class A {
public:
    void set(int value) {
        y = value; // OK, we are inside A
    }

private:
    int y;
};
```

Main design idea:

```text
public  -> interface
private -> implementation details
```

This is encapsulation. Users of the class should not depend on private internals.

---

## 4. Members of the same class can access private members of other objects

```cpp
class C {
private:
    int x = 0;

public:
    bool same(const C& other) const {
        return x == other.x; // OK
    }
};
```

This is legal.

Privacy is per class, not per object.

So any member function of `C` can access private fields of any `C` object.

---

## 5. `friend` functions

```cpp
class C {
private:
    int x{3};

public:
    friend void g2(const C& c, int y);
};
```

A friend function:

```text
is not a member function,
but gets access to private/protected members.
```

So:

```cpp
void g2(const C& c, int y) {
    std::cout << c.x + y + 1; // OK, g2 is friend
}
```

Without `friend`, this would be illegal:

```cpp
void g(const C& c, int y) {
    std::cout << c.x + y; // error: x is private
}
```

---

## 6. Friend function defined inside class

```cpp
class C {
private:
    int x{3};

public:
    friend void g3(const C& c, int y) {
        std::cout << c.x + y;
    }
};
```

This is still **not a member function**.

So you do not call it like this:

```cpp
C c;
c.g3(c, 5); // error
```

You call it like a normal function:

```cpp
C c;
g3(c, 5); // OK, found by ADL
```

Because one argument has type `C`, argument-dependent lookup can find this friend function.

Important:

```text
friend defined inside class is an external function with special access.
It does not have `this`.
```

---

## 7. Friend class

```cpp
class C {
private:
    int x{3};

    friend class CC;
};
```

Now all member functions of `CC` can access private members of `C`.

Example:

```cpp
class CC {
public:
    void print(const C& c) {
        std::cout << c.x; // OK
    }
};
```

But friend relationship is limited:

```text
Friendship is not symmetric.
Friendship is not transitive.
Friendship is not inherited.
```

Meaning:

```text
If A is friend of B, B is not automatically friend of A.
If A is friend of B and B is friend of C, A is not automatically friend of C.
```

---

## 8. Is `friend` bad?

Your note says:

```text
FRIENDNESS IS BAD
```

Better version:

```text
friend is powerful and should be used carefully.
```

It breaks normal encapsulation, so overusing it usually means bad design.

But it has valid uses:

```text
operator overloading
testing internals
serialization
factory functions
tight cooperation between two classes
```

Example where `friend` is natural:

```cpp
class Vector2 {
private:
    double x;
    double y;

public:
    Vector2(double x, double y) : x(x), y(y) {}

    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};
```

So:

```text
friend is not evil,
but it should be rare and intentional.
```

---

## 9. Private nested class

```cpp
class B {
private:
    class Inner {
    public:
        int x = 1;

    private:
        int y = 2;
    };

public:
    Inner f() {
        return Inner();
    }
};
```

Question:

```cpp
void test1() {
    B b;
    std::cout << b.f().x;
}
```

This is OK and prints:

```text
1
```

Why?

Because you are not naming the private type directly. You call a public function `f()`, receive its result, and access public member `x` of that result.

But this is not allowed:

```cpp
B::Inner inner; // error: Inner is private
```

However, this can work:

```cpp
auto inner = b.f(); // OK
std::cout << inner.x;
```

Important idea:

```text
private nested type means the type name is private.
But if you obtain an object through public API, you may use its public members.
```

Still, returning private nested types from public APIs is weird design. Usually better to expose a public type if users are supposed to work with it.

---

## 10. Overload resolution happens before access checking

```cpp
class G {
private:
    void f(int) {
        std::cout << 1;
    }

public:
    void f(float) {
        std::cout << 2;
    }
};
```

Now:

```cpp
G g;
g.f(0);
```

This is a compile-time error.

Why?

Overload resolution first selects the best overload.

For `0`, type is `int`.

So the best match is:

```cpp
void f(int)
```

because it is exact match.

Only after overload resolution, compiler checks access.

Then it sees:

```text
selected function is private
```

So error.

Important order:

```text
1. Name lookup
2. Overload resolution
3. Access checking
```

Not:

```text
1. Remove private functions
2. Then overload
```

That is why private overloads can still affect overload resolution.

---

## 11. More overload examples

```cpp
g.f(3.14);
```

`3.14` has type `double`.

Candidates:

```cpp
f(int)    // double -> int
f(float)  // double -> float
```

Both are standard conversions of similar rank, so the call is ambiguous.

Result:

```text
compile-time error: ambiguous call
```

This one is OK:

```cpp
g.f(3.14f);
```

because `3.14f` has type `float`.

So public:

```cpp
void f(float)
```

is exact match.

---

## 12. Bypassing private with reinterpret cast

```cpp
class C {
private:
    int x = 5;
};

int main() {
    C c;
    int b = reinterpret_cast<int&>(c);
    std::cout << b;
}
```

This may print:

```text
5
```

But this is not a good idea.

Important points:

```text
private does not protect memory at runtime
reinterpret_cast can bypass the nice type interface
but this is fragile and bad design
```

For this exact simple class, many compilers lay out `x` as the first field, so reading first bytes as `int` gives `5`.

But this depends on object layout assumptions.

If the class changes:

```cpp
class C {
private:
    char c;
    int x = 5;
};
```

then the first bytes are no longer `x`.

Also if class gets virtual functions, base classes, different layout, padding, etc., this technique breaks.

The correct lesson is:

```text
C++ access control is not a security boundary.
It is a compile-time abstraction mechanism.
```

Do not use casts to access private fields in real code.

---

## 13. Important issue in your code: duplicate class name

Your code has:

```cpp
class C {
    int x = 5;
};
```

and later again:

```cpp
class C {
private:
    int x{3};
public:
    friend void g2(const C& c, int y);
};
```

This is a redefinition error if both are in the same namespace and same file.

You need different names, or comment one of them out.

Example:

```cpp
class C1 {
    int x = 5;
};

class C2 {
private:
    int x{3};

public:
    friend void g2(const C2& c, int y);
};
```

---

## 14. Compact corrected example

```cpp
#include <iostream>

class FriendExample {
private:
    int x{3};

public:
    friend void g2(const FriendExample& c, int y);

    friend void g3(const FriendExample& c, int y) {
        std::cout << c.x + y;
    }
};

void g2(const FriendExample& c, int y) {
    std::cout << c.x + y + 1;
}

class B {
private:
    class Inner {
    public:
        int x = 1;

    private:
        int y = 2;
    };

public:
    Inner f() {
        return Inner{};
    }
};

class G {
private:
    void f(int) {
        std::cout << 1;
    }

public:
    void f(float) {
        std::cout << 2;
    }
};

void test1() {
    B b;

    std::cout << b.f().x; // OK

    auto inner = b.f();   // OK
    std::cout << inner.x; // OK

    // B::Inner x; // error: Inner is private
}

void test2() {
    G g;

    // g.f(0);    // error: selected f(int), but it is private
    // g.f(3.14); // error: ambiguous
    g.f(3.14f);  // OK, calls public f(float)
}
```

---

## 15. Interview-style summary

### What is the difference between `public` and `private`?

`public` members are accessible from outside the class. `private` members are accessible only from the class itself and its friends.

### Is privacy checked at runtime?

No. Access control is checked at compile time.

### Can private members affect overload resolution?

Yes. Overload resolution happens before access checking.

### Is a friend function a member function?

No. A friend function is an external function that has special access to private/protected members.

### Is `friend` always bad?

No, but it should be used carefully because it weakens encapsulation.

### Can a private nested type be returned from a public function?

Yes. External code may use the returned object with `auto` or directly access its public members, but cannot name the private nested type directly.

### Can casts bypass private?

Sometimes yes in practice, because private is not runtime protection. But doing this is fragile, non-idiomatic, and should not be used in real code.
