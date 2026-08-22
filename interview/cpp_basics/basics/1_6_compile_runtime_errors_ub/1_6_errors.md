# Compile-time errors, runtime errors, and UB in C++

C++ errors can be divided into several categories:

```text id="4ikkqv"
1. Compile-time errors
2. Runtime errors
3. Exceptions
4. Undefined behavior
5. Implementation-defined / unspecified behavior
```

This topic is very important in C++ because the language gives you a lot of control, but also allows many dangerous situations if you access invalid memory, use uninitialized values, or rely on unspecified evaluation order. It is especially relevant for systems/performance-oriented C++ learning. 

---

# 1. Compile-time errors

A **compile-time error** is detected before the program runs.

The compiler analyzes your source code and rejects invalid code.

Common groups:

```text id="qxyk8u"
lexical errors
syntax errors
semantic errors
```

---

## 1.1 Lexical errors

A lexical error happens when the compiler cannot split the source code into valid tokens.

Tokens are things like:

```text id="iysiff"
keywords
identifiers
operators
literals
punctuation
```

Example:

```cpp id="xbd5f6"
int main() {
    \3;
}
```

The compiler does not understand this as valid C++ tokens.

Another example:

```cpp id="y0d62p"
int main() {
    int x = 10 @ 5;
}
```

`@` is not a valid C++ operator.

---

## 1.2 Syntax errors

A syntax error means the tokens are valid, but their structure does not match C++ grammar.

Example:

```cpp id="r287aa"
int main() {
    int x = 5 + ;
}
```

The compiler sees binary operator `+`, but the second operand is missing.

Another example:

```cpp id="fhj0id"
int main() {
    if (true {
    }
}
```

The condition is not correctly closed with `)`.

---

## 1.3 Semantic errors

A semantic error means the syntax is structurally valid, but the meaning is invalid.

Example:

```cpp id="2ev8tu"
int main() {
    int x = "abc";
}
```

The compiler understands the structure, but assigning a string literal to `int` is invalid.

Another example:

```cpp id="5ms3xr"
int main() {
    int x = 5;
    ++x++;
}
```

Why is this invalid?

```cpp id="ehnlx1"
x++
```

returns the old value as a temporary rvalue. Then:

```cpp id="kp6o8a"
++(x++)
```

tries to increment a temporary value, which is not allowed for built-in integers.

---

# 2. Runtime errors

A **runtime error** happens while the program is running.

Examples:

```text id="29fq29"
segmentation fault
division by zero trap
uncaught exception
abort
memory corruption
stack overflow
```

Important: not all runtime errors are C++ exceptions.

For example:

```text id="vtvx4y"
segmentation fault -> OS signal
integer division by zero -> usually CPU trap + OS signal
std::out_of_range -> C++ exception
abort -> process termination
```

---

# 3. Segmentation fault

A **segmentation fault** usually means:

```text id="txqq8l"
The program tried to access memory that it is not allowed to access.
```

Example:

```cpp id="tdw5l6"
int* p = nullptr;
*p = 10; // likely segmentation fault
```

The program tries to write through address `0`, which is not mapped for user-space access.

The OS protects memory using virtual memory and page tables. If your program accesses a virtual address that is not mapped or does not have the required permissions, the CPU raises a fault, and the OS usually sends your process `SIGSEGV`.

---

# 4. Why does `v[500]` not always segfault?

Your example:

```cpp id="s8mgni"
#include <vector>

int main() {
    std::vector<int> v(10);
    v[500] = 1;
}
```

This is already **undefined behavior**.

Why?

`std::vector<int> v(10)` creates 10 elements:

```text id="ahvfle"
valid indexes: 0..9
```

But:

```cpp id="54b7cp"
v[500]
```

accesses outside the vector.

Important:

```cpp id="i99fco"
operator[] does not check bounds.
```

So this:

```cpp id="bmkumj"
v[500] = 1;
```

just calculates an address far after the beginning of the vector’s internal array and writes there.

It may crash, but it may also appear to work.

---

## Why no immediate crash?

Because memory protection usually works at the level of **pages**, not individual C++ objects.

A typical memory page is:

```text id="p3asne"
4096 bytes
```

Your vector stores 10 integers:

```text id="28xfcf"
10 * 4 bytes = 40 bytes
```

But the heap allocator and OS may have mapped a larger memory area around it.

So `v[500]` means approximately:

```text id="wwzb8b"
500 * sizeof(int) = 2000 bytes after v[0]
```

This address may still be inside a mapped heap page.

So the OS says:

```text id="6dhv5o"
This address belongs to your process, so I will not stop you.
```

But from the C++ point of view, it is still invalid because it is outside the vector object.

So:

```text id="7x49cv"
No crash does not mean no bug.
```

It means:

```text id="ucehlo"
The undefined behavior did not visibly explode this time.
```

---

# 5. Why can `v[50'000]` segfault?

Example:

```cpp id="q3v7x9"
std::vector<int> v(10);
v[50'000] = 1;
```

Now the program writes approximately:

```text id="hpkn9e"
50,000 * 4 = 200,000 bytes
```

after the start of the vector storage.

This is much more likely to hit:

```text id="b8j2sn"
unmapped memory
protected memory
guard pages
another invalid virtual page
```

Then the CPU detects invalid memory access, the OS sends `SIGSEGV`, and you get:

```text id="4t3fas"
Segmentation fault (core dumped)
```

But still, both examples are UB:

```cpp id="jqq26d"
v[500] = 1;      // UB, may not crash
v[50'000] = 1;   // UB, may crash
```

---

# 6. `operator[]` vs `.at()`

`operator[]` does not check bounds:

```cpp id="md7bkj"
std::vector<int> v(10);

v[500] = 1; // UB
```

`.at()` checks bounds:

```cpp id="gkwhr1"
std::vector<int> v(10);

v.at(500) = 1; // throws std::out_of_range
```

Example:

```cpp id="g03dk0"
#include <iostream>
#include <vector>
#include <stdexcept>

int main() {
    std::vector<int> v(10);

    try {
        v.at(500) = 1;
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << std::endl;
    }
}
```

So:

```text id="cswox1"
v[i]     -> fast, no checks, UB if invalid
v.at(i)  -> checked, throws exception if invalid
```

---

# 7. Why does `v.at(10)` give `Aborted (core dumped)`?

Example:

```cpp id="5lvbq3"
std::vector<int> v(10);
v.at(10);
```

Valid indexes are:

```text id="vx9bsj"
0..9
```

So:

```cpp id="bsa2zf"
v.at(10)
```

throws:

```cpp id="msgz7p"
std::out_of_range
```

If you do not catch the exception, C++ calls:

```cpp id="1u69no"
std::terminate()
```

Usually `std::terminate()` calls:

```cpp id="orexja"
std::abort()
```

Then the OS prints something like:

```text id="b3xd6h"
Aborted (core dumped)
```

So this is not a segmentation fault. This is an uncaught C++ exception that terminated the program.

---

# 8. Integer division by zero

Your example:

```cpp id="jmsani"
int y = 1;
std::cout << y / 0;
```

In C++, integer division by zero is **undefined behavior**.

On many CPUs, integer division by zero triggers a hardware exception/trap.

On Linux/Unix-like systems, the OS often converts this into signal:

```text id="bfia7v"
SIGFPE
```

The terminal may print:

```text id="wnt2qp"
Floating point exception (core dumped)
```

This name is confusing.

Even though the message says “floating point exception”, it can also happen for integer arithmetic errors like integer division by zero.

So:

```text id="8ofvgr"
int division by zero -> UB in C++
often CPU trap -> OS signal SIGFPE
```

---

# 9. Why does `5.0 / 0` give infinity?

Example:

```cpp id="z8c4nl"
double x = 5.0 / 0.0;
std::cout << x;
```

For IEEE 754 floating-point arithmetic, division by zero does not necessarily crash.

Instead:

```text id="94pdfv"
positive / +0.0 -> +inf
negative / +0.0 -> -inf
0.0 / 0.0       -> NaN
```

Example:

```cpp id="lbsb8f"
#include <iostream>
#include <cmath>

int main() {
    double a = 5.0 / 0.0;
    double b = -5.0 / 0.0;
    double c = 0.0 / 0.0;

    std::cout << a << std::endl; // inf
    std::cout << b << std::endl; // -inf
    std::cout << c << std::endl; // nan
}
```

Why?

Floating-point numbers have special values:

```text id="bv2dft"
+infinity
-infinity
NaN
positive zero
negative zero
```

IEEE 754 was designed for numerical computation, where continuing with `inf` or `nan` can be more useful than immediately crashing.

By default, most floating-point exceptions are non-trapping. They set status flags instead of killing the program.

---

# 10. `v[-1]` and memory corruption

Your example:

```cpp id="g6sqwn"
std::vector<int> v(10);
v[-1] = 1000;
```

This is also undefined behavior.

Important detail:

`std::vector::operator[]` takes an unsigned index type:

```cpp id="iy3iht"
std::vector<int>::size_type
```

Usually this is:

```cpp id="8j97j4"
std::size_t
```

So:

```cpp id="p3an8e"
v[-1]
```

does not really mean “index minus one” in a safe checked way.

The `-1` is converted to a huge unsigned value.

On a 64-bit system:

```text id="h58ihq"
-1 converted to size_t = 18446744073709551615
```

Then vector tries to access:

```cpp id="py4fof"
v[18446744073709551615]
```

Of course, this is invalid.

Depending on implementation and pointer arithmetic behavior, it can effectively write before or far outside the allocated vector buffer.

---

## Why can `v[-1]` cause “double free or corruption”?

Heap allocators store metadata near allocated memory blocks.

A heap allocation may look conceptually like this:

```text id="mygjat"
[ allocator metadata ][ user data ][ maybe more metadata ]
                       ^
                       v.data()
```

If you write before the beginning of the vector data, you may overwrite allocator metadata.

Example:

```cpp id="hshoz1"
v[-1] = 1000;
```

may corrupt memory used by the allocator to manage heap blocks.

Later, when the vector is destroyed and frees its memory, the allocator checks metadata and notices it is corrupted.

Then it may abort the program with:

```text id="6j9k1u"
double free or corruption
Aborted (core dumped)
```

This does not mean `v[-1]` stores some meaningful element.

It means:

```text id="sjp3qc"
You wrote outside the vector and corrupted memory.
```

---

# 11. Uninitialized variable

Example:

```cpp id="3b86ip"
int x;
std::cout << x;
```

For a local automatic variable:

```cpp id="ucpu2t"
int x;
```

`x` is not initialized.

Reading it is undefined behavior.

It may print:

```text id="3btnw6"
0
random value
old stack data
different values in different runs
```

But from the C++ standard point of view:

```text id="nu5f7s"
The program has undefined behavior.
```

Correct:

```cpp id="6cvwvs"
int x = 0;
std::cout << x;
```

Global variables are different:

```cpp id="6fvdey"
int x; // global, zero-initialized
```

But local variables are not automatically initialized.

---

# 12. Undefined behavior from unsequenced modification

Your example:

```cpp id="fszz4w"
int x = 1;
x++ + ++x;
```

This is undefined behavior because `x` is modified more than once in the same expression without proper sequencing.

There are two modifications:

```cpp id="u4libr"
x++
++x
```

The `+` operator does not guarantee whether the left operand or right operand is evaluated first.

So the compiler is not required to produce any meaningful result.

Important:

```text id="tmaenm"
Undefined behavior means the C++ standard gives no requirements.
```

The compiler may:

```text id="suusw2"
produce different results
optimize the code strangely
remove code
crash
appear to work
```

Correct version:

```cpp id="impq74"
int x = 1;

int a = x++;
int b = ++x;

int result = a + b;
```

Now the order is clear.

---

# 13. Undefined behavior is not “random behavior”

Undefined behavior does not mean:

```text id="i8lt2n"
The program randomly chooses a result.
```

It means:

```text id="cfc04p"
The C++ standard imposes no requirements on the program behavior.
```

The compiler can assume UB never happens and optimize based on that assumption.

Example:

```cpp id="8j9tj1"
int foo(int x) {
    return x + 1 > x;
}
```

For signed `int`, overflow is UB.

The compiler may assume:

```text id="rn4l0s"
x + 1 never overflows
```

Therefore:

```cpp id="fexv0x"
x + 1 > x
```

is always true, and the compiler may optimize the function to:

```cpp id="wu2xnj"
int foo(int x) {
    return 1;
}
```

This is why UB is dangerous in optimized C++ code.

---

# 14. Core dump

A **core dump** is a file containing a snapshot of the process memory when it crashed.

It may include:

```text id="a0czlx"
stack
heap
registers
instruction pointer
loaded libraries
thread states
```

It is useful for debugging crashes after they happen.

It is not a compiler flag by itself.

It is usually controlled by the OS/shell.

---

## How to enable core dumps on Linux

Check current limit:

```bash id="tbp7d8"
ulimit -c
```

If it prints:

```text id="eb2dft"
0
```

core dumps are disabled.

Enable them:

```bash id="8glqt9"
ulimit -c unlimited
```

Compile with debug information:

```bash id="rggzju"
g++ -g -O0 main.cpp -o app
```

Run:

```bash id="hio6kk"
./app
```

If it crashes, it may generate a core file.

Then inspect with `gdb`:

```bash id="0z8aiv"
gdb ./app core
```

Inside `gdb`:

```bash id="td8jvl"
bt
```

`bt` means backtrace. It shows where the program crashed.

On many modern Linux systems, core dumps may be managed by `systemd-coredump`, so you can use:

```bash id="6eb3ak"
coredumpctl list
coredumpctl debug ./app
```

---

# 15. `abort`

`abort` means the program intentionally terminates abnormally.

Example:

```cpp id="uu97b0"
#include <cstdlib>

int main() {
    std::abort();
}
```

This usually sends:

```text id="haq8lz"
SIGABRT
```

and the terminal may show:

```text id="i9d66b"
Aborted (core dumped)
```

Common reasons:

```text id="p0cxa5"
uncaught exception
failed assertion
manual std::abort()
heap corruption detected by allocator
std::terminate()
```

Example with assertion:

```cpp id="evbbph"
#include <cassert>

int main() {
    int x = 5;
    assert(x == 10);
}
```

If assertion fails, the program usually aborts.

---

# 16. The as-if rule

The **as-if rule** says:

```text id="oathip"
The compiler may transform the program in any way,
as long as the observable behavior is the same as if the original code was executed.
```

Observable behavior includes things like:

```text id="2c1vd0"
volatile accesses
input/output operations
program termination behavior
some interactions with external environment
```

Example:

```cpp id="d63xnj"
int x = 2 + 3;
std::cout << x;
```

The compiler does not have to actually generate code for `2 + 3`.

It can optimize to:

```cpp id="q41pfj"
std::cout << 5;
```

because observable behavior is the same.

Another example:

```cpp id="hgyx0y"
int x = 0;

for (int i = 0; i < 1000; ++i) {
    x += i;
}
```

If `x` is never used, the compiler may remove the entire loop.

---

## As-if rule and UB

The as-if rule becomes especially powerful when UB is involved.

If your program has undefined behavior, the compiler is allowed to assume that this situation never occurs.

Example:

```cpp id="3tb6y0"
int safe_divide(int a, int b) {
    return a / b;
}
```

Since division by zero is UB for integers, the compiler may assume:

```text id="1tkb63"
b is never zero
```

That assumption can affect optimization.

This is one of the reasons why C++ UB is dangerous in optimized builds.

---

# 17. Your code with comments corrected

```cpp id="uqahca"
#include <iostream>
#include <vector>

// 1.6 Compile-time errors, runtime errors, and UB

int main() {
    // ================== RUNTIME ERRORS / UB ==================

    std::vector<int> v(10);

    // UB: out-of-bounds access.
    // May not segfault because the address may still be in mapped process memory.
    v[500] = 1;

    // UB: more likely to segfault because it is farther away from valid memory.
    // v[50'000] = 1;

    // UB: integer division by zero.
    // Often causes CPU trap and OS signal SIGFPE on Linux.
    int y = 1;
    // std::cout << y / 0;

    // Floating-point division by zero usually gives inf in IEEE 754.
    std::cout << 5.0 / 0.0 << std::endl; // inf

    // Throws std::out_of_range.
    // If uncaught, program terminates, usually with abort.
    // v.at(10);

    // UB: negative index is converted to huge unsigned size_t.
    // Can corrupt heap metadata and later cause "double free or corruption".
    // v[-1] = 1000;

    // ================== UB ==================

    int x;

    // UB: reading uninitialized local variable.
    // std::cout << x;

    x = 1;

    // UB: unsequenced modifications of x.
    // x++ + ++x;
}
```

---

# 18. Practical debugging tools

For C++ UB and memory bugs, useful tools:

```text id="b53jc5"
AddressSanitizer
UndefinedBehaviorSanitizer
Valgrind
gdb
compiler warnings
debug STL mode
```

Compile with sanitizers:

```bash id="ziqlwi"
g++ -g -O1 -fsanitize=address,undefined main.cpp -o app
```

Run:

```bash id="q83gfb"
./app
```

AddressSanitizer can detect:

```text id="zzjc27"
out-of-bounds access
use-after-free
stack buffer overflow
heap buffer overflow
double free
```

UndefinedBehaviorSanitizer can detect:

```text id="x85agp"
signed integer overflow
division by zero
invalid shifts
some uninitialized/invalid operations
```

For interview and real engineering, this is a very important habit:

```text id="1tus7h"
If code has suspicious memory behavior, run it with sanitizers.
```

---

# 19. Interview-style summary

A compile-time error is detected by the compiler before the program runs. Lexical errors happen when the compiler cannot form valid tokens, syntax errors happen when the token structure violates C++ grammar, and semantic errors happen when the code is grammatically valid but meaningless or illegal in C++.

A runtime error happens while the program is executing. Examples include segmentation faults, aborts, uncaught exceptions, and hardware traps such as integer division by zero. A segmentation fault usually means the process accessed memory that is not mapped or not allowed by the OS.

Undefined behavior means the C++ standard gives no requirements for what happens. Out-of-bounds vector access with `operator[]`, reading an uninitialized local variable, signed integer overflow, integer division by zero, and unsequenced modifications like `x++ + ++x` are examples of UB.

`std::vector::operator[]` does not check bounds, so `v[500]` may not crash even though it is invalid. It may write into still-mapped heap memory. `v.at(500)` checks bounds and throws `std::out_of_range`.

A core dump is a snapshot of the crashed process memory, useful for debugging with tools like `gdb`. It is controlled by the operating system, not directly by the compiler.

The as-if rule allows the compiler to optimize code however it wants as long as observable behavior is preserved. However, if code contains UB, the compiler may assume that situation never happens and optimize aggressively, which can lead to surprising results.
