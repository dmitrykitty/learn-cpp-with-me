# 2.2. Types of Memory in C++ Programs

Important correction first:

A program usually does **not** directly “get pieces of RAM” in the C++ sense.
A process gets a **virtual address space** from the operating system. Parts of this virtual address space are backed by real RAM only when needed.

Simplified:

```text
C++ program thinks:
    I have addresses.

Operating system manages:
    virtual pages -> physical RAM frames / disk / not mapped

CPU executes:
    virtual address -> TLB/page table -> cache/RAM
```

So when we talk about `stack`, `heap`, `data`, `text`, etc., we mostly talk about **regions of process virtual memory**, not separate physical memory chips.

---

# 1. Hardware memory vs process memory layout

There are two different perspectives.

## Hardware memory hierarchy

This is about performance:

```text
CPU registers
L1 cache
L2 cache
L3 cache
RAM
SSD / swap
```

This matters for high-performance C++ because memory access speed depends on cache locality, TLB misses, page faults, and RAM bandwidth.

## Process virtual memory layout

This is about where program objects live logically:

```text
text/code segment
read-only data
initialized global/static data
zero-initialized global/static data
heap
stack
memory-mapped regions
thread-local storage
shared libraries
```

In this lecture topic, we mostly discuss **process memory layout**.

---

# 2. Simplified process memory layout

Typical simplified layout:

```text
High addresses
+-------------------------------+
| Stack                         |
| grows down on many systems    |
+-------------------------------+
|                               |
| unused / guard pages / mmap   |
|                               |
+-------------------------------+
| Heap                          |
| grows up conceptually         |
+-------------------------------+
| .bss                          |
| zero-initialized static data  |
+-------------------------------+
| .data                         |
| initialized static data       |
+-------------------------------+
| .rodata                       |
| read-only data                |
+-------------------------------+
| .text                         |
| machine code                  |
+-------------------------------+
Low addresses
```

Important: this is a simplified model. Real layouts are more complex because of ASLR, shared libraries, memory-mapped files, multiple stacks, thread-local storage, etc.

---

# 3. Text segment

The **text segment** contains machine code of the program.

Example:

```cpp
void f() {
    int x = 10;
}
```

The compiled instructions for `f()` are stored in the text/code segment.

Properties:

```text
usually read-only
usually executable
shared between processes if possible
contains compiled machine instructions
```

A function itself is code, so it lives in the text segment.

---

## What about static functions?

Example:

```cpp
static void helper() {
}
```

A `static` function still has its machine code in the **text segment**.

The word `static` here means **internal linkage**:

```text
The function name is visible only inside this translation unit.
```

It does **not** mean the function is stored in the `.data` static memory segment.

So:

```text
static function -> code in .text
static variable -> object in .data or .bss
```

---

# 4. Read-only data: `.rodata`

Read-only data usually contains:

```text
string literals
const global objects, sometimes
virtual tables, usually
constant lookup tables, sometimes
```

Example:

```cpp
const char* s = "hello";
```

The pointer variable `s` may be in `.data` or on the stack depending on where it is declared, but the string literal `"hello"` usually lives in read-only memory.

Example:

```cpp
#include <iostream>

int main() {
    const char* s = "hello";
    std::cout << s << '\n';
}
```

Conceptually:

```text
s                 -> pointer variable
"hello"           -> string literal in read-only memory
```

Trying to modify a string literal is undefined behavior:

```cpp
char* p = (char*)"hello";
p[0] = 'H'; // UB, often segfault
```

---

# 5. Data segment: `.data`

The `.data` segment stores **initialized objects with static storage duration**.

Examples:

```cpp
int globalX = 10;

namespace N {
    int value = 20;
}

void f() {
    static int counter = 1;
}
```

These objects exist for the whole program lifetime.

They are initialized before `main()` starts, or during first call in the case of some local statics.

---

# 6. BSS segment: `.bss`

The `.bss` segment stores **zero-initialized static storage objects**.

Examples:

```cpp
int globalA;          // zero-initialized
static int globalB;   // zero-initialized

namespace N {
    int x;            // zero-initialized
}

void f() {
    static int count; // zero-initialized
}
```

These are initialized to zero by default.

Example:

```cpp
#include <iostream>

int globalX;

int main() {
    std::cout << globalX << '\n'; // 0
}
```

Why separate `.bss`?

Because the executable does not need to physically store many zeros in the file. It can just say:

```text
Reserve N bytes and initialize them to zero.
```

---

# 7. Where are different objects stored?

## Global variable

```cpp
int x = 5;
```

Usually:

```text
.data
```

## Zero-initialized global variable

```cpp
int x;
```

Usually:

```text
.bss
```

## Namespace variable

```cpp
namespace N {
    int x = 10;
}
```

Usually:

```text
.data
```

or `.bss` if zero-initialized.

Namespace variables are still static storage duration objects.

## Local variable

```cpp
void f() {
    int x = 10;
}
```

Usually:

```text
stack
```

unless optimized into a register or optimized away.

## Local static variable

```cpp
void f() {
    static int x = 10;
}
```

Usually:

```text
.data
```

or `.bss` if zero-initialized.

It is local in **scope**, but not local in **lifetime**.

```text
scope: only visible inside f()
lifetime: whole program
storage: static storage area
```

## Static array

Depends on where it is declared.

```cpp
int globalArr[1000]; // .bss
```

```cpp
void f() {
    static int arr[1000]; // .bss or .data
}
```

```cpp
void g() {
    int arr[1000]; // stack
}
```

```cpp
void h() {
    int* arr = new int[1000]; // arr pointer on stack, elements on heap
}
```

---

# 8. Stack memory

The stack is used for function calls and automatic local variables.

Example:

```cpp
void f() {
    int x = 10;
    int y = 20;
}
```

`x` and `y` usually live on the stack, unless optimized into registers or removed.

Important correction:

The stack pointer does not necessarily move after every single variable declaration.
Compilers usually reserve stack space for the whole function frame at once.

Conceptual beginner model:

```text
create x -> stack pointer moves
create y -> stack pointer moves again
scope ends -> stack pointer moves back
```

More realistic compiled model:

```text
enter function -> reserve stack frame once
use offsets from stack pointer/frame pointer
leave function -> release whole frame
```

---

# 9. Stack growth diagram

On many systems, the stack grows downward, toward lower addresses.

Before function call:

```text
High addresses

+-------------------+
| previous frames   |
+-------------------+
| free stack space   |
+-------------------+ <- stack pointer

Low addresses
```

After entering function `f()`:

```text
High addresses

+---------------------------+
| caller stack frame        |
+---------------------------+
| return address            |
+---------------------------+
| saved registers           |
+---------------------------+
| local variable x          |
+---------------------------+
| local variable y          |
+---------------------------+ <- stack pointer
| free stack space          |

Low addresses
```

Example:

```cpp
void f() {
    int x = 1;
    int y = 2;
}
```

Possible stack frame:

```text
+---------------------------+
| return address            |
+---------------------------+
| saved old frame pointer   |
+---------------------------+
| x                         |
+---------------------------+
| y                         |
+---------------------------+
| padding/alignment         |
+---------------------------+
```

The exact order is not guaranteed. The compiler can reorder variables, keep them in registers, remove them, add padding, or optimize the whole function away.

---

# 10. Function call stack example

Code:

```cpp
#include <iostream>

void g() {
    int c = 3;
}

void f() {
    int a = 1;
    int b = 2;
    g();
}

int main() {
    f();
}
```

When `main()` calls `f()`, and `f()` calls `g()`, stack may look conceptually like this:

```text
High addresses

+---------------------------+
| main() frame              |
+---------------------------+
| return address to main    |
+---------------------------+
| f(): local a              |
+---------------------------+
| f(): local b              |
+---------------------------+
| return address to f       |
+---------------------------+
| g(): local c              |
+---------------------------+ <- stack pointer

Low addresses
```

When `g()` returns:

```text
g() frame is removed
stack pointer moves back to f() frame
```

When `f()` returns:

```text
f() frame is removed
stack pointer moves back to main() frame
```

---

# 11. What is saved during a function call?

Besides local variables and return address, a stack frame can contain:

```text
return address
old frame pointer
local variables
function arguments not passed in registers
spilled registers
saved callee-saved registers
temporary compiler-generated values
alignment padding
exception handling related data indirectly
security canary / stack protector value
```

On modern x86-64 systems, many function arguments are passed in registers first.

For example, on System V AMD64 ABI:

```text
first integer/pointer args:
    RDI, RSI, RDX, RCX, R8, R9
```

Additional arguments may be passed on the stack.

On Windows x64, there is also “shadow space” reserved by the caller.

---

# 12. How much memory does one function call need?

There is no single answer.

It depends on:

```text
number and size of local variables
compiler
optimization level
target architecture
ABI/calling convention
whether function is inlined
whether registers need to be saved
whether stack protector is enabled
alignment requirements
```

Example:

```cpp
void f() {
}
```

May need almost nothing, or only return address. It may also be inlined, so no stack frame exists.

Example:

```cpp
void g() {
    int arr[1000];
}
```

This needs about:

```text
1000 * sizeof(int) = 4000 bytes
```

for the local array, plus frame overhead.

Example:

```cpp
void h() {
    int* arr = new int[1000];
}
```

Here:

```text
arr pointer -> stack, usually 8 bytes
1000 ints   -> heap
```

---

# 13. Stack size

Stack size is determined by the OS, runtime, linker settings, and thread settings.

Typical examples:

```text
Linux main thread: often around 8 MB by default
Windows main thread: often around 1 MB by default
pthread-created threads: configurable
```

On Linux you can check:

```bash
ulimit -s
```

Example output:

```text
8192
```

This means:

```text
8192 KB = 8 MB stack limit
```

But this is not a C++ language guarantee. It is platform-specific.

---

# 14. Stack overflow

Stack overflow happens when the stack grows beyond its allowed region.

Example:

```cpp
void recursive() {
    int arr[1000];
    recursive();
}

int main() {
    recursive();
}
```

Each call creates a new stack frame. Eventually the stack hits a guard page or unmapped memory.

On Linux, this usually causes:

```text
Segmentation fault
```

On Windows, it may be reported as stack overflow exception.

Important:

```text
Stack overflow is usually not a C++ exception.
It is usually an OS/hardware-level fault.
```

---

# 15. Heap / dynamic memory

The heap is used for dynamic allocation during runtime.

Example:

```cpp
int* p = new int;
delete p;
```

The heap is not a physical hardware memory type. It is a region of your process virtual address space managed by allocators.

When actively used, heap pages are usually backed by RAM.

---

# 16. What happens in `int* p = new int;`?

Code:

```cpp
int* p = new int;
```

Conceptual steps:

```text
1. The new-expression asks for memory for one int.
2. It calls operator new(sizeof(int)).
3. The allocator finds or requests a suitable memory block.
4. The int object lifetime begins in that memory.
5. Because this is new int without (), the int is default-initialized.
6. For int, default-initialization means the value is indeterminate.
7. The address is returned and stored in p.
```

Important difference:

```cpp
int* p1 = new int;   // uninitialized int, indeterminate value
int* p2 = new int(); // value-initialized int, value is 0
int* p3 = new int(5); // initialized with 5
```

Example:

```cpp
#include <iostream>

int main() {
    int* p = new int(5);

    std::cout << *p << '\n';

    delete p;
}
```

---

# 17. What does the allocator do?

For small allocations, `new` usually does not ask the OS for exactly 4 bytes.

Instead, allocator may already have a larger heap arena.

Simplified:

```text
new int
    -> call operator new(4)
    -> malloc-like allocator
    -> find a small block in allocator cache/free list
    -> maybe split a larger block
    -> return pointer
```

If allocator needs more memory:

```text
allocator asks OS using:
    brk/sbrk on Unix-like systems, historically
    mmap on Unix-like systems
    VirtualAlloc on Windows
```

Returned block may include allocator metadata around it.

Conceptual heap block:

```text
+-----------------------+
| allocator metadata    |
+-----------------------+
| user object: int      | <- p points here
+-----------------------+
| padding/alignment     |
+-----------------------+
```

Even though `int` needs 4 bytes, allocator overhead can make real cost larger.

---

# 18. `delete p`

Code:

```cpp
delete p;
```

Conceptual steps:

```text
1. If p is nullptr, do nothing.
2. Destroy the object pointed to by p.
3. Call operator delete to return memory to allocator.
```

For `int`, destruction is trivial, but for classes destructor matters.

Example:

```cpp
struct User {
    ~User() {
        std::cout << "destroyed\n";
    }
};

int main() {
    User* u = new User;
    delete u; // calls destructor, then frees memory
}
```

Important:

```text
delete returns memory to the allocator.
It does not always immediately return memory to the operating system.
```

The allocator may keep memory for future allocations.

---

# 19. Dynamic arrays: `new[]` and `delete[]`

Code:

```cpp
int* pa = new int[10'000];
delete[] pa;
```

`pa` points to the first element:

```text
pa        -> &pa[0]
pa + 1    -> &pa[1]
pa + 9999 -> &pa[9999]
```

Correct deallocation:

```cpp
delete[] pa;
```

Not:

```cpp
delete pa; // UB
```

---

# 20. Why is `delete[]` important?

Because arrays may require destruction of every element.

Example:

```cpp
#include <iostream>

struct X {
    ~X() {
        std::cout << "destroy\n";
    }
};

int main() {
    X* arr = new X[3];

    delete[] arr; // calls destructor 3 times
}
```

For arrays, the runtime may need to know how many elements were allocated.

How can it know?

Often, implementations store an **array cookie** before the pointer returned to you.

Conceptually:

```text
real allocated block:
+-----------------------+
| array length/cookie   |
+-----------------------+
| arr[0]                | <- pointer returned to user
+-----------------------+
| arr[1]                |
+-----------------------+
| arr[2]                |
+-----------------------+
```

Then `delete[] arr` can read the cookie and call destructors for all elements.

For simple types like `int`, the compiler/runtime may not need a cookie, especially with sized deallocation and trivial destructors. But this is implementation detail.

The rule is still:

```text
new      must be matched with delete
new[]    must be matched with delete[]
malloc   must be matched with free
```

Mismatching them is undefined behavior.

---

# 21. Memory leak example

Your code:

```cpp
#include <iostream>

void f() {
    int* pa = new int();
    std::cout << pa << '\n';
}

int main() {
    while (true) {
        f();
    }
}
```

This leaks memory.

Why?

Inside `f()`:

```cpp
int* pa = new int();
```

allocates one `int` on the heap.

At the end of `f()`:

```text
pa local pointer is destroyed
but the heap int is not deleted
```

So the pointer value is lost. You no longer have a way to call:

```cpp
delete pa;
```

Each loop iteration leaks another allocation.

Correct version:

```cpp
#include <iostream>

void f() {
    int* pa = new int();
    std::cout << pa << '\n';
    delete pa;
}

int main() {
    while (true) {
        f();
    }
}
```

Better modern C++:

```cpp
#include <iostream>
#include <memory>

void f() {
    auto pa = std::make_unique<int>(0);
    std::cout << pa.get() << '\n';
}
```

Here memory is freed automatically when `pa` goes out of scope.

---

# 22. Is memory actually allocated for `new int()` if we do not write to it?

Conceptually, yes:

```cpp
int* pa = new int();
```

creates an `int` object on the free store and returns a pointer to it.

But physically, the OS and allocator may use lazy allocation.

There are several levels:

## C++ level

```text
An int object is created.
pa points to it.
Since you wrote new int(), the int is value-initialized to 0.
```

## Allocator level

The allocator may not ask the OS for 4 bytes each time. It may already have a large heap arena and return a small block from it.

## OS virtual memory level

The OS works with pages, commonly 4096 bytes.

Physical RAM may be assigned lazily:

```text
virtual page reserved/mapped
physical RAM frame assigned only when the page is touched
```

This is called demand paging.

## Linux overcommit

On Linux, memory allocation can sometimes appear to succeed even if the system does not have enough physical RAM for all allocated virtual memory.

The process may be killed later when it actually touches too much memory.

---

## In your exact code

```cpp
int* pa = new int();
std::cout << pa << '\n';
```

`new int()` value-initializes the int to zero, so conceptually the object has value `0`.

This may involve writing to the allocated memory, but exact physical behavior depends on compiler, allocator, OS, and optimization.

Even if the user bytes were not immediately backed by a new physical page, the allocation still consumes allocator metadata and virtual address space. With an infinite loop and no `delete`, memory usage will eventually grow and the program may become slow, throw `std::bad_alloc`, or be killed by the OS.

Important mental model:

```text
new creates a C++ object.
The allocator manages heap blocks.
The OS backs virtual pages with RAM lazily.
These are different layers.
```

---

# 23. Why the same address may appear many times

If you add `delete`, like this:

```cpp
void f() {
    int* pa = new int();
    std::cout << pa << '\n';
    delete pa;
}
```

You may see the same pointer address repeatedly.

Why?

Because allocator reuses freed blocks.

```text
allocate int -> address A
delete int   -> block A goes to free list
allocate int -> allocator gives block A again
```

Without `delete`, addresses usually keep changing because memory is leaked.

---

# 24. `delete p, pp`

Expression:

```cpp
delete p, pp;
```

is parsed as:

```cpp
(delete p), pp;
```

because comma operator has very low precedence.

Meaning:

```text
1. delete p
2. evaluate pp
3. discard result of pp
```

It does **not** delete `pp`.

Example:

```cpp
int* p = new int(1);
int* pp = new int(2);

delete p, pp; // deletes only p, pp is leaked
```

Correct:

```cpp
delete p;
delete pp;
```

Also be careful:

```cpp
delete (p, pp);
```

This uses comma operator inside parentheses.

It evaluates `p`, discards it, then returns `pp`, so it deletes only `pp`.

```text
delete (p, pp) -> delete pp
```

Still not what you usually want.

---

# 25. Heap allocation and RAII

Manual memory management:

```cpp
int* p = new int(5);

// many lines of code

delete p;
```

Problem: if exception or early return happens before `delete`, memory leaks.

Modern C++ prefers RAII:

```cpp
#include <memory>

auto p = std::make_unique<int>(5);
```

Now memory is automatically released when `p` goes out of scope.

For arrays:

```cpp
std::vector<int> v(10'000);
```

instead of:

```cpp
int* pa = new int[10'000];
delete[] pa;
```

Best rule:

```text
Prefer std::vector for dynamic arrays.
Prefer std::unique_ptr for single dynamic ownership.
Avoid raw owning pointers.
Use raw pointers mainly as non-owning observers.
```

---

# 26. Stack vs heap example

```cpp
#include <vector>

void f() {
    int x = 10;                  // stack
    int arr[1000];               // stack
    int* p = new int(5);          // p on stack, int on heap
    std::vector<int> v(1000);     // v object on stack, elements on heap

    delete p;
}
```

Memory locations:

```text
x:
    stack, unless optimized

arr:
    stack

p:
    pointer variable on stack

*p:
    int object on heap

v:
    vector object on stack

v elements:
    heap
```

For `std::vector<int> v(1000);`, the vector object usually contains three pointers:

```text
begin
end
capacity_end
```

The actual elements are stored dynamically on heap.

---

# 27. Important nuance: stack variables may be optimized away

Example:

```cpp
void f() {
    int x = 5;
}
```

The compiler may remove `x` completely because it has no observable effect.

Or:

```cpp
int add(int a, int b) {
    int c = a + b;
    return c;
}
```

`c` may live only in a CPU register, not on the stack.

So when we say “local variables are on the stack”, we mean:

```text
Conceptually they have automatic storage duration.
In real optimized machine code, they may be in registers or optimized away.
```

---

# 28. Summary table

```text
.text:
    machine code of functions

.rodata:
    string literals, read-only constants, vtables usually

.data:
    initialized global/static variables

.bss:
    zero-initialized global/static variables

stack:
    automatic local variables, function call frames

heap:
    dynamic allocations: new, malloc, vector elements, string buffers

mmap/shared regions:
    shared libraries, memory-mapped files, large allocations

TLS:
    thread_local variables
```

---

# 29. Interview-style summary

A C++ program runs inside a process virtual address space. This address space is divided into regions such as text, read-only data, initialized data, BSS, heap, stack, memory-mapped regions, and thread-local storage. These are logical memory regions; physical RAM is managed by the operating system using virtual memory and pages.

The text segment contains machine code. The data segment contains initialized global and static variables. The BSS segment contains zero-initialized global and static variables. String literals and vtables usually live in read-only memory. Local automatic variables usually live on the stack, although the compiler may optimize them into registers or remove them.

The stack stores function call frames. A frame may contain the return address, old frame pointer, local variables, spilled registers, saved registers, alignment padding, arguments, and security metadata. Stack size is platform-dependent, often around 8 MB on Linux by default, and stack overflow usually causes a segmentation fault.

The heap is used for dynamic memory allocation during runtime. `new int` calls an allocation function, creates an `int` object, and returns its address. `delete` destroys the object and returns memory to the allocator. For arrays allocated with `new[]`, `delete[]` must be used because the runtime may need to call destructors for every element and may store array size metadata. Mismatching `new/delete` and `new[]/delete[]` is undefined behavior.

Modern C++ usually avoids raw owning pointers. Prefer `std::vector` for dynamic arrays and `std::unique_ptr` for single-object dynamic ownership.
