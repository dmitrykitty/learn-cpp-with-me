You are right. The previous version was too “commentary-like”. Below is a deeper **Notion-ready rewrite** with low-level mechanisms: memory layout, array decay, stack/static/heap, `new[]` metadata, `delete[]`, VLA problems, 2D arrays, cache locality, and C-string internals.

---

# C++ Arrays and C-Strings — Deep Notes

## 1. What is a C++ array?

An array in C++ is a fixed-size sequence of objects of the same type stored **contiguously** in memory.

Example from your notes:

```cpp
int a[5]; // array of 5 uninitialized ints
```

This creates 5 `int` objects one after another:

```text
a[0] a[1] a[2] a[3] a[4]
```

If `sizeof(int) == 4`, then the whole array takes:

```text
5 * 4 = 20 bytes
```

Memory layout can be imagined as:

```text
address:  1000   1004   1008   1012   1016
object:   a[0]   a[1]   a[2]   a[3]   a[4]
```

So when you write:

```cpp
a[3]
```

the compiler calculates:

```text
address_of_a + 3 * sizeof(int)
```

This is why array indexing is very fast: it is just address arithmetic.

Important correction:

```cpp
int a[5];
```

does **not** create “random numbers”. It creates 5 `int`s with **indeterminate values**. Reading them before initialization is undefined behavior.

Bad:

```cpp
int a[5];
std::cout << a[0]; // undefined behavior
```

Good:

```cpp
int a[5] = {};
std::cout << a[0]; // 0
```

---

## 2. Array initialization

Your examples:

```cpp
int a[5];                  // uninitialized
int b[] = {1, 2, 3, 4, 5};  // size deduced as 5
int c[5] = {};             // 0 0 0 0 0
int d[5] = {1, 2};         // 1 2 0 0 0
```

### `int a[5];`

For local automatic variables, elements are not initialized.

```cpp
int a[5];
```

Memory contains whatever bits were already on the stack.

### `int b[] = {1, 2, 3, 4, 5};`

The compiler deduces the size:

```cpp
int b[5] = {1, 2, 3, 4, 5};
```

### `int c[5] = {};`

Value-initialization. For `int`, this means zero-initialization:

```text
0 0 0 0 0
```

### `int d[5] = {1, 2};`

Only the first two elements are explicitly initialized. The rest are zero-initialized:

```text
1 2 0 0 0
```

This rule is very useful in low-level code because it allows partial initialization safely.

---

# 3. Stack, static memory, heap

Your example:

```cpp
// int e[50'000'000]; // possible stack overflow
static int e[50'000'000]; // static storage duration
```

This is a very important systems topic.

## Local array

```cpp
int e[50'000'000];
```

This is usually allocated on the **stack**.

If `sizeof(int) == 4`, then:

```text
50,000,000 * 4 = 200,000,000 bytes ≈ 200 MB
```

Most program stacks are much smaller than this. On many systems, stack size may be something like 1 MB, 8 MB, or 16 MB. So this can crash with stack overflow or segmentation fault.

The stack is designed for function calls, return addresses, local variables, and temporary objects. It is fast, but limited.

## Static array

```cpp
static int e[50'000'000];
```

This is not placed on the stack. It has **static storage duration**, meaning it exists for the whole lifetime of the program.

Because it is zero-initialized, it usually goes into the `.bss` section.

Typical memory sections:

```text
.text    -> machine code
.rodata  -> read-only constants, string literals
.data    -> initialized global/static variables with non-zero values
.bss     -> zero-initialized global/static variables
heap     -> dynamic memory: new, malloc
stack    -> function calls and local automatic variables
```

Example:

```cpp
static int a[100];        // usually .bss
static int b[3] = {1, 2}; // usually .data
const char* s = "abc";    // pointer on stack/global, literal usually in .rodata
```

Deep point: `.bss` does not necessarily make the executable file huge. The binary can store only metadata saying: “reserve this much zero-initialized memory at program start.” The OS loader maps zero-filled pages when the program starts.

---

# 4. Arrays are not pointers

Your note:

```cpp
int arr[]{1, 2, 3, 4, 5}; // arr is a pointer to first element
```

Important correction:

```text
arr is not a pointer.
```

`arr` is an array object.

But in many expressions, an array automatically converts to a pointer to its first element. This is called **array-to-pointer decay**.

Example:

```cpp
int arr[]{1, 2, 3, 4, 5};

std::cout << *(arr + 2) << std::endl; // 3
```

Here `arr` decays to:

```cpp
&arr[0]
```

So:

```cpp
*(arr + 2)
```

means:

```cpp
arr[2]
```

The compiler calculates:

```text
address_of_arr + 2 * sizeof(int)
```

If `arr` starts at address `1000`, then:

```text
arr + 2 = 1000 + 2 * 4 = 1008
```

Then it dereferences address `1008`.

---

# 5. Why `a[n] == *(a + n)`

Array indexing is defined in terms of pointer arithmetic:

```cpp
a[n] == *(a + n)
```

Because of this, this weird expression is also valid:

```cpp
n[a] == *(n + a)
```

Example:

```cpp
int arr[]{1, 2, 3, 4, 5};

std::cout << arr[2] << std::endl; // 3
std::cout << 2[arr] << std::endl; // 3
```

This works because:

```cpp
arr[2] == *(arr + 2)
2[arr] == *(2 + arr)
```

Both point to the same address.

But in real code, never write `2[arr]`. It is legal, but unreadable.

---

# 6. Pointer arithmetic depends on type

This is important.

If you have:

```cpp
int* p;
```

then:

```cpp
p + 1
```

does not mean “add 1 byte”.

It means:

```text
move by sizeof(int) bytes
```

If `sizeof(int) == 4`, then:

```text
p + 1 moves 4 bytes forward
```

For:

```cpp
double* p;
```

`p + 1` usually moves 8 bytes forward.

Pointer arithmetic is scaled by the pointed type.

That is why this works:

```cpp
int arr[]{1, 2, 3, 4, 5};

int* p = arr;
std::cout << *(p + 3); // arr[3]
```

The CPU itself works with byte addresses, but C++ pointer arithmetic abstracts this by multiplying by `sizeof(T)`.

---

# 7. Negative indexing through pointers

Your code:

```cpp
int arr[]{1, 2, 3, 4, 5};

int* parr = arr + 3; 
std::cout << parr[-1] << std::endl; // 3
```

Memory:

```text
index:    0   1   2   3   4
value:    1   2   3   4   5
address:  A   A+4 A+8 A+12 A+16
```

After:

```cpp
int* parr = arr + 3;
```

`parr` points to `arr[3]`, value `4`.

Then:

```cpp
parr[-1]
```

means:

```cpp
*(parr - 1)
```

So it accesses `arr[2]`.

This is valid only because the resulting pointer still points inside the same array.

Valid:

```cpp
parr[-1] // arr[2]
parr[-2] // arr[1]
parr[-3] // arr[0]
```

Undefined behavior:

```cpp
parr[-4] // before arr[0]
```

C++ allows pointer arithmetic only inside the same array object, plus one-past-the-end.

This is valid as a pointer value:

```cpp
int* end = arr + 5;
```

But dereferencing it is invalid:

```cpp
*end; // undefined behavior
```

---

# 8. Why arrays cannot be assigned or incremented

Your notes:

```cpp
// a = b
// a++
// a += 1
```

Example:

```cpp
int a[5];
int b[5];

a = b; // error
```

Why?

Because `a` is the array object itself, not a pointer variable. The name `a` can decay to pointer in expressions, but it is not a modifiable pointer.

This works:

```cpp
int* p = a;
p++;
```

Because `p` is a real pointer variable.

This does not:

```cpp
a++;
```

Because you cannot move the array object. Its address is fixed for its lifetime.

Also, raw arrays do not have built-in assignment semantics. If you want assignable arrays, use:

```cpp
std::array<int, 5> a;
std::array<int, 5> b;

a = b; // OK
```

`std::array` is a wrapper around a raw array with value semantics.

---

# 9. `sizeof(array)` vs `sizeof(pointer)`

Your code:

```cpp
int a[5];

int* pa = a; 
std::cout << "pa: " << sizeof(pa) 
          << " , a: " << sizeof(a) << std::endl;
```

Possible output on a 64-bit system:

```text
pa: 8 , a: 20
```

Why?

```cpp
sizeof(pa)
```

is the size of a pointer. On a 64-bit system, pointers are usually 8 bytes.

```cpp
sizeof(a)
```

is the size of the entire array:

```text
5 * sizeof(int) = 5 * 4 = 20
```

Important: arrays do not decay in `sizeof`.

These are special cases where array-to-pointer decay does not happen:

```cpp
sizeof(a)
&a
decltype(a)
```

Example:

```cpp
int a[5];

std::cout << sizeof(a) << "\n";    // 20
std::cout << sizeof(&a) << "\n";   // 8, pointer to whole array
```

`&a` has type:

```cpp
int (*)[5]
```

Pointer to array of 5 integers.

But `a` in most expressions decays to:

```cpp
int*
```

Pointer to first element.

These are different:

```cpp
a      // usually int* after decay
&a     // int (*)[5]
```

---

# 10. Arrays as function parameters

Your code:

```cpp
void f(int* a) {
    std::cout << a[2] << std::endl; 
}
```

You noted that this cannot be overloaded with:

```cpp
void f(int a[3]);
```

Correct.

In function parameters, these are equivalent:

```cpp
void f(int* a);
void f(int a[]);
void f(int a[3]);
void f(int a[100]);
```

All are treated as:

```cpp
void f(int* a);
```

The size is ignored.

That means this function:

```cpp
void f(int a[3]) {
    std::cout << sizeof(a);
}
```

does not know the array has 3 elements. Inside the function, `a` is just a pointer.

On a 64-bit system:

```cpp
sizeof(a)
```

will likely print:

```text
8
```

not:

```text
12
```

This is why C APIs often pass pointer + size:

```cpp
void f(int* a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        std::cout << a[i] << " ";
    }
}
```

Modern C++ alternatives:

```cpp
std::array<int, 5>       // fixed size, value semantics
std::vector<int>         // dynamic size, owns memory
std::span<int>           // non-owning view over contiguous memory
```

`std::span` is especially important because it is basically a safe pair:

```text
pointer + size
```

---

# 11. Dynamic arrays with `new[]`

Your code:

```cpp
int* a = new int[12];   // uninitialized values
int* b = new int[5]{};  // 0 0 0 0 0
```

`new int[12]` allocates an array on the heap.

For fundamental types:

```cpp
new int[12]
```

default-initializes the elements. For `int`, that means they are uninitialized.

This:

```cpp
new int[5]{}
```

value-initializes elements, so they become zero.

The returned pointer points to the first element:

```text
a ---> a[0] a[1] a[2] ... a[11]
```

But the allocation itself may contain extra hidden information before `a[0]`.

---

# 12. Why `delete[]` is needed

Your note:

```cpp
// delete a - UB
delete[] a;
```

This is one of the most important low-level C++ rules:

```text
new      -> delete
new[]    -> delete[]
malloc   -> free
```

Do not mix them.

## Why?

Because array allocation and single-object allocation are different operations.

For a single object:

```cpp
T* p = new T;
delete p;
```

The compiler needs to:

```text
1. call destructor for one T
2. release memory
```

For an array:

```cpp
T* p = new T[n];
delete[] p;
```

The compiler needs to:

```text
1. know how many elements there are
2. call destructor for each element
3. release memory
```

For a type like this:

```cpp
struct Logger {
    Logger()  { std::cout << "ctor\n"; }
    ~Logger() { std::cout << "dtor\n"; }
};
```

If you do:

```cpp
Logger* arr = new Logger[3];
delete[] arr;
```

C++ must call:

```text
arr[2].~Logger()
arr[1].~Logger()
arr[0].~Logger()
```

usually in reverse order of construction.

But how does `delete[]` know there are 3 objects?

Implementation detail: many C++ runtimes store an **array cookie**.

Conceptually, memory may look like this:

```text
[ cookie: number of elements ][ object 0 ][ object 1 ][ object 2 ]
                               ^
                               |
                         pointer returned to user
```

So if you write:

```cpp
Logger* arr = new Logger[3];
```

the real allocated block may contain metadata before `arr`.

The pointer you receive points to the first object, not necessarily to the beginning of the raw allocation.

Then:

```cpp
delete[] arr;
```

can look at the cookie, find the number of elements, call destructors, and release the correct block.

But if you write:

```cpp
delete arr;
```

the compiler treats `arr` as a pointer to a single object.

It may call only:

```cpp
arr[0].~Logger();
```

or do something even worse. Then it passes an incompatible pointer to the deallocation function. That is undefined behavior.

Important nuance: for `int`, there is no destructor. Some implementations may not store a cookie for trivially destructible types, or optimized/sized deallocation may work differently. But the language rule is still strict:

```cpp
int* a = new int[12];
delete a; // undefined behavior
```

Even if it “works” on your machine, it is still wrong.

---

# 13. Why modern C++ avoids raw `new[]`

Raw `new[]` is dangerous because it separates allocation and deallocation manually.

Bad style:

```cpp
int* a = new int[12];

// many lines of code
// possible exception
// possible early return

delete[] a;
```

If an exception or early return happens before `delete[]`, memory leaks.

Modern C++ uses RAII.

Better:

```cpp
std::vector<int> a(12);
```

or:

```cpp
auto a = std::make_unique<int[]>(12);
```

RAII means: resource lifetime is tied to object lifetime.

When `std::vector<int>` goes out of scope, its destructor automatically releases memory.

This is one of the central ideas of C++.

---

# 14. Two-dimensional arrays

Your code:

```cpp
int a[5][5]; // array of 5 arrays of length 5
```

This is not “array of pointers”. It is a real contiguous 2D array.

Type:

```cpp
int[5][5]
```

Meaning:

```text
array of 5 elements,
where each element is array of 5 ints
```

Memory layout is row-major:

```text
a[0][0] a[0][1] a[0][2] a[0][3] a[0][4]
a[1][0] a[1][1] a[1][2] a[1][3] a[1][4]
...
```

In actual memory:

```text
a[0][0], a[0][1], a[0][2], a[0][3], a[0][4],
a[1][0], a[1][1], a[1][2], a[1][3], a[1][4],
...
```

Address formula:

```text
&a[i][j] = base + (i * 5 + j) * sizeof(int)
```

The compiler needs to know the second dimension `5` to calculate the offset.

That is why this works:

```cpp
void f(int (*a)[5]) {
    std::cout << a[2][3];
}
```

But this is not equivalent:

```cpp
void f(int** a);
```

---

# 15. `int a[5][5]` vs `int a[25]`

Your question:

```cpp
int a[5][5];
// what is more effective: a[5][5] or a[25]?
```

Memory-wise, both are contiguous.

```cpp
int a[5][5];
```

and:

```cpp
int b[25];
```

both store 25 integers in one block.

The access:

```cpp
a[i][j]
```

is approximately:

```cpp
b[i * 5 + j]
```

Performance should be essentially the same because the compiler knows the row size at compile time.

The difference is type and interface.

`int a[5][5]` gives natural matrix syntax:

```cpp
a[i][j]
```

`int b[25]` gives manual control:

```cpp
b[i * cols + j]
```

In high-performance code, flat arrays are often used:

```cpp
std::vector<double> matrix(rows * cols);

matrix[i * cols + j]
```

Why?

Because dimensions are often runtime values, and one flat allocation gives better locality than “array of pointers”.

---

# 16. Cache locality and 2D arrays

This matters for performance.

Because memory is row-major, this is cache-friendly:

```cpp
for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
        a[i][j]++;
    }
}
```

You access memory in the same order as it is laid out.

This is less cache-friendly for large arrays:

```cpp
for (int j = 0; j < 5; ++j) {
    for (int i = 0; i < 5; ++i) {
        a[i][j]++;
    }
}
```

Because you jump between rows.

For small `5x5`, it does not matter. For huge matrices, it matters a lot.

Modern CPUs load memory in cache lines, often 64 bytes. If `int` is 4 bytes, one cache line contains about 16 ints. Sequential access uses cache efficiently.

This is why memory layout matters in performance-oriented C++.

---

# 17. Array of pointers vs pointer to array

Your code:

```cpp
int* b[5];   // array of 5 pointers to int 
int (*c)[5]; // pointer to array of 5 ints
```

These are very different.

## `int* b[5]`

```cpp
int* b[5];
```

Means:

```text
b is an array of 5 pointers to int
```

Memory:

```text
b[0] b[1] b[2] b[3] b[4]
 |    |    |    |    |
 v    v    v    v    v
int  int  int  int  int
```

The pointers are contiguous, but the pointed integers can be anywhere.

This is not the same as a contiguous matrix.

## `int (*c)[5]`

```cpp
int (*c)[5];
```

Means:

```text
c is a pointer to an array of 5 ints
```

Example:

```cpp
int a[5][5];

int (*c)[5] = a;
```

Here `c` points to the first row.

Then:

```cpp
c[0][0] // a[0][0]
c[1][0] // a[1][0]
```

When you do:

```cpp
c + 1
```

it moves by:

```text
sizeof(int[5]) = 5 * sizeof(int)
```

So it jumps one entire row.

---

# 18. Why `int[5][5]` does not decay to `int**`

Your notes:

```cpp
void f(int**){}
void f(int(*)[5]){}
void f(int* [5]){}
```

Important rule:

```text
int[5][5] decays to int (*)[5], not int**.
```

Reason:

An array decays only one level.

For:

```cpp
int a[5][5];
```

`a` is an array of 5 arrays of 5 ints.

When passed to a function, it decays to pointer to its first element.

The first element is:

```cpp
int[5]
```

Therefore the decayed type is:

```cpp
int (*)[5]
```

Pointer to array of 5 ints.

This is correct:

```cpp
void f(int (*a)[5]) {
    std::cout << a[2][3];
}

int main() {
    int a[5][5]{};
    f(a);
}
```

This is not correct for `int[5][5]`:

```cpp
void f(int** a);
```

`int**` means pointer to pointer. It expects memory shaped like:

```text
pointer -> pointer -> int
```

But `int[5][5]` is:

```text
25 ints in one contiguous block
```

These are different memory models.

---

# 19. Variable Length Arrays — VLA

Your code:

```cpp
int n; 
std::cin >> n; 

int arr[n];
```

This is a VLA: Variable Length Array.

It is standard C in C99, but it is **not standard C++**.

GCC and Clang may allow it as an extension, but with strict flags like:

```bash
-Wpedantic
```

you should get a warning.

Correct C++:

```cpp
int n;
std::cin >> n;

std::vector<int> arr(n);
```

---

# 20. Why VLA is bad in C++

VLA is not only “non-standard”. There are deeper reasons why it is problematic.

## 1. Stack overflow risk

```cpp
int n;
std::cin >> n;

int arr[n];
```

If user enters:

```text
100000000
```

you try to allocate hundreds of megabytes on the stack.

Stack memory is limited. This can crash immediately.

With `std::vector`, memory is allocated on the heap:

```cpp
std::vector<int> arr(n);
```

Heap is usually much larger and allocation failure can be handled with `std::bad_alloc`.

## 2. Runtime stack pointer adjustment

For fixed-size array:

```cpp
int arr[100];
```

the compiler knows at compile time how much stack space to reserve.

For VLA:

```cpp
int arr[n];
```

the compiler must generate runtime code to adjust the stack pointer by `n * sizeof(int)`.

That makes function prologue/epilogue more complicated.

## 3. Harder optimization

Fixed-size arrays give the compiler more information.

Example:

```cpp
int arr[16];
```

The compiler knows:

```text
size = 16
alignment
bounds for some optimizations
possible unrolling
```

For VLA, the size is unknown at compile time, so some optimizations become harder.

## 4. Bad interaction with C++ object lifetime

C++ has constructors and destructors.

For:

```cpp
std::string arr[n];
```

a VLA would need to construct `n` strings and destroy them correctly during stack unwinding.

This is complicated and not part of standard C++.

C++ prefers abstractions that manage lifetime explicitly:

```cpp
std::vector<std::string> arr(n);
```

## 5. No good error handling

Stack allocation failure usually does not throw a nice C++ exception. It often just crashes.

Heap allocation through `new`/`vector` can throw `std::bad_alloc`.

## 6. Portability

Code with VLA may compile on GCC but fail on MSVC.

For serious C++ code, especially portfolio/CV code, avoid VLA.

Use:

```cpp
std::array<T, N>  // compile-time size
std::vector<T>    // runtime size, owning
std::span<T>      // runtime-size view, non-owning
```

---

# 21. C-strings and null termination

Your code:

```cpp
const char* s = "abc";
```

The literal `"abc"` is not just 3 characters.

It is:

```text
'a' 'b' 'c' '\0'
```

So its type is:

```cpp
const char[4]
```

The last character is the null terminator.

C-string functions like `strlen`, `printf("%s")`, and `std::cout << const char*` read characters until they find `'\0'`.

Example:

```cpp
const char* s = "abc";

std::cout << strlen(s); // 3
std::cout << sizeof("abc"); // 4
```

`strlen` counts logical characters before `'\0'`.

`sizeof("abc")` counts the whole array, including `'\0'`.

---

# 22. Where string literals are stored

Your question:

```cpp
const char* s = "abc";
// is this string saved in static memory? rodata?
```

Yes.

String literals have **static storage duration**. They exist for the whole program execution.

They are usually placed in read-only memory, often `.rodata`.

Example:

```cpp
const char* s = "abc";
```

Conceptually:

```text
.rodata:
    "abc\0"

stack:
    s -> points to "abc\0"
```

If `s` is a local variable, the pointer variable `s` itself is on the stack, but the characters are in static read-only storage.

That is why this is wrong:

```cpp
char* k = "abc"; // invalid in modern C++
```

Correct:

```cpp
const char* k = "abc";
```

If you need a mutable string:

```cpp
char k[] = "abc";
k[0] = 'x'; // OK
```

Difference:

```cpp
const char* k = "abc";
```

means:

```text
k points to read-only static literal
```

But:

```cpp
char k[] = "abc";
```

means:

```text
create local array: 'a' 'b' 'c' '\0'
```

and copy the literal into it.

---

# 23. Internal `'\0'` in string literals

Your code:

```cpp
const char* n = "abc\0def";
std::cout << n << std::endl; // abc only
```

Actual memory:

```text
'a' 'b' 'c' '\0' 'd' 'e' 'f' '\0'
```

Size:

```text
8 characters
```

But C-string printing stops at the first `'\0'`.

So:

```cpp
std::cout << n;
```

prints:

```text
abc
```

And:

```cpp
strlen(n)
```

returns:

```text
3
```

But:

```cpp
sizeof("abc\0def")
```

returns:

```text
8
```

This demonstrates the key limitation of C-strings:

```text
They do not store length.
They rely on a terminator.
```

So a C-string cannot naturally represent embedded null characters as part of text unless you separately store length.

Modern `std::string` can store embedded nulls:

```cpp
std::string s = std::string("abc\0def", 7);

std::cout << s.size(); // 7
```

But printing it may still look confusing because terminal output stops visually at null-like behavior depending on how you output it.

---

# 24. Why `std::cout << char*` prints text, not address

Your code:

```cpp
char* k = "abc";
std::cout << k << std::endl;
```

Correct modern version:

```cpp
const char* k = "abc";
std::cout << k << std::endl;
```

For most pointer types, `std::cout` prints the address:

```cpp
int x = 10;
int* p = &x;

std::cout << p; // address
```

But for:

```cpp
char*
const char*
signed char*
unsigned char*
```

streams have special overloads that treat them as C-strings.

So:

```cpp
std::cout << k;
```

prints characters until `'\0'`.

That is why:

```cpp
std::cout << &k[0] << std::endl;
```

also prints:

```text
abc
```

Because `&k[0]` is also `const char*`.

And:

```cpp
std::cout << &k[1] << std::endl;
```

prints:

```text
bc
```

because it starts printing from the second character.

Memory:

```text
index:  0    1    2    3
value: 'a'  'b'  'c' '\0'
```

`&k[1]` points to `'b'`, so output starts there.

---

# 25. How to print the actual address of `char*`

To print address, cast to `const void*`:

```cpp
const char* k = "abc";

std::cout << static_cast<const void*>(k) << std::endl;
std::cout << static_cast<const void*>(&k[1]) << std::endl;
```

Why `const void*`?

Because stream overload for `const void*` prints pointer value as an address.

So:

```cpp
std::cout << k;
```

means:

```text
print C-string
```

But:

```cpp
std::cout << static_cast<const void*>(k);
```

means:

```text
print pointer address
```

This is a classic C++ overload-resolution trap.

---

# 26. Clean corrected C-string example

```cpp
#include <iostream>
#include <cstring>
#include <string>

int main() {
    const char* s = "abc";

    std::cout << s << '\n';                 // abc
    std::cout << strlen(s) << '\n';         // 3
    std::cout << sizeof("abc") << '\n';     // 4

    const char* n = "abc\0def";

    std::cout << n << '\n';                 // abc
    std::cout << strlen(n) << '\n';         // 3
    std::cout << sizeof("abc\0def") << '\n'; // 8

    const char* k = "abc";

    std::cout << k << '\n';                 // abc
    std::cout << &k[1] << '\n';             // bc
    std::cout << k[1] << '\n';              // b

    std::cout << static_cast<const void*>(k) << '\n';     // address
    std::cout << static_cast<const void*>(&k[1]) << '\n'; // address of 'b'

    std::string real_string("abc\0def", 7);
    std::cout << real_string.size() << '\n'; // 7
}
```

---

# 27. Interview-style summary

## Is an array a pointer?

No. An array is a real object containing elements. But in many expressions it decays to a pointer to its first element.

```cpp
int a[5];
int* p = a; // decay
```

But:

```cpp
sizeof(a)
```

returns the size of the whole array.

## Why does `void f(int a[5])` not preserve size?

Because array parameters are adjusted to pointer parameters.

```cpp
void f(int a[5]);
```

is the same as:

```cpp
void f(int* a);
```

The function receives only an address, not the array size.

## Why is `delete[]` needed?

Because `new[]` creates an array. For arrays of non-trivial objects, C++ must know how many destructors to call. Implementations often store an array cookie before the first element. `delete[]` uses the correct destruction/deallocation path. Using plain `delete` after `new[]` is undefined behavior.

## Why are VLAs bad in C++?

Because they are not standard C++, allocate runtime-sized memory on the stack, risk stack overflow, complicate generated code, reduce portability, interact badly with C++ object lifetime, and provide worse error handling than `std::vector`.

## Does `int[5][5]` decay to `int**`?

No. It decays to:

```cpp
int (*)[5]
```

because the first element of `int[5][5]` is `int[5]`.

## Why does `std::cout << char*` print text?

Because `operator<<` has special overloads for `char*` and `const char*` that interpret them as null-terminated C-strings. To print the address, cast to `const void*`.

---

# 28. Practical engineering lesson

Raw arrays and C-strings are dangerous, but they teach the foundation of C++:

```text
memory layout
pointer arithmetic
stack vs static storage vs heap
array decay
object lifetime
manual deallocation
undefined behavior
cache locality
ABI/runtime implementation details
```

In production C++, you usually prefer:

```cpp
std::array
std::vector
std::string
std::span
std::unique_ptr<T[]>
```

But understanding raw arrays is necessary if you want to understand how STL containers, allocators, low-level APIs, operating systems, networking buffers, and performance-sensitive code actually work under the hood. This connects directly to your long-term goal of building stronger C++/systems/performance fundamentals. 
