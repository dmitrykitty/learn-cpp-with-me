# C++ Function Pointers — Deep Notes

## 1. What is a function in a compiled program?

In C/C++, a function is not a normal runtime object like `int`, `std::string`, or an array.

When the compiler compiles this:

```cpp
bool cmp(int x, int y) {
    return x * x < y * y;
}
````

it generates machine instructions for this function. These instructions are usually placed into the program's **text segment**:

```text
.text -> executable machine code
.data -> initialized global/static variables
.bss  -> zero-initialized global/static variables
.rodata -> read-only constants, string literals
stack -> local variables / call frames
heap  -> dynamic allocations
```

So conceptually:

```text
cmp:
    machine instruction
    machine instruction
    machine instruction
    ret
```

A pointer to function stores something that allows the program to call that function.

Simplified mental model:

```text
function pointer = address of executable code
```

But in real systems, it can be more complicated because of dynamic linking, PLT/GOT stubs, thunks, virtual dispatch helpers, different instruction/data address spaces, etc.

For everyday C++ on x86-64 Linux, thinking about it as “address of code in `.text`” is usually good enough.

---

## 2. Example from the lecture

```cpp
#include <iostream>
#include <algorithm>

bool cmp(int x, int y){
    return x * x < y * y;
}

void f(int){}
void f(double){}

int main(){
    int a[] = {1, -5, 2, 9, -3}; 

    std::sort(a, a + 5, cmp);

    bool (*p)(int, int) = cmp; 
    std::cout << (void*)p << std::endl;

    for(int i = 0; i < 5; i++){
        std::cout << a[i] << " "; 
    }

    void (*p1)(int) = f;
}
```

This lecture part is about several important mechanisms:

```text
1. Function-to-pointer conversion
2. Function pointer syntax
3. Passing functions as callbacks
4. Overload resolution
5. Why printing function pointers is tricky
6. Difference between compile-time declaration and link-time definition
```

---

# 3. Function pointer syntax

This declaration:

```cpp
bool (*p)(int, int);
```

means:

```text
p is a pointer to a function
that takes two int arguments
and returns bool
```

The parentheses are necessary.

Compare:

```cpp
bool (*p)(int, int);
```

versus:

```cpp
bool *p(int, int);
```

The second one means:

```text
p is a function that takes two ints and returns bool*
```

So for function pointers, the syntax is:

```cpp
ReturnType (*pointerName)(ArgumentTypes...)
```

Examples:

```cpp
int (*p1)(int);
double (*p2)(double, double);
void (*p3)();
bool (*p4)(int, int);
```

---

# 4. Why `cmp` and `&cmp` are almost the same

You asked:

```cpp
std::sort(a, a + 5, cmp);
// why cmp and &cmp is the same?
```

For a normal function:

```cpp
bool cmp(int x, int y);
```

the expression:

```cpp
cmp
```

can automatically convert to:

```cpp
&cmp
```

This is called **function-to-pointer conversion**.

So these two are equivalent in this context:

```cpp
bool (*p1)(int, int) = cmp;
bool (*p2)(int, int) = &cmp;
```

Both store a pointer to the function `cmp`.

Similarly:

```cpp
std::sort(a, a + 5, cmp);
```

and:

```cpp
std::sort(a, a + 5, &cmp);
```

both work.

The important rule:

```text
When a function name is used in an expression where a function pointer is expected,
the function name is automatically converted to a pointer to that function.
```

This is similar in spirit to array-to-pointer decay:

```cpp
int arr[5];
int* p = arr; // arr decays to pointer to first element
```

But functions and arrays are still different concepts.

---

# 5. What is the type of `&cmp`?

For:

```cpp
bool cmp(int x, int y);
```

the type of `cmp` as a function is:

```cpp
bool(int, int)
```

The type of `&cmp` is:

```cpp
bool (*)(int, int)
```

Meaning:

```text
pointer to function taking (int, int) and returning bool
```

So this is correct:

```cpp
bool (*p)(int, int) = cmp;
```

or:

```cpp
bool (*p)(int, int) = &cmp;
```

Calling through the pointer:

```cpp
p(2, 3);
```

is equivalent to:

```cpp
(*p)(2, 3);
```

Both work.

---

# 6. Function pointers as callbacks

This line:

```cpp
std::sort(a, a + 5, cmp);
```

passes `cmp` as a **callback**.

`std::sort` does not know in advance how exactly you want to compare elements. You provide comparison logic.

Your comparator:

```cpp
bool cmp(int x, int y){
    return x * x < y * y;
}
```

means:

```text
Sort numbers by their absolute value squared.
```

Array:

```text
1, -5, 2, 9, -3
```

Squares:

```text
1, 25, 4, 81, 9
```

Sorted by square:

```text
1, 2, -3, -5, 9
```

The comparator answers the question:

```text
Should x go before y?
```

So:

```cpp
cmp(2, -3)
```

checks:

```text
2 * 2 < (-3) * (-3)
4 < 9
true
```

So `2` should be before `-3`.

---

# 7. Comparator requirements for `std::sort`

`std::sort` requires the comparator to define a **strict weak ordering**.

For comparator `comp(x, y)`:

```text
comp(x, y) == true
```

means:

```text
x should be ordered before y
```

Important rules:

```text
1. comp(x, x) must be false
2. If comp(a, b) is true, then comp(b, a) must be false
3. The relation must be transitive
```

Your comparator:

```cpp
bool cmp(int x, int y){
    return x * x < y * y;
}
```

mostly works for sorting by absolute value.

But there is a low-level danger:

```cpp
x * x
```

can overflow for large `int`.

Example:

```cpp
int x = 50000;
x * x; // may overflow int
```

Signed integer overflow in C++ is **undefined behavior**.

Safer version:

```cpp
bool cmp(int x, int y) {
    return 1LL * x * x < 1LL * y * y;
}
```

Now multiplication is done in `long long`.

Even safer for edge cases like `INT_MIN`, one should be careful with `abs`, because:

```cpp
std::abs(INT_MIN)
```

can also overflow for `int`.

---

# 8. Why `std::cout << p` prints `1` or `0`

You wrote:

```cpp
bool (*p)(int, int) = cmp; 
std::cout << (void*)p << std::endl; // without casting we will get 0 or 1. why?
```

If you write:

```cpp
std::cout << p << std::endl;
```

you might expect it to print an address.

But often it prints:

```text
1
```

Why?

Because `std::ostream` has overloads for many pointer types, especially:

```cpp
operator<<(const void*)
```

But a function pointer is not necessarily convertible to `const void*` in standard C++.

Object pointers and function pointers are different categories.

Example object pointer:

```cpp
int x = 10;
int* p = &x;

std::cout << p; // prints address
```

Here `int*` can convert to `const void*`.

But function pointer:

```cpp
bool (*p)(int, int) = cmp;
```

is not an object pointer. It points to code, not to an object.

So overload resolution may not find a good pointer-printing overload.

However, function pointers can be converted to `bool`:

```cpp
if (p) {
    // pointer is not null
}
```

So `std::cout` uses the `bool` overload and prints:

```text
1
```

for non-null pointer, or:

```text
0
```

for null pointer.

That is why:

```cpp
std::cout << p;
```

prints `1`.

It means:

```text
p is not null
```

not:

```text
the address is 1
```

---

# 9. Can we cast function pointer to `void*`?

You wrote:

```cpp
std::cout << (void*)p << std::endl;
```

On many systems, especially normal Linux/x86-64, this will print something that looks like an address.

But there is an important C++ standard nuance.

In standard C++, converting a **function pointer** to `void*` is not fully portable in the same way as converting an object pointer to `void*`.

Why?

Because C++ allows systems where:

```text
object addresses and function addresses have different representations
```

For example, on some architectures:

```text
data memory and instruction memory are separate
```

So a function pointer is not necessarily representable as `void*`.

On POSIX systems, this is commonly supported because of APIs like `dlsym`, but pure ISO C++ does not want to assume this for every possible machine.

So this is okay as a debugging trick on common platforms:

```cpp
std::cout << reinterpret_cast<void*>(p) << std::endl;
```

or C-style:

```cpp
std::cout << (void*)p << std::endl;
```

But conceptually remember:

```text
object pointer -> void* is standard and normal
function pointer -> void* is implementation-dependent / conditionally supported
```

Better wording:

```text
This works on my platform, but it is not something I should rely on for portable C++ code.
```

---

# 10. Function pointer and null value

A function pointer can be null:

```cpp
bool (*p)(int, int) = nullptr;
```

Before calling it, you can check:

```cpp
if (p) {
    p(1, 2);
}
```

Calling a null function pointer is undefined behavior:

```cpp
bool (*p)(int, int) = nullptr;
p(1, 2); // UB
```

Low-level meaning:

```text
The CPU would try to jump to an invalid address.
```

In practice this often crashes with segmentation fault, but the standard says undefined behavior.

---

# 11. How calling through a function pointer works

Direct call:

```cpp
cmp(2, 3);
```

The compiler knows exactly which function is called.

Indirect call:

```cpp
bool (*p)(int, int) = cmp;
p(2, 3);
```

Here the program loads the target address from `p` and jumps/calls there.

At machine-code level, conceptually:

```text
direct call:
    call cmp

indirect call:
    call [address stored in p]
```

This matters for performance.

Direct calls are easier to inline and optimize.

Function pointer calls are indirect. The compiler may have a harder time inlining them because the target can change at runtime.

Example:

```cpp
bool (*p)(int, int);

if (condition) {
    p = cmp1;
} else {
    p = cmp2;
}

p(x, y);
```

At compile time, the compiler may not know which function will be called.

This is one reason why modern C++ often prefers lambdas or function objects for generic algorithms.

---

# 12. Function pointer vs lambda in `std::sort`

Your version:

```cpp
std::sort(a, a + 5, cmp);
```

works.

But this version is often better for optimization:

```cpp
std::sort(a, a + 5, [](int x, int y) {
    return 1LL * x * x < 1LL * y * y;
});
```

Why?

A non-capturing lambda creates a unique compile-time type. Since `std::sort` is a template, the comparator type is known exactly.

The compiler often can inline lambda calls better than calls through a function pointer.

So:

```text
function pointer comparator -> possible indirect call
lambda comparator -> often easier to inline
```

For small code, this does not matter. For performance-sensitive code, it can matter.

This is an important C++ performance idea:

```text
Generic programming with templates often allows zero-cost abstraction because concrete types are known at compile time.
```

---

# 13. Overloaded functions and function pointers

You have:

```cpp
void f(int){}
void f(double){}
```

So `f` is overloaded.

That means `f` is not one function. It is an **overload set**:

```text
f(int)
f(double)
```

Now this line:

```cpp
void (*p1)(int) = f;
```

works.

You asked:

```text
Here type of f is not defined and depends on what we have on the left side.
How does it work?
```

Exactly.

The name `f` refers to an overload set. The compiler uses the target type on the left side:

```cpp
void (*p1)(int)
```

to choose the correct overload.

It sees:

```text
I need a pointer to function taking int and returning void.
```

From the overload set:

```cpp
void f(int);
void f(double);
```

it selects:

```cpp
void f(int);
```

So this works:

```cpp
void (*p1)(int) = f;
```

And this also works:

```cpp
void (*p2)(double) = f;
```

because now the expected type is:

```cpp
void (*)(double)
```

---

# 14. When overload resolution is ambiguous

This may fail:

```cpp
auto p = f;
```

Why?

Because `auto` needs to deduce a type, but `f` is an overload set. There is no single type to deduce.

The compiler sees:

```text
f could mean f(int)
f could mean f(double)
```

So it cannot choose.

You need to provide the target type explicitly:

```cpp
void (*p1)(int) = f;
```

or use `static_cast`:

```cpp
auto p1 = static_cast<void (*)(int)>(&f);
auto p2 = static_cast<void (*)(double)>(&f);
```

This is useful when the context is not enough.

---

# 15. `f` vs `&f` with overloaded functions

For non-overloaded function:

```cpp
bool cmp(int, int);
```

these are usually equivalent:

```cpp
bool (*p1)(int, int) = cmp;
bool (*p2)(int, int) = &cmp;
```

For overloaded functions:

```cpp
void f(int);
void f(double);
```

both can work if the target type is known:

```cpp
void (*p1)(int) = f;
void (*p2)(int) = &f;
```

But without target type:

```cpp
auto p = &f; // error: cannot resolve overloaded function
```

because the compiler does not know whether you mean:

```cpp
&f(int)
```

or:

```cpp
&f(double)
```

Correct:

```cpp
auto p = static_cast<void (*)(int)>(&f);
```

---

# 16. Declaration vs definition and linker errors

You wrote:

```cpp
// if function is not defined - we cant take it address -> we will get linker exception
```

This is basically correct, but let’s make it precise.

A function declaration tells the compiler:

```text
This function exists somewhere.
Here is its name and type.
```

Example:

```cpp
void g(int); // declaration only
```

This is enough for compilation:

```cpp
void (*p)(int) = g;
```

The compiler knows the type of `g`.

But taking the address of `g` means the program actually needs the function symbol to exist at link time.

If there is no definition:

```cpp
void g(int) {
    // implementation
}
```

then the linker will fail with something like:

```text
undefined reference to `g(int)'
```

or on MSVC:

```text
unresolved external symbol
```

So:

```cpp
void g(int); // declaration

int main() {
    void (*p)(int) = g; // compiles
}
```

can compile but fail to link.

The compiler phase checks syntax and types.

The linker phase checks whether all referenced symbols actually exist.

---

# 17. Function pointer types must match

This is valid:

```cpp
bool cmp(int, int);

bool (*p)(int, int) = cmp;
```

This is invalid:

```cpp
bool (*p)(double, double) = cmp;
```

because `cmp` takes two `int`s, not two `double`s.

Function pointer types include:

```text
1. Return type
2. Parameter types
3. cv/ref qualifiers for member functions
4. noexcept in modern C++ function types
```

Example with `noexcept`:

```cpp
void h() noexcept;

void (*p1)() noexcept = h; // OK
void (*p2)() = h;          // usually OK: pointer to noexcept function can convert to pointer to non-noexcept function
```

But the details can become subtle.

For now, remember:

```text
Function pointer type must match the function signature.
```

---

# 18. Function pointers are not the same as pointers to member functions

Normal function pointer:

```cpp
void (*p)(int);
```

Pointer to member function:

```cpp
struct A {
    void f(int);
};

void (A::*pm)(int) = &A::f;
```

These are very different.

To call a member function pointer, you need an object:

```cpp
A obj;
(obj.*pm)(10);
```

A member function has an implicit `this` parameter, so its pointer representation can be more complex than a normal function pointer.

This matters later when learning C++ object model and virtual functions.

---

# 19. Full corrected example

```cpp
#include <iostream>
#include <algorithm>

bool cmp(int x, int y) {
    return 1LL * x * x < 1LL * y * y;
}

void f(int) {
    std::cout << "f(int)\n";
}

void f(double) {
    std::cout << "f(double)\n";
}

int main() {
    int a[] = {1, -5, 2, 9, -3};

    // Function name cmp decays to pointer to function.
    std::sort(a, a + 5, cmp);

    // p is a pointer to function taking two ints and returning bool.
    bool (*p)(int, int) = cmp;

    // Without cast, std::cout may print 1 because function pointer converts to bool.
    std::cout << "As bool: " << p << '\n';

    // Implementation-dependent debugging trick on common platforms.
    std::cout << "As address: " << reinterpret_cast<void*>(p) << '\n';

    for (int i = 0; i < 5; i++) {
        std::cout << a[i] << " ";
    }

    std::cout << '\n';

    // f is overloaded.
    // Left side tells compiler which overload we want.
    void (*p1)(int) = f;
    void (*p2)(double) = f;

    p1(10);     // calls f(int)
    p2(3.14);   // calls f(double)

    // Alternative explicit form:
    auto p3 = static_cast<void (*)(int)>(&f);
    p3(42);
}
```

---

# 20. Practical summary

## What is a function pointer?

A function pointer stores a callable address/reference to a function’s code.

Example:

```cpp
bool (*p)(int, int) = cmp;
```

## Why are `cmp` and `&cmp` equivalent?

Because function names automatically convert to function pointers in contexts where a pointer is needed.

```cpp
bool (*p1)(int, int) = cmp;
bool (*p2)(int, int) = &cmp;
```

## Why does `std::cout << p` print `1`?

Because function pointers are not normal object pointers. `std::ostream` may use conversion to `bool`, so it prints whether the pointer is non-null.

## Why does `void (*p1)(int) = f;` work with overloaded `f`?

Because the left side gives the expected type. The compiler selects the overload matching:

```cpp
void(int)
```

from the overload set.

## Why can missing function definition cause linker error?

A declaration is enough for compilation, but if you take the address or call the function, the linker must find its actual definition.

---

# 21. Engineering lesson

Function pointers are one of the foundations of low-level C and C++ callback mechanisms.

They are used in:

```text
C APIs
sorting/searching callbacks
signal handlers
plugin systems
dynamic loading
jump tables
interpreters
dispatch tables
embedded systems
operating systems
```

But in modern C++, for many cases we prefer:

```cpp
lambda
function object
template callable
std::function
```

because they are safer, more flexible, and often easier for the compiler to optimize.

Still, understanding raw function pointers is important because it teaches how code is represented, how indirect calls work, how callbacks are implemented, and how C-style APIs communicate behavior.

```

To CV/C++ systems path jest bardzo dobry temat, bo funkcje jako wskaźniki prowadzą potem naturalnie do dispatch tables, pluginów, ABI, dynamic linking, virtual calls i ogólnie do tego, jak program naprawdę wykonuje kod. :contentReference[oaicite:0]{index=0}
```
