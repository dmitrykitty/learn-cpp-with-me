# C++ Casts — Deep Notes

## 1. Why do casts exist?

A cast tells the compiler:

```text
Treat this expression as another type.
```

But different casts mean very different things.

Some casts are relatively safe and express normal conversions:

```cpp
int x = 5;
double y = static_cast<double>(x);
```

Some casts are dangerous and reinterpret memory:

```cpp
long long x = 1729;
double& y = reinterpret_cast<double&>(x); // dangerous, usually UB when used
```

Some casts remove `const`:

```cpp
const int x = 5;
int& y = const_cast<int&>(x); // modifying y would be UB
```

The main idea:

```text
A cast is not just syntax.
A cast is a statement about type system, memory, object lifetime, and allowed operations.
```

In modern C++, casts should be explicit and intentional.

---

# 2. Main C++ casts

C++ has four named casts:

```text
static_cast
dynamic_cast
reinterpret_cast
const_cast
```

Additionally, there is the old C-style cast:

```cpp
(T)expr
```

and function-style cast:

```cpp
T(expr)
```

In your lecture notes, we can think about five categories:

```text
1. static_cast
2. dynamic_cast
3. reinterpret_cast
4. const_cast
5. C-style cast
```

`dynamic_cast` is mainly connected with inheritance and polymorphism, so we can discuss it later.

---

# 3. `static_cast`

Basic example:

```cpp
int x = 0;
double y = static_cast<double>(x);
```

This means:

```text
Convert int value to double value.
```

The compiler performs a real value conversion.

So this is not “view the same bits as double”.

It creates a new `double` value representing the numeric value `0.0`.

---

## 3.1 Good mental model for `static_cast`

Your note says:

> If you need to do a cast and you do not know exactly what cast to use, use `static_cast`.

Better version:

```text
If the conversion is a normal type conversion and you want to make it explicit, use static_cast.
```

But if you do not understand why the cast is needed, first try to avoid the cast.

Good uses:

```cpp
double d = 3.14;
int x = static_cast<int>(d); // explicit narrowing conversion
```

```cpp
int x = 65;
char c = static_cast<char>(x);
```

```cpp
enum class Color {
    Red,
    Green,
    Blue
};

int value = static_cast<int>(Color::Green);
```

`static_cast` is for conversions that the compiler understands semantically.

---

# 4. `static_cast` and numeric conversions

Example:

```cpp
double d = 3.99;
int x = static_cast<int>(d);
```

Result:

```text
x == 3
```

The fractional part is discarded.

Important: `static_cast` does not protect you from all problems.

Example:

```cpp
double d = 1e100;
int x = static_cast<int>(d); // problematic, can be undefined/implementation-specific depending on case
```

A cast can make code compile, but it does not guarantee the value makes sense.

Another example:

```cpp
int x = 300;
unsigned char c = static_cast<unsigned char>(x);
```

If `unsigned char` is 8-bit, the result is usually:

```text
300 mod 256 = 44
```

For unsigned integer conversions, arithmetic modulo `2^N` is used.

For signed overflow/narrowing, things can be more subtle.

---

# 5. `static_cast` and pointers

## 5.1 Pointer to `void*`

You can convert object pointers to `void*`.

```cpp
int x = 5;

int* p = &x;
void* vp = static_cast<void*>(p);
```

`void*` means:

```text
pointer to some object, but the type is unknown
```

You cannot dereference `void*` directly:

```cpp
*vp; // error
```

because the compiler does not know:

```text
How many bytes should be read?
What type is stored there?
How should pointer arithmetic work?
```

You can cast it back:

```cpp
int* p2 = static_cast<int*>(vp);
std::cout << *p2 << '\n'; // 5
```

This is safe if the original object really was an `int`.

---

## 5.2 `void*` is common in C APIs

C often uses `void*` for generic data.

Example:

```c
void qsort(
    void* base,
    size_t count,
    size_t size,
    int (*compar)(const void*, const void*)
);
```

`qsort` does not know the type of elements.

It only knows:

```text
base pointer
number of elements
size of each element
comparison callback
```

Inside the comparator, you cast back:

```cpp
int cmp_ints(const void* a, const void* b) {
    const int* x = static_cast<const int*>(a);
    const int* y = static_cast<const int*>(b);

    return (*x > *y) - (*x < *y);
}
```

In C++, we usually prefer templates, `std::sort`, and typed functions instead.

---

# 6. Array-to-pointer conversion and casts

Example:

```cpp
int a[5] = {1, 2, 3, 4, 5};

int* p = a;
```

Here `a` decays to pointer to first element:

```text
a -> &a[0]
```

This is not really a cast you should write manually.

You can write:

```cpp
int* p = static_cast<int*>(a);
```

but it is unnecessary and less idiomatic.

Usually:

```cpp
int* p = a;
```

or:

```cpp
int* p = &a[0];
```

is clearer.

Important distinction:

```cpp
a      // often decays to int*
&a     // pointer to the whole array, type int (*)[5]
```

Example:

```cpp
int a[5];

int* p1 = a;       // pointer to first int
int (*p2)[5] = &a; // pointer to array of 5 ints
```

These are different pointer types.

---

# 7. `static_cast` does not remove `const`

Example:

```cpp
const int x = 5;

int& y = static_cast<int&>(x); // error
```

This is forbidden.

`static_cast` cannot remove constness.

To remove `const`, C++ has a special cast:

```cpp
const_cast
```

This is intentional. Removing `const` is dangerous, so the language forces you to write a very explicit cast.

---

# 8. `reinterpret_cast`

`reinterpret_cast` is much more dangerous.

Your note:

> `reinterpret_cast` is used when we want to read bits of one type as bits of another type. No recoding is processed.

This is close, but needs correction.

A better version:

```text
reinterpret_cast tells the compiler to reinterpret a pointer/reference/value as another unrelated type, usually without changing the bit pattern.
```

But:

```text
Being able to create such a reference or pointer does not mean it is legal to read/write through it.
```

That is the key point.

---

# 9. Example: interpreting `long long` as `double`

Your example:

```cpp
long long y = 1729;
double& x = reinterpret_cast<double&>(y); // UB when used
```

What happens?

`y` is a real object of type:

```cpp
long long
```

You create a reference of type:

```cpp
double&
```

to the same memory.

Conceptually:

```text
same bytes
viewed as long long through y
viewed as double through x
```

But in C++, an object has a real type.

The real object is still `long long`.

Reading it through a `double&` violates the strict aliasing/type access rules.

So this is undefined behavior:

```cpp
std::cout << x << '\n'; // UB
```

It is not just “will print weird double”.

It is formally illegal in C++.

The compiler is allowed to assume that a `double&` does not refer to a `long long` object.

That assumption is used for optimization.

---

# 10. Strict aliasing

Strict aliasing is a rule that allows the compiler to assume that pointers/references of unrelated types do not point to the same object.

Example:

```cpp
int x = 5;
double* p = reinterpret_cast<double*>(&x);

*p = 3.14; // UB
```

The compiler can assume:

```text
int* and double* do not alias the same object
```

because normally they cannot legally refer to the same object.

This helps optimization.

For example, if a function receives:

```cpp
void f(int* a, double* b) {
    *a = 10;
    *b = 3.14;
    std::cout << *a;
}
```

The compiler may assume writing through `b` cannot change `*a`.

If you break this rule with `reinterpret_cast`, the optimizer may produce surprising results.

---

# 11. Legal way to inspect object bytes

There is a special exception: you can inspect object representation using character-like types:

```cpp
char
unsigned char
std::byte
```

Example:

```cpp
#include <cstddef>
#include <iostream>

int x = 0x12345678;

unsigned char* bytes = reinterpret_cast<unsigned char*>(&x);

for (size_t i = 0; i < sizeof(x); ++i) {
    std::cout << static_cast<int>(bytes[i]) << ' ';
}
```

This is allowed.

Why?

Because C++ allows any object’s bytes to be inspected through `char`, `unsigned char`, or `std::byte`.

This is useful for:

```text
serialization
debugging
hashing raw object representation
binary protocols
low-level systems code
```

But even here you must remember:

```text
byte order depends on endianness
object may contain padding bytes
representation may differ between platforms
```

---

# 12. `reinterpret_cast` with structs of the same layout

Your example:

```cpp
struct A {
    int a; 
    double b; 
};

struct B {
    int c; 
    double d; 
};

int main() {
    A a; 
    a.a = 5; 
    a.b = 12.543; 

    B& b = reinterpret_cast<B&>(a); 

    std::print("A.a={} A.b={} B.c={} B.d={}", a.a, a.b, b.c, b.d); 
}
```

This may appear to work.

Memory of `A` may look like:

```text
[int a][padding][double b]
```

Memory of `B` may look like:

```text
[int c][padding][double d]
```

On many platforms, layout is the same.

So `b.c` may read the same bytes as `a.a`.

But in standard C++, this is not generally legal.

The real object is of type `A`, not `B`.

Accessing it through `B&` violates type aliasing/lifetime rules.

So:

```cpp
B& b = reinterpret_cast<B&>(a);
std::cout << b.c; // UB
```

can be undefined behavior.

Important:

```text
Same fields in same order does not automatically make reinterpret_cast safe.
```

---

# 13. Correct alternatives for same-layout structs

## Option 1: Explicit conversion

```cpp
B convert(const A& a) {
    return B{a.a, a.b};
}
```

This creates a copy, but it is safe and clear.

## Option 2: Use a common struct

If the data layout is really the same, model it as the same type:

```cpp
struct Common {
    int x;
    double y;
};
```

Then use different wrappers if needed.

## Option 3: `std::bit_cast`

C++20 introduced:

```cpp
std::bit_cast
```

It safely copies object representation from one type to another type of the same size.

Example:

```cpp
#include <bit>

B b = std::bit_cast<B>(a);
```

Requirements:

```text
sizeof(A) == sizeof(B)
A and B are trivially copyable
```

Important:

```text
std::bit_cast creates a new object.
It does not create an alias to the same memory.
```

So it avoids strict aliasing problems.

---

# 14. `reinterpret_cast` does not create objects

This is very important.

```cpp
A a;
B& b = reinterpret_cast<B&>(a);
```

This does **not** create a `B` object.

It only tells the compiler:

```text
Treat this memory address as if it referred to B.
```

But the lifetime of a `B` object has not started there.

The object whose lifetime exists is still `A`.

C++ object model cares about object lifetime.

This is why reinterpretation is dangerous.

---

# 15. Can we read two `int`s as one `long long`?

Your question:

> Can we read bytes of two ints, for example 4 and 5, as `long long` and get a big value as bit sum of these numbers?

Suppose:

```cpp
int a[2] = {4, 5};
long long& x = reinterpret_cast<long long&>(a);
```

This is not safe.

Problems:

```text
1. Strict aliasing: int[2] is not a long long object.
2. Alignment: the address of a[0] may not be properly aligned for long long.
3. Size: sizeof(int[2]) may equal sizeof(long long), but it is not guaranteed by the language.
4. Endianness: result depends on byte order.
5. Representation: int representation is platform-dependent.
```

So reading like this is undefined behavior:

```cpp
std::cout << x; // UB
```

---

## 15.1 Safe way with `std::memcpy`

If you really want to copy raw bytes into a `long long`, use `memcpy`.

```cpp
#include <cstring>
#include <iostream>

int a[2] = {4, 5};

static_assert(sizeof(a) == sizeof(long long));

long long x;
std::memcpy(&x, a, sizeof(x));

std::cout << x << '\n';
```

This is allowed because `memcpy` works with bytes.

But the resulting number depends on:

```text
endianness
sizeof(int)
integer representation
padding, if any
```

On a typical little-endian machine with 32-bit int and 64-bit long long:

```text
a[0] = 4 -> low 32 bits
a[1] = 5 -> high 32 bits
```

So `x` could be:

```text
5 * 2^32 + 4
```

But this is platform-specific.

---

## 15.2 Safe way with `std::bit_cast`

In C++20:

```cpp
#include <array>
#include <bit>
#include <cstdint>
#include <iostream>

std::array<int, 2> a = {4, 5};

static_assert(sizeof(a) == sizeof(std::uint64_t));

std::uint64_t x = std::bit_cast<std::uint64_t>(a);

std::cout << x << '\n';
```

Again: this is safe as a bit copy, but the numeric value is platform-dependent.

Important distinction:

```text
reinterpret_cast reference -> alias same memory, often UB when read
memcpy / bit_cast -> copy bytes into a new object, safe if conditions are met
```

---

# 16. When is `reinterpret_cast` actually used?

`reinterpret_cast` is used in low-level code, but carefully.

Examples:

```text
memory-mapped hardware registers
serialization/deserialization boundaries
operating systems
network packet parsing
custom allocators
interfacing with C APIs
debugging object representation
converting between pointer and integer types in special cases
```

But good C++ code tries to isolate it.

A common design rule:

```text
reinterpret_cast should appear in very small, low-level, well-tested parts of the codebase.
```

If `reinterpret_cast` appears everywhere, the design is probably wrong.

---

# 17. Pointer-to-pointer and unrelated pointer casts

`reinterpret_cast` can convert one object pointer type to another:

```cpp
int x = 5;

int* p = &x;
double* q = reinterpret_cast<double*>(p);
```

The conversion itself may compile.

But dereferencing `q` is not legal:

```cpp
std::cout << *q; // UB
```

The cast only changes the pointer type.

It does not change the object.

The object is still an `int`.

---

# 18. Can `reinterpret_cast` cast an integer to pointer?

You wrote:

> We can cast pointer to any other pointer OR type to any other type, but we can’t cast type to pointer.

Correction: `reinterpret_cast` can convert between pointer and integer types in both directions, with restrictions.

Example:

```cpp
#include <cstdint>

int x = 5;

int* p = &x;

std::uintptr_t raw = reinterpret_cast<std::uintptr_t>(p);
int* p2 = reinterpret_cast<int*>(raw);
```

`std::uintptr_t` is an unsigned integer type capable of storing a pointer value, if available.

But this is low-level and platform-dependent.

What you cannot do safely is invent arbitrary integers and dereference them as pointers:

```cpp
int* p = reinterpret_cast<int*>(12345);
*p = 10; // almost certainly crash or UB
```

This is only meaningful in special systems contexts, such as memory-mapped hardware addresses.

---

# 19. Function pointers and `reinterpret_cast`

You can sometimes cast between function pointer types:

```cpp
using F1 = void (*)(int);
using F2 = void (*)(double);

F1 f1 = ...;
F2 f2 = reinterpret_cast<F2>(f1);
```

But calling through the wrong function pointer type is undefined behavior.

Function pointer casts are especially dangerous because calling convention, parameter passing, return type, ABI, and registers must match.

---

# 20. `const_cast`

`const_cast` is used to add or remove `const`/`volatile`.

Most commonly, it removes `const`.

Example:

```cpp
const int c = 5;

int& r = const_cast<int&>(c);
```

This compiles.

But modifying through `r` is undefined behavior:

```cpp
r = 10; // UB, because original object was const
```

The key rule:

```text
const_cast can remove const from the access path.
It does not make a truly const object mutable.
```

---

# 21. `const_cast` is okay if the original object was not const

Example:

```cpp
void f(const int& x) {
    int& y = const_cast<int&>(x);
    ++y;
}

int main() {
    int x = 5;
    f(x);

    // x == 6
}
```

Here the original object is:

```cpp
int x = 5;
```

not:

```cpp
const int x = 5;
```

So removing const inside `f` and modifying it is not undefined behavior.

But this is still bad style.

Why?

The function signature says:

```cpp
void f(const int& x)
```

This communicates:

```text
I will not modify x.
```

But the implementation secretly modifies it.

This violates API expectations.

Better:

```cpp
void f(int& x) {
    ++x;
}
```

Use `const_cast` only when interacting with old APIs or very special low-level code.

---

# 22. Correct syntax for const reference parameter

Your note had:

```cpp
void f(const& int x)
```

Correct syntax is:

```cpp
void f(const int& x)
```

or:

```cpp
void f(int const& x)
```

Both mean the same:

```text
reference to const int
```

---

# 23. `const_cast` with pointers

Example:

```cpp
int x = 5;

const int* pc = &x;

int* p = const_cast<int*>(pc);

*p = 10; // OK, because original x was not const
```

But:

```cpp
const int x = 5;

const int* pc = &x;

int* p = const_cast<int*>(pc);

*p = 10; // UB, original object is const
```

Again:

```text
Removing const from the pointer type is not enough.
The original object must actually be non-const.
```

---

# 24. `const_cast` does not magically make a const pointer reseatable

Important distinction:

```cpp
int x = 1;
int y = 2;

int* const cp = &x;
```

`cp` is a const pointer to int.

You cannot do:

```cpp
cp = &y; // error
```

Can `const_cast` help?

Not really in the way you want.

The constness here is top-level const on the pointer object itself.

You cannot assign to a const variable.

You can create a non-const copy:

```cpp
int* p = cp;
p = &y; // OK, changes p, not cp
```

But that does not change `cp`.

`const_cast` is mostly useful for removing const from the pointed/referred object type:

```cpp
const int* pc = &x;
int* p = const_cast<int*>(pc);
```

Not for making a const variable itself assignable.

---

# 25. Dangerous example: modifying truly const object

```cpp
const int c = 5;

int& cc = const_cast<int&>(c);

cc = 10; // UB
```

Why UB?

Because `c` is a real const object.

The compiler may place it in read-only memory, or optimize assuming it never changes.

Example:

```cpp
const int c = 5;

int& r = const_cast<int&>(c);
r = 10;

std::cout << c << '\n';
std::cout << r << '\n';
```

This can print strange results, crash, or be optimized unpredictably.

The compiler is allowed to assume:

```text
c is const, so c is always 5.
```

---

# 26. Common legitimate use of `const_cast`

Sometimes old C APIs are not const-correct.

Example:

```c
void old_api(char* s);
```

But you know it does not actually modify the string.

You have:

```cpp
void wrapper(const char* s) {
    old_api(const_cast<char*>(s));
}
```

This is acceptable only if `old_api` truly does not modify `s`.

If it modifies a string literal:

```cpp
wrapper("abc");
```

then UB occurs.

So even legitimate `const_cast` is risky.

---

# 27. C-style casts

C-style cast:

```cpp
double y = (double)x;
```

or:

```cpp
B& b = (B&)a;
```

C-style casts are dangerous in C++ because they can mean many different things.

They may perform something like:

```text
const_cast
static_cast
static_cast + const_cast
reinterpret_cast
reinterpret_cast + const_cast
```

depending on context.

So this:

```cpp
(T)expr
```

does not clearly say what kind of conversion you intended.

That is why modern C++ prefers named casts.

---

# 28. Why C-style casts are dangerous

Example:

```cpp
const int c = 5;

int* p = (int*)&c;
```

This C-style cast removes const.

It is effectively a `const_cast`-like operation.

Another example:

```cpp
long long x = 1729;

double& d = (double&)x;
```

This is a reinterpretation.

So `(T)` can silently perform very dangerous conversions.

Named casts make intent visible:

```cpp
static_cast<T>(expr)       // normal conversion
reinterpret_cast<T>(expr)  // low-level reinterpretation
const_cast<T>(expr)        // remove/add const
dynamic_cast<T>(expr)      // runtime checked polymorphic cast
```

When reviewing code, this is much easier to reason about.

---

# 29. Functional-style casts

This syntax:

```cpp
int x = int(3.14);
```

is called function-style cast.

For simple numeric conversions it is common.

But for complex types, it can also behave like a C++ cast/construction.

Example:

```cpp
std::string s("abc");
```

This is construction, not dangerous.

But:

```cpp
int x = int(3.14);
```

is a cast-like conversion.

In serious C++ code, for narrowing or suspicious conversions, `static_cast` is clearer:

```cpp
int x = static_cast<int>(3.14);
```

---

# 30. `dynamic_cast` short preview

We will discuss it later, but basic idea:

```cpp
Base* b = ...;
Derived* d = dynamic_cast<Derived*>(b);
```

`dynamic_cast` checks at runtime whether the object really is a `Derived`.

It works with polymorphic classes, meaning classes with at least one virtual function.

If cast fails for pointer:

```cpp
d == nullptr
```

If cast fails for reference:

```cpp
std::bad_cast
```

This is different from `static_cast`, which does not perform runtime checking.

---

# 31. Comparison table

| Cast               | Main purpose                     |                                Dangerous? | Example                        |
| ------------------ | -------------------------------- | ----------------------------------------: | ------------------------------ |
| `static_cast`      | Normal explicit conversion       |                          Usually moderate | `static_cast<double>(x)`       |
| `dynamic_cast`     | Runtime-checked polymorphic cast |                    Safer but runtime cost | `dynamic_cast<Derived*>(base)` |
| `reinterpret_cast` | Low-level reinterpretation       |                            Very dangerous | `reinterpret_cast<char*>(&x)`  |
| `const_cast`       | Add/remove const/volatile        | Dangerous if modifying truly const object | `const_cast<int&>(x)`          |
| C-style cast       | Anything possible                |                                 Dangerous | `(T)x`                         |

---

# 32. Practical rules

## Rule 1: Prefer no cast

If code works without a cast, do not add one.

Casts often hide design problems.

## Rule 2: Use `static_cast` for normal conversions

```cpp
double d = static_cast<double>(x);
int i = static_cast<int>(d);
```

## Rule 3: Use `const_cast` only for const-correctness boundary problems

Usually when dealing with legacy APIs.

Never use it to lie about mutability.

## Rule 4: Use `reinterpret_cast` only in low-level code

Examples:

```text
byte inspection
serialization
memory-mapped IO
custom allocators
system interfaces
```

But avoid using it for ordinary application logic.

## Rule 5: Avoid C-style casts in C++

C-style casts hide intent.

Prefer named casts.

---

# 33. Corrected versions of your examples

## Static cast

```cpp
int x = 0;

double y = static_cast<double>(x);
```

This creates a new `double` value.

---

## Reinterpret cast example — dangerous

```cpp
long long y = 1729;

double& x = reinterpret_cast<double&>(y);

// Reading x is undefined behavior.
```

Better for bit copying:

```cpp
#include <bit>
#include <cstdint>

std::uint64_t bits = 1729;
double d = std::bit_cast<double>(bits);
```

This creates a `double` object from copied bits.

Still, the resulting `double` value depends on bit representation.

---

## Struct reinterpretation — not safe

```cpp
struct A {
    int a;
    double b;
};

struct B {
    int c;
    double d;
};

A a{5, 12.543};

B& b = reinterpret_cast<B&>(a); // dangerous
```

Better:

```cpp
B b{a.a, a.b};
```

or, if you specifically need bit-level copy and types allow it:

```cpp
#include <bit>

static_assert(sizeof(A) == sizeof(B));

B b = std::bit_cast<B>(a);
```

---

## Two ints as long long — safe byte copy

```cpp
#include <cstring>
#include <iostream>

int a[2] = {4, 5};

static_assert(sizeof(a) == sizeof(long long));

long long x;
std::memcpy(&x, a, sizeof(x));

std::cout << x << '\n';
```

Do not use:

```cpp
long long& x = reinterpret_cast<long long&>(a); // UB when read
```

---

## Const cast — original object non-const

```cpp
void f(const int& x) {
    int& y = const_cast<int&>(x);
    ++y;
}

int main() {
    int x = 5;
    f(x); // technically OK, but bad API design
}
```

Better:

```cpp
void f(int& x) {
    ++x;
}
```

---

## Const cast — original object const

```cpp
const int c = 5;

int& cc = const_cast<int&>(c);

cc = 10; // UB
```

---

# 34. Interview-style summary

## What is `static_cast`?

`static_cast` performs explicit conversions known to the compiler, such as numeric conversions, enum conversions, and some pointer conversions in related type hierarchies.

It does not remove `const`.

## What is `reinterpret_cast`?

`reinterpret_cast` performs low-level reinterpretation of a pointer/reference/integer representation.

It does not create a new object of the target type.

Using the result to access an incompatible object often causes undefined behavior.

## Why is `reinterpret_cast<double&>(long_long)` dangerous?

Because the actual object is still `long long`.

Reading it as `double` violates type aliasing and object lifetime rules.

## How should we reinterpret bits safely?

Use:

```cpp
std::bit_cast
```

in C++20, or:

```cpp
std::memcpy
```

for older standards.

## What is `const_cast`?

`const_cast` adds or removes const/volatile qualification.

Removing const and modifying is only safe if the original object was not actually const.

## Why are C-style casts discouraged?

Because they can perform many different kinds of casts, including `const_cast` and `reinterpret_cast`, without making the programmer’s intent clear.

Named casts are more explicit and safer for code review.

---

# 35. Practical engineering lesson

Casts are not just syntax. They interact with:

```text
object lifetime
type system
strict aliasing
const-correctness
memory layout
ABI
optimization
undefined behavior
```

In C++ systems/performance code, understanding casts is critical because wrong casts can compile and still make the program invalid.

The most important distinction:

```text
static_cast converts values in a type-aware way.

reinterpret_cast changes how a memory address/reference is viewed,
but does not change the real object.

const_cast changes access permissions,
but does not change whether the original object was truly const.
```

A good C++ programmer uses casts rarely, explicitly, and with a precise reason.
