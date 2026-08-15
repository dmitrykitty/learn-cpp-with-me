# C++ Pointers — Notes Based on Ilia’s Lecture Code

Pointers are one of the most important compound types in C++. A pointer stores an address of another object.

Simplified idea:

```text id="zv5jy7"
T        -> object of type T
T*       -> pointer to T
&T       -> address of object
*T*      -> dereference pointer and access object
```

Example:

```cpp id="sm0o85"
int x = 0;

int* px;     // px has type int*, pointer to int
px = &x;     // &x means "address of x"

int y = *px; // *px means "object pointed to by px"
```

Here:

```text id="o9dt4q"
x  -> int object
&x -> address of x
px -> pointer variable storing address of x
*px -> x itself
```

---

# 1. `*` has two different meanings

In a declaration:

```cpp id="gn0r5w"
int* px;
```

`*` is part of the type syntax. It means:

```text id="jpxydf"
px is a pointer to int
```

In an expression:

```cpp id="17iesf"
int y = *px;
```

`*` is the dereference operator. It means:

```text id="l2e6jf"
take the object located at the address stored in px
```

So:

```cpp id="cy5jph"
int* px;  // declaration: pointer type
*px;      // expression: dereference
```

---

# 2. Address-of operator `&`

The operator `&` takes the address of an object.

```cpp id="m1p9jb"
int x = 0;
int* px = &x;
```

Type transformation:

```text id="90z5ep"
x has type int
&x has type int*
```

General rule:

```text id="p82p8p"
& : T -> T*
```

Example:

```cpp id="iz7u9g"
#include <iostream>

int main() {
    int x = 42;
    int* px = &x;

    std::cout << px << std::endl;  // prints address of x
    std::cout << *px << std::endl; // prints 42
}
```

---

# 3. Pointer variable also has its own address

A pointer is also a variable. It is stored somewhere in memory and has its own address.

```cpp id="ufmeov"
int x = 0;
int* px = &x;
```

Here:

```text id="mlr6gb"
x   -> int object
px  -> pointer object
&x  -> address of x
&px -> address of px
```

Important:

```text id="4mqwlw"
px stores address of x.
&px is address of the pointer variable itself.
```

---

# 4. Multiple pointer declarations

This is a common C++ trap.

```cpp id="3togvk"
int a = 0, b = 1;
int* pa = &a, *pb = &b;
```

The `*` belongs to each declarator, not to the whole declaration line.

This is correct:

```cpp id="tsu6x6"
int* pa = &a, *pb = &b;
```

But this:

```cpp id="t2rhdq"
int* pa, pb;
```

means:

```text id="pf7u26"
pa -> int*
pb -> int
```

Not:

```text id="3v2d18"
pa -> int*
pb -> int*
```

For readability, many people prefer one declaration per line:

```cpp id="59f8pd"
int* pa = &a;
int* pb = &b;
```

---

# 5. Pointer size

On a typical 64-bit system:

```cpp id="q6ihvm"
sizeof(int*) // usually 8
```

This means:

```text id="vbnqv7"
A pointer variable occupies 8 bytes.
```

It does not mean that the pointed object has size 8.

Example:

```cpp id="0fcar3"
int a = 0;
int* pa = &a;

std::cout << sizeof(pa) << std::endl; // usually 8
std::cout << sizeof(*pa) << std::endl; // usually 4, because *pa is int
```

Important distinction:

```text id="u1kdp9"
sizeof(pa)  -> size of pointer
sizeof(*pa) -> size of pointed object
```

---

# 6. Pointer to pointer

Example:

```cpp id="o9y0d3"
#include <iostream>
#include <cstdio>

int main() {
    int a = 0;
    int* pa = &a;

    int** ppa = &pa;

    std::printf("ppa: %p, *ppa: %p, **ppa: %d\n",
                static_cast<void*>(ppa),
                static_cast<void*>(*ppa),
                **ppa);
}
```

Meanings:

```text id="81gx9v"
a     -> int object
pa    -> pointer to int, stores address of a
ppa   -> pointer to pointer to int, stores address of pa

ppa   = address of pa
*ppa  = value stored in pa = address of a
**ppa = value of a
```

If output is:

```text id="gp87vh"
&a / pa / *ppa = 0x7ffc8cc3fcf4
&pa / ppa      = 0x7ffc8cc3fcf8
```

Possible memory layout:

```text id="g5y31x"
0x...fcf4            int a begins here
0x...fcf5
0x...fcf6
0x...fcf7            int a ends here

0x...fcf8            int* pa begins here
0x...fcf9
0x...fcfa
0x...fcfb
0x...fcfc
0x...fcfd
0x...fcfe
0x...fcff            int* pa ends here
```

The difference between `ppa` and `*ppa` can be `4`, because:

```text id="lv5vmh"
ppa  = &pa
*ppa = pa = &a
```

So the printed difference is really:

```text id="nf5ah9"
&pa - &a
```

If `a` is a 4-byte `int` and `pa` was placed right after it, the difference is 4 bytes.

But:

```text id="mkdhj7"
The pointer pa still occupies 8 bytes.
The address stored inside pa points to a 4-byte int.
```

This stack layout is implementation-dependent and not guaranteed.

---

# 7. Dereferencing

Dereferencing means accessing the object pointed to by a pointer.

```cpp id="tzx787"
int x = 10;
int* px = &x;

std::cout << *px << std::endl; // 10
```

`*px` is an lvalue.

That means we can assign through it:

```cpp id="knxl9h"
int x = 10;
int* px = &x;

*px = 50;

std::cout << x << std::endl; // 50
```

So:

```text id="t7g8qi"
*px behaves like x
```

---

# 8. Address-of requires an lvalue

The address-of operator needs an object with identity.

This works:

```cpp id="91e952"
int a = 1;

int* p1 = &a;
```

This usually does not work:

```cpp id="t9sdpc"
int* p2 = &a++;
```

Why?

```cpp id="va4nnd"
a++
```

returns the old value as a temporary rvalue.

You cannot take the address of that temporary with built-in `&` in this context.

But this works:

```cpp id="dwgpht"
int* p3 = &(++a);
```

Because:

```cpp id="bn6w9l"
++a
```

returns `a` as an lvalue for built-in integers.

Still, in production code, avoid clever expressions like this. Prefer readability.

---

# 9. Pointer arithmetic

Pointer arithmetic is scaled by the size of the pointed type.

If:

```cpp id="7b3ao3"
int* p;
```

then:

```cpp id="hlqvmy"
p + 1
```

does not mean:

```text id="nff2df"
address + 1 byte
```

It means:

```text id="34pfbt"
address + sizeof(int)
```

General rule:

```text id="457at6"
T* + n -> address + n * sizeof(T)
```

Example with an array:

```cpp id="9hqfc8"
#include <iostream>

int main() {
    int arr[] = {10, 20, 30};

    int* p = &arr[0];

    std::cout << *p << std::endl;       // 10
    std::cout << *(p + 1) << std::endl; // 20
    std::cout << *(p + 2) << std::endl; // 30
}
```

If `int` is 4 bytes:

```text id="n9lvpx"
p + 1 moves by 4 bytes
p + 2 moves by 8 bytes
```

---

# 10. Important UB: pointer arithmetic outside arrays

Your code:

```cpp id="0bfx61"
int a = 0, b = 1;
int* pa = &a;
int* pb = &b;

std::cout << *(pa + 1) << std::endl;
```

This is dangerous.

Even if `b` happens to be stored right after `a`, `a` and `b` are separate objects, not elements of the same array.

For a single object, `pa + 1` can represent one-past-the-object, but dereferencing it is undefined behavior.

So:

```cpp id="h32sgq"
*(pa + 1) // UB
```

Correct example should use an array or vector storage:

```cpp id="iq9g8q"
int arr[] = {0, 1};
int* p = &arr[0];

std::cout << *(p + 1) << std::endl; // OK, prints 1
```

Important rule:

```text id="w6qi05"
Pointer arithmetic is valid inside the same array object,
including one-past-the-end pointer.

Dereferencing one-past-the-end is UB.
```

---

# 11. Allowed pointer operations

For pointers into the same array, you can do:

```text id="r0w6fw"
p + n
p - n
++p
--p
p1 - p2
p1 == p2
p1 != p2
p1 < p2
p1 <= p2
p1 > p2
p1 >= p2
```

Example:

```cpp id="6azgjq"
int arr[] = {10, 20, 30, 40};

int* first = &arr[0];
int* last = &arr[3];

std::cout << last - first << std::endl; // 3
```

Pointer subtraction:

```text id="rh7z7c"
last - first = (address(last) - address(first)) / sizeof(T)
```

Important:

```text id="p377m8"
Pointer subtraction and ordering comparisons are only meaningful
for pointers into the same array object.
```

This is invalid/undefined:

```cpp id="qehuwk"
int a = 1;
int b = 2;

int* pa = &a;
int* pb = &b;

std::cout << (pb - pa) << std::endl; // UB, different objects
```

Equality comparison is more general:

```cpp id="2nyatz"
pa == pb
```

can compare whether two pointers store the same address, but ordering/subtraction for unrelated objects is not valid.

---

# 12. Pointers and `std::vector`

Example:

```cpp id="v3m9sp"
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    int* pv = &v[0];

    std::cout << *pv << ' ' << *(pv + 1) << std::endl;
}
```

Output:

```text id="qeq7oa"
1 2
```

Because `std::vector` stores elements contiguously.

Better modern style:

```cpp id="da0pfy"
int* pv = v.data();
```

This is clearer and works well with vectors.

Important:

```text id="pf0w49"
std::vector elements are contiguous.
So pointer arithmetic works inside vector storage.
```

But be careful: vector reallocation can invalidate pointers.

Example:

```cpp id="qud0zf"
std::vector<int> v = {1, 2, 3};

int* p = v.data();

v.push_back(4); // may reallocate

std::cout << *p << std::endl; // possibly UB
```

After reallocation, old pointers to vector elements become dangling.

---

# 13. Postfix increment with pointers

Example from your code:

```cpp id="4m42cl"
std::vector<int> v = {1, 2, 3, 4, 5};

int* pv = &v[0];

std::cout << *pv << ' ' << *(pv + 1) << ' ' << *pv++ << std::endl;
```

Important part:

```cpp id="88e0rp"
*pv++
```

is parsed as:

```cpp id="58f09p"
*(pv++)
```

because postfix `++` has higher precedence than unary `*`.

Meaning:

```text id="a17m5h"
1. Use old pv.
2. Dereference old pv.
3. Then increment pv to point to next int.
```

So if `pv` points to `v[0]`, then:

```cpp id="8yzknu"
*pv++
```

returns `v[0]`, then moves `pv` to `v[1]`.

After this expression:

```text id="4r0v3k"
pv points to v[1]
```

Then:

```cpp id="9vcv5k"
int* pvlast = &v[4];

std::cout << pvlast - pv << std::endl;
```

prints:

```text id="3108d7"
3
```

because:

```text id="xiohce"
pvlast points to v[4]
pv points to v[1]
v[4] - v[1] = 3 elements
```

For readability, this is better:

```cpp id="w5om1n"
std::cout << *pv << ' ';
++pv;
```

---

# 14. `void*`

`void*` is a generic pointer type.

It can store the address of an object of any type:

```cpp id="ljmejm"
int x = 10;
void* p = &x;
```

But you cannot directly dereference `void*`.

This is invalid:

```cpp id="61tfpi"
std::cout << *p; // error
```

Why?

Because the compiler does not know the pointed type.

It does not know whether it should read:

```text id="93vqbe"
1 byte
4 bytes
8 bytes
some class object
```

So you must cast it back to the correct pointer type:

```cpp id="rdjdds"
int x = 10;
void* p = &x;

std::cout << *static_cast<int*>(p) << std::endl; // 10
```

---

# 15. Why does this print `0`?

Your code:

```cpp id="4mgf13"
int x = 0;
int* px = &x;
int y = *px;

int l = 2;
void* pl = &y;

std::cout << *(int*)pl << std::endl;
```

It prints `0` because:

```cpp id="yy1py2"
pl = &y;
```

not:

```cpp id="kglabz"
pl = &l;
```

Earlier:

```cpp id="7co1qq"
int y = *px;
```

and since `x == 0`, `y == 0`.

So:

```text id="5iqcxk"
pl points to y
y is 0
therefore *(int*)pl prints 0
```

Correct example with `l`:

```cpp id="rwkcnq"
int l = 2;
void* pl = &l;

std::cout << *static_cast<int*>(pl) << std::endl; // 2
```

Prefer C++ cast:

```cpp id="xzamrp"
static_cast<int*>(pl)
```

instead of C-style cast:

```cpp id="0wdo5w"
(int*)pl
```

---

# 16. `NULL` and `nullptr`

Old C/C++ code often uses:

```cpp id="sr9nil"
NULL
```

`NULL` is a macro. In C++, it is often defined as:

```cpp id="o9ebx2"
0
```

or sometimes:

```cpp id="m31s62"
0L
```

This can cause overload problems.

Example:

```cpp id="sclxm9"
void f(int);
void f(int*);

f(NULL); // may call f(int), not f(int*)
```

Modern C++ uses:

```cpp id="5z391p"
nullptr
```

`nullptr` is a keyword/literal of type:

```cpp id="4hfuqe"
std::nullptr_t
```

It represents a null pointer value and can convert to any pointer type.

Example:

```cpp id="ne8jy6"
int* p = nullptr;

if (p == nullptr) {
    std::cout << "p points to nothing\n";
}
```

Overload example:

```cpp id="ty5ftv"
#include <iostream>

void f(int) {
    std::cout << "int\n";
}

void f(int*) {
    std::cout << "int*\n";
}

int main() {
    f(nullptr); // calls f(int*)
}
```

Practical rule:

```text id="gp4ot1"
Use nullptr in modern C++.
Avoid NULL.
Avoid 0 as a null pointer constant in new code.
```

---

# 17. Dangling pointers

A dangling pointer is a pointer that stores an address of an object whose lifetime has ended.

Your code:

```cpp id="3dwplt"
#include <iostream>

int main() {
    int a = 1;
    int* pa = &a;

    {
        int b = 2;
        pa = &b;
    }

    std::cout << pa << ' ' << *pa << std::endl;
}
```

Inside the block:

```cpp id="1uw9bp"
int b = 2;
pa = &b;
```

`pa` points to `b`.

But after the block ends:

```cpp id="3g711b"
}
```

`b` is destroyed. Its lifetime is over.

So `pa` becomes a dangling pointer.

This line:

```cpp id="90u5uh"
std::cout << *pa << std::endl;
```

is undefined behavior.

It may print:

```text id="niquvf"
2
```

because the old stack memory still contains the previous bytes.

But it may also print garbage, crash, or behave differently after optimization.

Important:

```text id="md6zg7"
The pointer value still exists.
The object it points to does not.
Dereferencing it is UB.
```

---

# 18. Scope vs lifetime

This example is really about lifetime.

```cpp id="axg091"
{
    int b = 2;
    pa = &b;
}
```

`b` has block scope and automatic storage duration.

When the block ends:

```text id="6b8co6"
b is no longer alive
```

So any pointer to `b` becomes invalid.

After that, the compiler/stack may reuse the same memory for other variables:

```cpp id="yd2i85"
int c = 3, d = 4, e = 5, f = 6;

std::cout << &c << ' ' << &d << ' ' << &e << ' ' << &f;
```

Some of these variables may reuse the same stack location where `b` was stored before.

This is why dangling pointer bugs are dangerous: the memory may look valid, but it no longer belongs to the original object.

---

# 19. Pointer mental model

A pointer has two sides:

```text id="yzup3n"
1. The pointer object itself
2. The object it points to
```

Example:

```cpp id="9x9ww0"
int a = 10;
int* p = &a;
```

Conceptually:

```text id="pjvkcc"
p is stored somewhere in memory.
p contains an address.
That address is the location of a.
*p accesses a.
```

Do not confuse:

```cpp id="4jkwva"
p   // address stored inside pointer
&p  // address of pointer variable itself
*p  // object pointed to by pointer
```

---

# 20. Full cleaned version of your examples

```cpp id="3p3dgl"
#include <iostream>
#include <vector>
#include <cstdio>

int main() {
    int x = 0;

    int* px = &x;
    int y = *px;

    std::cout << "px = " << px << '\n';
    std::cout << "y = " << y << '\n';

    int a = 0;
    int b = 1;

    int* pa = &a;
    int* pb = &b;

    std::cout << "pa = " << pa << '\n';
    std::cout << "pb = " << pb << '\n';

    // Dangerous:
    // *(pa + 1) is UB because a and b are separate objects, not array elements.

    std::vector<int> v = {1, 2, 3, 4, 5};

    int* pv = v.data();

    std::cout << *pv << ' ' << *(pv + 1) << ' ' << *pv++ << '\n';

    int* pvlast = &v[4];

    std::cout << "distance = " << pvlast - pv << '\n'; // usually 3

    int l = 2;
    void* pl = &l;

    std::cout << *static_cast<int*>(pl) << '\n';

    int** ppa = &pa;

    std::printf("ppa: %p, *ppa: %p, **ppa: %d\n",
                static_cast<void*>(ppa),
                static_cast<void*>(*ppa),
                **ppa);

    std::cout << "sizeof(int*) = " << sizeof(int*) << '\n';
}
```

---

# 21. Interview-style summary

A pointer is an object that stores the address of another object. If `x` has type `T`, then `&x` has type `T*`. Dereferencing a pointer with `*p` gives access to the object pointed to by `p`.

The symbol `*` has different meanings depending on context: in a declaration, it is part of the pointer type; in an expression, it is the dereference operator. The symbol `&` is the address-of operator and requires an lvalue.

Pointer arithmetic is scaled by the size of the pointed type. If `p` is an `int*`, then `p + 1` moves by `sizeof(int)` bytes. Pointer arithmetic, subtraction, and ordering comparisons are only valid within the same array object. Dereferencing a one-past-the-end pointer or a pointer outside the array is undefined behavior.

`void*` is a generic pointer type that can store the address of any object, but it cannot be dereferenced without casting back to the correct pointer type.

`nullptr` is the modern C++ null pointer literal of type `std::nullptr_t`. It should be preferred over `NULL`, which is an old macro and may behave like integer `0`.

A dangling pointer stores the address of an object whose lifetime has ended. The pointer value may still look valid, but dereferencing it is undefined behavior. This commonly happens when a pointer points to a local variable after its scope has ended.
