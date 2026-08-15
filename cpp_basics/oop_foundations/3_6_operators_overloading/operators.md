# Operator Overloading in C++

## 1. General idea

Operator overloading allows user-defined types to behave like built-in types:

```cpp
Complex a(1, 2);
Complex b(3, 4);

Complex c = a + b;
++c;
std::cout << c;
```

But operators should keep their natural meaning.

Good:

```cpp
BigInteger a, b;
a + b;
a < b;
```

Bad:

```cpp
a + b; // sends network request
```

Operator overloading should make code more readable, not surprising.

---

## 2. Example class

```cpp
class Complex {
    double re = 0.0;
    double im = 0.0;

public:
    Complex(double re) : re(re) {}
    Complex(double re, double im) : re(re), im(im) {}

    double get_re() const {
        return re;
    }

    double get_im() const {
        return im;
    }
};
```

Important note: mathematically, complex numbers do not have a natural `<` ordering.
But for programming, we can define some artificial ordering, for example lexicographical order by `(re, im)`, if we need `std::set`, `std::map`, sorting, etc.

---

## 3. Assignment operator with ref-qualifier

```cpp
Complex& operator=(const Complex& other) & {
    re = other.re;
    im = other.im;
    return *this;
}
```

The `&` after the parameter list means:

```text
this operator can be called only on lvalues
```

So this is allowed:

```cpp
Complex a(1, 2);
Complex b(3, 4);

a = b; // OK
```

But this is blocked:

```cpp
(a + b) = Complex(5, 6); // CTE if operator= is lvalue-qualified
```

Without `&`, assignment to temporary can compile:

```cpp
c1 + c2 = c3;
```

Why? Because `c1 + c2` creates a temporary `Complex`, and non-const member functions can normally be called on temporaries.

Old workaround was returning `const Complex` from `operator+`, but that is bad modern style because it can break move semantics and makes value types less usable.

Better modern solution:

```cpp
Complex& operator=(const Complex& other) &;
```

---

## 4. `operator+=` as member

```cpp
Complex& operator+=(const Complex& other) {
    re += other.re;
    im += other.im;
    return *this;
}
```

`+=` modifies the current object, so it is natural as a member function.

It returns `Complex&` to allow chaining:

```cpp
a += b += c;
```

Pattern:

```text
compound assignment operators are usually members
+=, -=, *=, /=
```

---

## 5. `operator+`: usually non-member

Member version:

```cpp
Complex operator+(const Complex& other) const {
    return Complex(re + other.re, im + other.im);
}
```

But for binary symmetric operators, better style is usually non-member:

```cpp
Complex operator+(Complex lhs, const Complex& rhs) {
    lhs += rhs;
    return lhs;
}
```

Why this pattern?

```text
lhs is passed by value -> it is already a copy
lhs += rhs             -> reuse operator+= logic
return lhs             -> return result
```

This avoids duplicating addition logic.

---

## 6. Why non-member `operator+` is better

If `operator+` is a member:

```cpp
c1 + 3.14
```

means:

```cpp
c1.operator+(3.14)
```

This can work if `3.14` can be converted to `Complex`.

But:

```cpp
3.14 + c1
```

cannot call a member of `Complex`, because the left side is `double`.

A non-member operator:

```cpp
Complex operator+(Complex lhs, const Complex& rhs);
```

allows conversions on both operands.

So this can work:

```cpp
Complex c(1, 2);

Complex a = c + 3.14;
Complex b = 3.14 + c;
```

provided `Complex(double)` is not `explicit`.

If `Complex(double)` is `explicit`, then implicit conversion from `double` is blocked, and you need explicit overloads or explicit construction.

---

## 7. Do not define both member and non-member `operator+`

If you define both:

```cpp
Complex Complex::operator+(const Complex& other) const;
Complex operator+(const Complex& lhs, const Complex& rhs);
```

then for:

```cpp
c1 + c2
```

both can be viable candidates.

This can lead to ambiguity or confusing overload resolution.

Usually choose one approach.

Recommended:

```cpp
Complex& operator+=(const Complex& other); // member
Complex operator+(Complex lhs, const Complex& rhs); // non-member
```

---

## 8. RVO / NRVO and `return res += o2`

Good version:

```cpp
Complex operator+(Complex lhs, const Complex& rhs) {
    lhs += rhs;
    return lhs;
}
```

Alternative:

```cpp
Complex operator+(const Complex& o1, const Complex& o2) {
    Complex res = o1;
    res += o2;
    return res;
}
```

This can use NRVO: Named Return Value Optimization.

The compiler may construct `res` directly in the caller’s return slot.

But this version is worse:

```cpp
Complex operator+(const Complex& o1, const Complex& o2) {
    Complex res = o1;
    return res += o2;
}
```

Why?

`res += o2` returns `Complex&`.

So the return expression is not simply the local variable `res`.
It is an expression of type `Complex&`.

Because return type is `Complex`, compiler must create a `Complex` result from that reference.

Before move semantics, that means an extra copy.

So prefer:

```cpp
res += o2;
return res;
```

or even cleaner:

```cpp
Complex operator+(Complex lhs, const Complex& rhs) {
    lhs += rhs;
    return lhs;
}
```

Since C++17, returning a temporary like this has guaranteed copy elision:

```cpp
return Complex(a, b);
```

But returning a named local variable uses NRVO, which is allowed and usually performed, but not mandatory in every case.

---

## 9. Three-way comparison: `<=>`

```cpp
std::weak_ordering operator<=>(const Complex& o) const = default;
```

Since C++20, `<=>` is called the spaceship operator.

If defaulted, compiler compares fields in declaration order:

```text
first re
then im
```

lexicographically.

It can generate:

```text
<, >, <=, >=
```

and with defaulted comparison, also `==`/`!=` in many normal cases.

But if you define your own non-defaulted `<=>`, you should usually also define `operator==`.

Example custom implementation:

```cpp
std::weak_ordering operator<=>(const Complex& o) const {
    if (re < o.re) return std::weak_ordering::less;
    if (re > o.re) return std::weak_ordering::greater;

    if (im < o.im) return std::weak_ordering::less;
    if (im > o.im) return std::weak_ordering::greater;

    return std::weak_ordering::equivalent;
}

bool operator==(const Complex& o) const {
    return re == o.re && im == o.im;
}
```

---

## 10. `strong_ordering`, `weak_ordering`, `partial_ordering`

### `std::strong_ordering`

Use when equality means fully identical for ordering purposes.

Example:

```text
int
std::string
```

If neither `a < b` nor `b < a`, then `a == b`.

---

### `std::weak_ordering`

Use when different values may be equivalent for ordering.

Example: case-insensitive strings.

```text
"abc" and "ABC" are different strings
but equivalent in case-insensitive ordering
```

So:

```text
equivalent does not always mean equal
```

---

### `std::partial_ordering`

Use when some values are unordered.

Main example:

```text
double with NaN
```

For `NaN`:

```cpp
nan < 1.0  // false
nan > 1.0  // false
nan == 1.0 // false
```

So floating-point comparisons naturally use partial ordering.

Because `Complex` contains `double`, defaulted `<=>` may actually produce `std::partial_ordering`, not `std::weak_ordering`, depending on members.

So this may be better:

```cpp
auto operator<=>(const Complex& o) const = default;
```

Let compiler deduce correct comparison category.

---

## 11. Old-style comparison with `<`

Before C++20, often we defined only `<` and `==`.

```cpp
bool operator<(const Complex& c1, const Complex& c2) {
    return c1.get_re() < c2.get_re()
        || (c1.get_re() == c2.get_re() && c1.get_im() < c2.get_im());
}
```

This is lexicographical comparison by `(re, im)`.

For `std::set` and `std::map`, `<` must provide strict weak ordering.

That means:

```text
not random
not inconsistent
not dependent on changing state
```

---

## 12. `operator>`

```cpp
bool operator>(const Complex& c1, const Complex& c2) {
    return c2 < c1;
}
```

Good.

Reuse `<`.

---

## 13. `operator<=` correction

Your version:

```cpp
bool operator <=(const Complex& c1, const Complex& c2) {
    return c1 < c2 && !(c2 < c1);
}
```

This is wrong.

For `<=`, we want:

```text
c1 is less than c2 OR c1 is equivalent to c2
```

Correct:

```cpp
bool operator<=(const Complex& c1, const Complex& c2) {
    return !(c2 < c1);
}
```

Because if `c2 < c1` is false, then `c1 <= c2`.

Similarly:

```cpp
bool operator>=(const Complex& c1, const Complex& c2) {
    return !(c1 < c2);
}
```

For equality:

```cpp
bool operator==(const Complex& c1, const Complex& c2) {
    return !(c1 < c2) && !(c2 < c1);
}
```

But with `double`, exact equality can be tricky due to floating-point precision.

---

## 14. `operator<<`

```cpp
std::ostream& operator<<(std::ostream& out, const Complex& cmp) {
    return out << cmp.get_re() << ' ' << cmp.get_im();
}
```

This should be non-member because the left operand is `std::ostream`.

Expression:

```cpp
std::cout << c;
```

means:

```cpp
operator<<(std::cout, c);
```

It cannot be a member of `Complex`, because then syntax would need to be:

```cpp
c.operator<<(std::cout);
```

which is not how stream output works.

Return type is:

```cpp
std::ostream&
```

to allow chaining:

```cpp
std::cout << c1 << c2 << '\n';
```

If getters are natural public API, use getters.
If not, make `operator<<` a friend.

---

## 15. Prefix and postfix increment

Prefix:

```cpp
Complex& operator++() {
    ++re;
    ++im;
    return *this;
}
```

Usage:

```cpp
++c;
```

Returns reference to modified object.

Postfix:

```cpp
Complex operator++(int) {
    Complex copy = *this;
    ++re;
    ++im;
    return copy;
}
```

Usage:

```cpp
c++;
```

The `int` parameter is dummy. It exists only to distinguish postfix from prefix.

Postfix must return old value, so it needs a copy.

Therefore prefix is usually more efficient:

```cpp
++it; // preferred for iterators
it++; // may create copy
```

---

## 16. Functor: overloaded `operator()`

```cpp
struct Greater {
    bool operator()(int x, int y) const {
        return x > y;
    }
};
```

This makes object callable like a function:

```cpp
Greater cmp;
cmp(5, 3); // true
```

Used with algorithms:

```cpp
void functor_test() {
    std::vector<int> a = {1, -5, 7, 4};
    std::sort(a.begin(), a.end(), Greater());
}
```

`std::sort` uses comparator to decide order.

Comparator must implement strict weak ordering.

For descending order:

```cpp
x > y
```

is okay.

Modern alternative:

```cpp
std::sort(a.begin(), a.end(), [](int x, int y) {
    return x > y;
});
```

Functor is still useful when comparator has state or reusable type.

---

## 17. Correct compact version

```cpp
#include <iostream>
#include <compare>

class Complex {
    double re = 0.0;
    double im = 0.0;

public:
    Complex(double re) : re(re) {}
    Complex(double re, double im) : re(re), im(im) {}

    Complex& operator=(const Complex& other) & {
        re = other.re;
        im = other.im;
        return *this;
    }

    Complex& operator+=(const Complex& other) {
        re += other.re;
        im += other.im;
        return *this;
    }

    Complex& operator++() {
        ++re;
        ++im;
        return *this;
    }

    Complex operator++(int) {
        Complex copy = *this;
        ++(*this);
        return copy;
    }

    auto operator<=>(const Complex& other) const = default;

    double get_re() const {
        return re;
    }

    double get_im() const {
        return im;
    }
};

Complex operator+(Complex lhs, const Complex& rhs) {
    lhs += rhs;
    return lhs;
}

std::ostream& operator<<(std::ostream& out, const Complex& c) {
    return out << c.get_re() << ' ' << c.get_im();
}
```

Note: using `<=>` for `Complex` is technically possible, but mathematically artificial. For real complex numbers, equality makes sense, but ordering is not naturally defined.

---

## 18. Practical operator overloading rules

Use member operators for:

```text
operator=
operator[]
operator()
operator->
compound assignments: +=, -=, *=, /=
prefix/postfix ++ when modifying object
```

Use non-member operators for symmetric binary operations:

```text
operator+
operator-
operator*
operator/
operator==
operator<
operator<<
```

Common pattern:

```cpp
T& operator+=(const T& rhs);       // member
T operator+(T lhs, const T& rhs);  // non-member
```

Use `friend` only if the operator needs private access and exposing getters would reveal implementation details.

Do not overload operators with surprising meaning.

---

## 19. Short summary

`operator+=` should usually be a member because it modifies `this`.

`operator+` should usually be a non-member and implemented through `+=`.

Binary operators as members have asymmetry problems like `double + Complex`.

`operator<<` must be non-member because the left operand is `std::ostream`.

Prefix `++x` returns modified object by reference.

Postfix `x++` returns old value by value and usually needs a copy.

`operator<=>` can generate comparison operators, but comparison category matters.

For `double` fields, use `auto operator<=>(...) = default` because partial ordering may be needed.

`operator=` can be ref-qualified with `&` to forbid assignment to temporaries.
