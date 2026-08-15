# C++ Building Steps: Preprocessing, Compilation, Assembling, Linking

## 1. Big picture

When we write:

```bash
g++ main.cpp
```

we usually get an executable file:

```bash
a.out
```

But `g++` is not just “the compiler”. In practice, `g++` is a **driver program**.

It coordinates several tools:

```text
.cpp source code
    ↓ preprocessing
preprocessed source
    ↓ compilation
assembly code
    ↓ assembling
object file
    ↓ linking
executable file
```

The main stages are:

```text
1. Preprocessing
2. Compilation
3. Assembling
4. Linking
```

Important correction:

```text
Each .cpp file does not directly become a.out.
Each .cpp file becomes a translation unit, then usually an object file .o.
The linker combines object files and libraries into the final executable.
```

Example:

```bash
g++ a.cpp b.cpp c.cpp -o program
```

Internally:

```text
a.cpp -> a.o
b.cpp -> b.o
c.cpp -> c.o

a.o + b.o + c.o + libraries -> program
```

---

# 2. Translation unit

A **translation unit** is what the compiler actually compiles.

For a file:

```cpp
#include <iostream>

int main() {
    std::cout << "Hello\n";
}
```

the compiler does not compile only the original `main.cpp`.

First, preprocessing expands includes and macros.

So the translation unit is roughly:

```text
main.cpp + all included headers after preprocessing
```

This is why C++ compilation can be slow.

Including a large header like:

```cpp
#include <iostream>
```

pulls in a lot of declarations/templates/code into the translation unit.

---

# 3. Useful command: verbose mode

To see what `g++` actually runs internally:

```bash
g++ -v main.cpp
```

or:

```bash
g++ -v main.cpp -o program
```

This prints internal commands such as:

```text
cc1plus
as
collect2
ld
```

Example from your logs:

```text
/usr/libexec/gcc/x86_64-linux-gnu/13/cc1plus ...
```

This is the actual C++ compiler frontend used by GCC.

Then:

```text
as -v --64 -o /tmp/xxx.o /tmp/xxx.s
```

This is the assembler.

Then `collect2` is called as a wrapper around the linker.

---

# 4. Stage 1: Preprocessing

## What preprocessing does

Preprocessing handles preprocessor directives:

```cpp
#include
#define
#if
#ifdef
#ifndef
#pragma
```

Example:

```cpp
#include <iostream>
```

Conceptually, this means:

```text
Take the content of the iostream header and insert it into this file.
```

This is often described as “copy-paste”, and as a mental model it is useful.

But real compilers may optimize this internally, especially with precompiled headers/modules, but the language model is textual inclusion.

---

## `<...>` vs `"..."`

### Angle brackets

```cpp
#include <iostream>
```

means:

```text
Search in system include paths.
```

Usually these paths contain standard library headers and system headers.

Examples:

```text
/usr/include
/usr/include/c++/13
/usr/lib/gcc/...
```

You can see include search paths with:

```bash
g++ -v main.cpp
```

Look for:

```text
#include <...> search starts here:
```

---

### Quotes

```cpp
#include "my_header.hpp"
```

usually means:

```text
First search near the current source file.
Then search normal include paths.
```

This is used for your own project headers.

Example:

```cpp
#include "matrix.hpp"
#include "big_integer.hpp"
```

---

# 5. Running only the preprocessor

Command:

```bash
g++ -E prg.cpp > prg2.cpp
```

Meaning:

```text
-E -> stop after preprocessing
```

The output is preprocessed code.

It will contain:

```text
your source code
expanded includes
expanded macros
line markers
```

Example:

```bash
g++ -E main.cpp -o main.ii
```

Common extensions:

```text
.i  -> preprocessed C
.ii -> preprocessed C++
```

---

## Why preprocessing matters

Preprocessing explains several C/C++ mechanisms:

```text
1. Headers are included textually.
2. Macros are not real functions.
3. Include guards prevent duplicate definitions.
4. Template code often must be visible in headers.
5. Compilation time grows with included headers.
```

Example include guard:

```cpp
#ifndef MATRIX_HPP
#define MATRIX_HPP

class Matrix {
    // ...
};

#endif
```

or modern form:

```cpp
#pragma once
```

---

# 6. Macros vs real language constructs

Example:

```cpp
#define SQUARE(x) x * x
```

This is dangerous:

```cpp
SQUARE(1 + 2)
```

expands to:

```cpp
1 + 2 * 1 + 2
```

not:

```cpp
(1 + 2) * (1 + 2)
```

Better macro:

```cpp
#define SQUARE(x) ((x) * (x))
```

But in modern C++, prefer:

```cpp
constexpr int square(int x) {
    return x * x;
}
```

Preprocessor has no type system. It works before compilation.

---

# 7. Stage 2: Compilation

Compilation converts preprocessed C++ source into assembly.

Command:

```bash
g++ -S prg.cpp
```

This produces:

```text
prg.s
```

The `.s` file contains assembly code.

Example:

```bash
g++ -S main.cpp -o main.s
```

---

## What the compiler does internally

Compilation is one of the most complex stages.

Substages include:

```text
1. Lexical analysis / scanning
2. Parsing
3. Building AST
4. Semantic analysis
5. Type checking
6. Template instantiation
7. Optimization
8. Code generation
```

---

## 7.1 Lexical analysis

The compiler turns characters into tokens.

Example:

```cpp
int x = 5;
```

becomes tokens:

```text
int
identifier x
=
integer literal 5
;
```

---

## 7.2 Parsing

The compiler checks grammar.

Example:

```cpp
int x = 5;
```

is recognized as a variable declaration.

But:

```cpp
int = x 5;
```

fails during parsing.

---

## 7.3 AST construction

AST means:

```text
Abstract Syntax Tree
```

Example:

```cpp
int y = x + 3;
```

becomes a tree-like structure:

```text
declaration y
    initializer:
        binary +
            x
            3
```

The compiler reasons about the AST, not directly about raw text.

---

## 7.4 Semantic analysis and type checking

Example:

```cpp
std::string s = 123;
```

The compiler checks:

```text
Can int be converted to std::string?
Which constructor should be used?
Is this legal?
```

This is also where overload resolution happens:

```cpp
f(10);
```

The compiler decides which `f` should be called.

---

## 7.5 Template instantiation

Templates are not normal functions/classes until instantiated.

Example:

```cpp
template<typename T>
T max_value(T a, T b) {
    return a > b ? a : b;
}

int x = max_value(1, 2);
```

The compiler generates something like:

```cpp
int max_value<int>(int a, int b) {
    return a > b ? a : b;
}
```

This is why template code usually lives in headers: the compiler must see template definitions during compilation.

---

## 7.6 Optimization

With optimization flags:

```bash
g++ -O2 main.cpp
```

the compiler may:

```text
inline functions
remove unused code
unroll loops
propagate constants
eliminate dead stores
simplify arithmetic
vectorize loops
```

Example:

```cpp
int x = 2 + 3;
```

may become directly:

```cpp
int x = 5;
```

At high optimization levels, generated assembly may look very different from source code.

---

## 7.7 Code generation

Finally, the compiler emits assembly instructions for the target architecture.

Example architecture:

```text
x86_64
ARM64
RISC-V
```

This is why the compiler command contains flags like:

```text
-march=x86-64
-mtune=generic
```

They control target CPU and tuning.

---

# 8. `cc1plus`

From your verbose logs:

```text
/usr/libexec/gcc/x86_64-linux-gnu/13/cc1plus ...
```

`cc1plus` is the GCC C++ compiler frontend.

Roughly:

```text
cc1plus takes preprocessed/normal C++ source
and produces assembly .s
```

The `g++` command itself is a driver. It calls `cc1plus`, assembler, linker, and adds correct standard C++ libraries.

That is one reason why linking C++ programs with `gcc` can fail.

Example:

```bash
gcc main.cpp
```

may fail with undefined references to C++ standard library symbols.

Usually use:

```bash
g++ main.cpp
```

because `g++` automatically links `libstdc++`.

---

# 9. Stage 3: Assembling

Assembly file:

```text
main.s
```

is human-readable low-level code.

The assembler converts it into machine code inside an object file.

Command:

```bash
g++ -c prg.cpp
```

This stops after producing object file:

```text
prg.o
```

More explicit pipeline:

```bash
g++ -S prg.cpp -o prg.s
as prg.s -o prg.o
```

From your logs:

```text
as -v --64 -o /tmp/cc2rJVwz.o /tmp/ccHzioSB.s
```

`as` is the GNU assembler.

---

# 10. What is an object file?

An object file is binary, but it is not usually directly executable.

Example:

```bash
g++ -c main.cpp -o main.o
```

`main.o` contains:

```text
machine code for functions from main.cpp
global/static data
symbol table
relocation information
debug info if compiled with -g
section metadata
```

On Linux, object files usually use ELF format:

```text
ELF = Executable and Linkable Format
```

You can check:

```bash
file main.o
```

Possible output:

```text
ELF 64-bit LSB relocatable, x86-64
```

The word:

```text
relocatable
```

means:

```text
This file contains code/data, but addresses are not final yet.
The linker still needs to resolve symbols and relocate references.
```

---

# 11. Object file sections

Object files are divided into sections.

Common sections:

```text
.text      -> machine code
.data      -> initialized global/static data
.bss       -> zero-initialized global/static data
.rodata    -> read-only constants, string literals
.symtab    -> symbol table
.rela.text -> relocation entries for .text
.debug_*   -> debug information if -g was used
```

Example:

```cpp
int global = 5;
static int zero;
const char* s = "abc";

int add(int a, int b) {
    return a + b;
}
```

Possible placement:

```text
add() machine code -> .text
global             -> .data
zero               -> .bss
"abc"              -> .rodata
```

---

# 12. Symbols

A symbol is a named entity the linker may need to know about.

Examples:

```text
main
printf
std::cout
my_function
global_variable
```

Use:

```bash
nm main.o
```

Example output may contain:

```text
0000000000000000 T main
                 U printf
```

Meaning:

```text
T main   -> symbol main is defined in text section
U printf -> printf is undefined here; linker must find it elsewhere
```

So an object file can contain references to functions/data that are not defined inside it.

---

# 13. Placeholders and relocations

Your note says:

> In place of function calls somewhere are placeholders.

Good intuition.

More precise:

```text
Object file contains relocation entries.
```

When compiler emits a call to a function whose final address is unknown, it cannot put the final address yet.

Example:

```cpp
void f();

int main() {
    f();
}
```

If `f` is defined in another `.cpp` file, the compiler compiling `main.cpp` does not know where `f` will be in the final executable.

So it emits:

```text
call placeholder
relocation: this place needs address of f
```

The linker later patches this.

This is called **relocation**.

---

# 14. Stage 4: Linking

Linking combines:

```text
object files
static libraries
dynamic library references
startup code
runtime support
```

into an executable.

Example:

```bash
g++ main.o utils.o -o program
```

The linker resolves undefined symbols.

If `main.o` says:

```text
U add
```

and `utils.o` says:

```text
T add
```

the linker connects them.

---

## 14.1 Linker errors

Example:

```cpp
// main.cpp
void f();

int main() {
    f();
}
```

Compile:

```bash
g++ main.cpp
```

You may get:

```text
undefined reference to `f()'
```

This is not a syntax error.

This means:

```text
Compilation succeeded.
But linking failed because f was declared but not defined anywhere.
```

Compiler only needs declaration to type-check the call.

Linker needs actual definition to produce executable.

---

# 15. `collect2`

Your logs mention:

```text
collect2
```

`collect2` is a GCC wrapper around the real linker.

It helps with things like:

```text
constructors/destructors of global objects
initialization/finalization sections
linker invocation details
```

Eventually it invokes the real linker, commonly:

```text
ld
```

or a linker like:

```text
ld.bfd
gold
lld
```

---

# 16. Why C++ linking is more complex than C

C++ has features that require runtime/linker support:

```text
global object constructors
global object destructors
exceptions
RTTI
virtual tables
templates
name mangling
standard library
```

Example:

```cpp
#include <iostream>

int main() {
    std::cout << "Hello\n";
}
```

This requires linking against C++ standard library.

`g++` does that automatically.

---

# 17. Name mangling

C++ supports overloads:

```cpp
void f(int);
void f(double);
```

At linker level, symbols cannot both simply be called `f`.

So C++ compilers encode type information into symbol names.

This is called **name mangling**.

Example:

```bash
nm main.o
```

may show:

```text
_Z1fi
_Z1fd
```

You can demangle:

```bash
nm -C main.o
```

Then you see:

```text
f(int)
f(double)
```

This is why C and C++ linkage differ.

For C-compatible symbols:

```cpp
extern "C" void f();
```

disables C++ name mangling for `f`.

---

# 18. Useful commands for inspecting build artifacts

## `file`

```bash
file a.out
file main.o
```

Shows file type.

Example:

```text
ELF 64-bit LSB pie executable
ELF 64-bit LSB relocatable
```

---

## `hd` / `hexdump`

```bash
hd main.o
```

or:

```bash
hexdump -C main.o
```

Shows raw bytes.

This is useful to remember:

```text
Object files and executables are just binary files in a structured format.
```

But raw bytes are hard to understand without ELF tools.

---

## `readelf`

```bash
readelf -h a.out
```

Shows ELF header.

```bash
readelf -S main.o
```

Shows sections.

```bash
readelf -s main.o
```

Shows symbol table.

```bash
readelf -r main.o
```

Shows relocation entries.

---

## `nm`

```bash
nm main.o
```

Shows symbols.

Useful flags:

```bash
nm -C main.o
```

demangles C++ names.

---

## `objdump`

`objdump` inspects object files/executables.

Common usage:

```bash
objdump -d a.out
```

Disassembles machine code.

With demangling:

```bash
objdump -d -C a.out
```

Intel syntax:

```bash
objdump -d -M intel a.out
```

Disassemble object file:

```bash
objdump -d -C main.o
```

Show all headers:

```bash
objdump -x a.out
```

Show relocation entries:

```bash
objdump -r main.o
```

---

# 19. What is `objdump`?

`objdump` is a tool for displaying information from object files, static libraries, shared libraries, and executables.

It can show:

```text
assembly instructions
sections
symbols
relocations
headers
raw binary data
```

Most useful for learning:

```bash
objdump -d -C -M intel a.out
```

Meaning:

```text
-d        -> disassemble code sections
-C        -> demangle C++ symbols
-M intel  -> use Intel assembly syntax
```

Example:

```bash
g++ -O0 -g main.cpp -o main
objdump -d -C -M intel main
```

This lets you see generated machine code.

---

# 20. Full pipeline manually

Given:

```cpp
// main.cpp
#include <iostream>

int main() {
    std::cout << "Hello\n";
}
```

## Preprocess

```bash
g++ -E main.cpp -o main.ii
```

## Compile to assembly

```bash
g++ -S main.cpp -o main.s
```

## Assemble to object file

```bash
g++ -c main.cpp -o main.o
```

or:

```bash
as main.s -o main.o
```

Usually use `g++ -c`, because it passes correct options.

## Link

```bash
g++ main.o -o main
```

Run:

```bash
./main
```

---

# 21. Multiple files example

Project:

```text
project/
    main.cpp
    math.cpp
    math.hpp
```

`math.hpp`:

```cpp
#pragma once

int add(int a, int b);
```

`math.cpp`:

```cpp
#include "math.hpp"

int add(int a, int b) {
    return a + b;
}
```

`main.cpp`:

```cpp
#include <iostream>
#include "math.hpp"

int main() {
    std::cout << add(2, 3) << '\n';
}
```

Build manually:

```bash
g++ -c main.cpp -o main.o
g++ -c math.cpp -o math.o
g++ main.o math.o -o program
```

If you forget `math.o`:

```bash
g++ main.o -o program
```

you get linker error:

```text
undefined reference to `add(int, int)'
```

Because `main.o` knows `add` exists, but linker cannot find implementation.

---

# 22. Compilation vs linking errors

## Compilation error

Example:

```cpp
int main() {
    x = 5;
}
```

Error:

```text
x was not declared
```

This is compilation stage.

The compiler cannot understand/type-check the code.

---

## Linking error

Example:

```cpp
void f();

int main() {
    f();
}
```

Compilation succeeds because declaration exists.

Linking fails because definition is missing:

```text
undefined reference to `f()'
```

---

# 23. Default executable name: `a.out`

When you run:

```bash
g++ main.cpp
```

the default output executable is:

```text
a.out
```

This name is historical.

Usually better:

```bash
g++ main.cpp -o program
```

Then run:

```bash
./program
```

---

# 24. Debug information: `-g`

Compile with debug info:

```bash
g++ -g main.cpp -o program
```

This adds debug sections to the executable.

It helps tools like:

```text
gdb
lldb
sanitizers
addr2line
```

Debug info maps machine code back to:

```text
source files
line numbers
variable names
types
```

Without `-g`, sanitizer reports and debugger output are less useful.

---

# 25. Optimization levels

Common flags:

```bash
-O0
-O1
-O2
-O3
-Ofast
-Og
```

## `-O0`

No optimization. Best for simple debugging.

```bash
g++ -O0 -g main.cpp
```

## `-Og`

Debug-friendly optimization.

```bash
g++ -Og -g main.cpp
```

## `-O2`

Common production optimization.

```bash
g++ -O2 main.cpp
```

Important:

```text
With optimizations, source lines and machine code may not correspond directly.
Variables may be optimized away.
Functions may be inlined.
```

---

# 26. Sanitizers

Sanitizers are runtime bug detectors.

They work by adding instrumentation during compilation.

Example:

```bash
g++ -fsanitize=address -g p.cpp -o p
```

The compiler inserts extra checks into the program.

Then at runtime, sanitizer can detect bugs.

Sanitizers usually:

```text
increase compilation time
increase runtime overhead
increase memory usage
make binary larger
```

But they are extremely useful during development.

---

# 27. AddressSanitizer: ASan

Command:

```bash
g++ -fsanitize=address -g p.cpp -o p
```

Often recommended:

```bash
g++ -fsanitize=address -g -O1 -fno-omit-frame-pointer p.cpp -o p
```

ASan detects many memory errors:

```text
heap buffer overflow
stack buffer overflow
use after free
use after scope
double free
invalid free
some memory leaks depending on platform/config
```

Example:

```cpp
int* p = new int[3];

p[3] = 10; // out of bounds

delete[] p;
```

ASan can report:

```text
heap-buffer-overflow
```

---

# 28. LeakSanitizer: LSan

Command:

```bash
g++ -fsanitize=leak -g p.cpp -o p
```

Detects memory leaks:

```cpp
int* p = new int(5);
// no delete
```

At program exit, LSan may report leaked memory.

Note:

```text
On many platforms, LeakSanitizer is integrated with AddressSanitizer.
```

So:

```bash
g++ -fsanitize=address -g p.cpp -o p
```

may also detect leaks.

---

# 29. UndefinedBehaviorSanitizer: UBSan

Command:

```bash
g++ -fsanitize=undefined -g p.cpp -o p
```

Detects many kinds of UB, for example:

```text
signed integer overflow
division by zero
invalid shift
null pointer dereference in some cases
out-of-bounds array access in some cases
misaligned pointer access
invalid enum value
```

Example:

```cpp
int x = 2147483647;
x = x + 1; // signed integer overflow, UB
```

UBSan can report:

```text
runtime error: signed integer overflow
```

---

# 30. Combining sanitizers

Common development command:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -g -O1 \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    main.cpp -o program
```

This gives:

```text
warnings
debug info
reasonable sanitizer stack traces
ASan memory checks
UBSan undefined behavior checks
```

For serious C++ learning, this is a very good default.

---

# 31. What sanitizers cannot do

Sanitizers are powerful, but they do not prove your program is correct.

They detect bugs only on executed paths.

If a buggy branch is never executed, sanitizer will not see it.

Also, some UB cannot be detected reliably.

Example:

```cpp
int* p;
if (some_never_true_condition) {
    *p = 5;
}
```

If condition never runs, sanitizer cannot report it.

So sanitizers are:

```text
dynamic runtime checkers
not formal verification
```

---

# 32. Warnings are also important

Use compiler warnings:

```bash
g++ -Wall -Wextra -Wpedantic main.cpp
```

Common useful flags:

```bash
-Wall
-Wextra
-Wpedantic
-Wshadow
-Wconversion
-Wsign-conversion
```

Warnings catch many issues before runtime.

Example:

```cpp
int x = 3.14;
```

`-Wconversion` can warn about narrowing.

Recommended learning build:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -g -O0 main.cpp -o main
```

Sanitizer build:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -g -O1 \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    main.cpp -o main
```

---

# 33. Useful build flags summary

## Show internal commands

```bash
g++ -v main.cpp
```

## Preprocess only

```bash
g++ -E main.cpp -o main.ii
```

## Compile to assembly

```bash
g++ -S main.cpp -o main.s
```

## Compile to object file

```bash
g++ -c main.cpp -o main.o
```

## Link object file

```bash
g++ main.o -o program
```

## Debug info

```bash
-g
```

## Output filename

```bash
-o program
```

## Warnings

```bash
-Wall -Wextra -Wpedantic
```

## Sanitizers

```bash
-fsanitize=address
-fsanitize=leak
-fsanitize=undefined
```

---

# 34. How to inspect files

## Source

```bash
cat main.cpp
```

## Preprocessed output

```bash
less main.ii
```

## Assembly

```bash
less main.s
```

## Raw bytes

```bash
hd main.o
```

or:

```bash
hexdump -C main.o
```

## ELF info

```bash
readelf -h main.o
readelf -S main.o
readelf -s main.o
readelf -r main.o
```

## Symbols

```bash
nm -C main.o
```

## Disassembly

```bash
objdump -d -C -M intel main.o
objdump -d -C -M intel program
```

---

# 35. Mental model of the whole process

When we compile:

```bash
g++ main.cpp -o main
```

the pipeline is roughly:

```text
main.cpp
    ↓
preprocessor
    handles #include, #define, #if
    ↓
translation unit
    ↓
compiler frontend cc1plus
    parses C++, checks types, instantiates templates
    ↓
assembly file .s
    ↓
assembler as
    converts assembly into machine code
    ↓
object file .o
    contains code, data, symbols, relocations
    ↓
linker via collect2/ld
    resolves symbols, connects libraries, applies relocations
    ↓
executable file
```

---

# 36. Practical engineering lesson

The C++ build process is important because it explains many real errors:

```text
Header not found                 -> preprocessing/include path problem
Syntax/type error                -> compilation problem
Undefined reference              -> linking problem
Multiple definition              -> linking/ODR problem
Segmentation fault               -> runtime problem
ASan/UBSan report                -> runtime bug detected by instrumentation
```

Understanding build stages helps debug faster.

Instead of thinking:

```text
C++ compiler failed
```

we should ask:

```text
Which stage failed?
preprocessing?
compilation?
assembling?
linking?
runtime?
```

This is a core systems skill.
