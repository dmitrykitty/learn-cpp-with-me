# 1.3 Declarations, definitions, scopes and namespaces, usings

Status: C++ Basics

# 1. Declarations, definitions, and initialization

## Declaration

A **declaration** introduces a name to the compiler.

It tells the compiler:

```
“There exists something with this name and type.”
```

Examples:

```cpp
void f(int x);
void fun();
void ffun();

class C;
struct S;
union U;
```

Here:

```cpp
void f(int x);
```

means:

```
There is a function called f that takes int and returns void.
```

But the body is not provided yet, so this is only a declaration.

---

## Definition

A **definition** actually creates the entity or provides its full body.

Examples:

```cpp
int a = 5;

void fun() {
    std::cout << "Hello\n";
}

class C {
public:
    int x;
};
```

Important rule:

```
Every definition is also a declaration,
but not every declaration is a definition.
```

For example:

```cpp
void f(int x); // declaration only

void f(int x) { // definition
    std::cout << x << std::endl;
}
```

---

## Initialization

**Initialization** means giving an initial value to an object.

```cpp
int a = 5;
```

Here:

```
declaration: int a
definition: memory/storage for a is created
initialization: a gets value 5
```

Your original comment:

```cpp
int a = 5; // by default it's 0
```

needs correction.

If this variable is global:

```cpp
int a;
```

then it is zero-initialized by default.

But:

```cpp
int a = 5;
```

is explicitly initialized to `5`.

For local variables:

```cpp
void f() {
    int x; // not initialized, has indeterminate value
}
```

Local automatic variables are **not automatically zeroed**.

---

# 2. Global variables and static storage duration

Example:

```cpp
#include <iostream>

int a = 5; // global variable, definition, initialized with 5

int main() {
    std::cout << a << std::endl;
}
```

A global variable has **static storage duration**.

This means:

```
It exists for the entire lifetime of the program.
```

If a global variable is not explicitly initialized, it is zero-initialized:

```cpp
int x; // global variable, initialized to 0

int main() {
    std::cout << x << std::endl; // prints 0
}
```

But local variables are different:

```cpp
void f() {
    int x; // indeterminate value, dangerous to read
}
```

---

# 3. Namespaces

A **namespace** groups names and prevents name conflicts.

Example:

```cpp
namespace N {
    int a;
    int b;
}

namespace M {
    int c;
    int d;
}
```

Here:

```cpp
N::a
N::b
M::c
M::d
```

are different names, even if variable names inside namespaces are similar.

Variables inside namespaces also have static storage duration, so:

```cpp
namespace N {
    int a;
}
```

means `N::a` is zero-initialized.

---

## Accessing namespace members

You can access a namespace member with `::`, the **scope resolution operator**.

```cpp
#include <iostream>

namespace N {
    int a;
    int b;
}

int main() {
    N::b = 6;
    std::cout << N::b << std::endl;
}
```

`N::b` means:

```
Take b from namespace N.
```

---

# 4. `using` declaration

A **using declaration** brings one specific name into the current scope.

Example:

```cpp
#include <iostream>

namespace N {
    int a;
    int b;
}

int main() {
    using N::a;

    a = 10;

    std::cout << a << std::endl;
}
```

This:

```cpp
using N::a;
```

means:

```
Inside this scope, I can write a instead of N::a.
```

It brings only `a`, not the whole namespace.

So this would still require qualification:

```cpp
N::b = 6;
```

---

# 5. `using namespace`

A **using directive** makes names from the namespace available for unqualified lookup.

Example:

```cpp
#include <iostream>

namespace M {
    int c;
    int d;
}

int main() {
    using namespace M;

    c = 12;
    d = 13;

    std::cout << c << " " << d << std::endl;
}
```

This:

```cpp
using namespace M;
```

means:

```
When looking for a name, also search namespace M.
```

---

## Why `using namespace std;` is not recommended globally

This is not recommended:

```cpp
using namespace std;
```

especially in header files or global scope.

Because it can create name conflicts.

Example idea:

```cpp
#include <iostream>
#include <iterator>

using namespace std;

int distance = 10;

// std::distance also exists
```

Now names from `std` and your own global names can conflict or make code less clear.

Better:

```cpp
std::cout << "Hello" << std::endl;
```

or in a limited local scope:

```cpp
void print() {
    using std::cout;
    using std::endl;

    cout << "Hello" << endl;
}
```

---

# 6. Multiple `using` declarations since C++17

Since C++17, you can write:

```cpp
#include <iostream>

int main() {
    using std::cout, std::cin, std::endl;

    int x;
    cin >> x;
    cout << x << endl;
}
```

Before C++17, you would usually write:

```cpp
using std::cout;
using std::cin;
using std::endl;
```

---

# 7. Type aliases with `using`

`using` can also create a type alias.

Modern C++ style:

```cpp
#include <vector>

using vi = std::vector<int>;
```

Then:

```cpp
vi numbers;
```

means:

```cpp
std::vector<int> numbers;
```

Older C-style syntax:

```cpp
typedef std::vector<int> vi;
```

In modern C++, `using` is generally preferred because it is more readable and works better with templates.

Example:

```cpp
#include <vector>
#include <iostream>

using vi = std::vector<int>;

int main() {
    vi numbers = {1, 2, 3};

    for (int x : numbers) {
        std::cout << x << " ";
    }
}
```

---

# 8. Nested namespaces

Before C++17, nested namespaces were written like this:

```cpp
namespace A {
    namespace B {
        namespace C {
            int x = 10;
        }
    }
}
```

Since C++17, we can write:

```cpp
namespace A::B::C {
    int x = 10;
}
```

Usage:

```cpp
#include <iostream>

namespace A::B::C {
    int x = 10;
}

int main() {
    std::cout << A::B::C::x << std::endl;
}
```

This is just cleaner syntax.

---

# 9. Scope

A **scope** is a region of code where a name is visible.

Common scopes:

```
global scope
namespace scope
class scope
function scope
block scope
loop scope
```

Example from your code:

```cpp
#include <iostream>

int x = 0; // global scope

void fun() {
    int x = 1; // function/block scope

    for (int i = 0; i < 1; ++i) {
        int x = 2; // inner block scope

        std::cout << x << ' ' << ::x << std::endl;
    }
}

int main() {
    fun();
}
```

Output:

```
2 0
```

Why?

Inside the loop:

```cpp
std::cout << x;
```

uses the most local `x`.

So this:

```cpp
x
```

refers to:

```cpp
int x = 2;
```

But this:

```cpp
::x
```

means:

```
Take x from the global scope.
```

So:

```cpp
::x
```

refers to:

```cpp
int x = 0;
```

---

# 10. Name hiding / shadowing

When an inner scope has a variable with the same name as an outer scope, the inner one hides the outer one.

Example:

```cpp
int x = 0;

void f() {
    int x = 1;

    {
        int x = 2;
        std::cout << x << std::endl; // 2
    }

    std::cout << x << std::endl; // 1
}
```

This is called:

```
name hiding
```

or:

```
shadowing
```

Rule:

```
C++ usually chooses the most local visible name.
```

---

# 11. Global scope operator `::`

The operator:

```cpp
::
```

is called the **scope resolution operator**.

Used as:

```cpp
::x
```

means:

```
x from global namespace
```

Used as:

```cpp
N::x
```

means:

```
x from namespace N
```

Used as:

```cpp
ClassName::member
```

means:

```
member from class scope
```

Example:

```cpp
#include <iostream>

int value = 100;

namespace N {
    int value = 200;
}

int main() {
    int value = 300;

    std::cout << value << std::endl;    // 300, local
    std::cout << ::value << std::endl;  // 100, global
    std::cout << N::value << std::endl; // 200, namespace N
}
```

---

# 12. Important detail: `using namespace` and global lookup

`using namespace D;` does **not physically move** variables from namespace `D` into the global namespace.

However, it makes names from `D` visible during **name lookup** from the scope where the directive appears.

Example:

```cpp
#include <iostream>

namespace D {
    int r = 2;
}

using namespace D;

int main() {
    std::cout << r << std::endl;   // OK, finds D::r
    std::cout << ::r << std::endl; // Also OK, finds D::r through global using-directive
}
```

Output:

```
2
2
```

Important distinction:

```cpp
D::r
```

means:

```
Take r explicitly from namespace D.
```

But:

```cpp
::r
```

means:

```
Search for r from the global namespace scope.
```

Because `using namespace D;` was written in global scope, global lookup can also find `D::r`.

So practically this may work:

```cpp
std::cout << ::r << std::endl;
```

But conceptually, `r` is still a member of namespace `D`.

---

## Example with global variable conflict

```cpp
#include <iostream>

namespace D {
    int r = 2;
}

namespace E {
    int g = 1;
}

int g = 0;

using namespace D;
using namespace E;

void ffun() {
    std::cout << "r from namespace D: " << ::r << std::endl;
    std::cout << "global g: " << ::g << std::endl;
}

int main() {
    ffun();
}
```

Output:

```
r from namespace D: 2
global g: 0
```

Explanation:

```cpp
::r
```

can find `D::r`, because `using namespace D;` is placed in global scope.

But:

```cpp
::g
```

refers to the real global variable:

```cpp
int g = 0;
```

not to:

```cpp
E::g
```

because a direct global declaration has priority.

To access the namespace variable explicitly, write:

```cpp
E::g
```

---

## Ambiguity example

If two namespaces expose the same name, lookup becomes ambiguous.

```cpp
#include <iostream>

namespace D {
    int r = 2;
}

namespace F {
    int r = 3;
}

using namespace D;
using namespace F;

int main() {
    std::cout << ::r << std::endl; // error: reference to r is ambiguous
}
```

The compiler can find both:

```cpp
D::r
F::r
```

and cannot decide which one should be used.

Correct version:

```cpp
std::cout << D::r << std::endl;
std::cout << F::r << std::endl;
```

---

## Practical rule

```
using namespace D does not copy D::r into the global namespace.
It only makes D::r visible for name lookup.

If the using-directive is placed in global scope,
then ::r may find D::r.

However, D::r is clearer, safer, and better for production code.
```

---

# 13. Conflict with `using namespace`

Example:

```cpp
#include <iostream>

namespace NN1 {
    int num = 1;
}

namespace NN2 {
    int num = 2;
}

void ref_is_ambiguous_error() {
    using namespace NN1;
    using namespace NN2;

    std::cout << num; // error: reference to num is ambiguous
}
```

Why is this an error?

Because both namespaces provide a name called:

```cpp
num
```

So the compiler sees two possible candidates:

```cpp
NN1::num
NN2::num
```

and cannot decide which one you mean.

Correct versions:

```cpp
std::cout << NN1::num;
std::cout << NN2::num;
```

or:

```cpp
void ok() {
    using NN2::num;

    std::cout << num; // NN2::num
}
```

Full example:

```cpp
#include <iostream>

namespace NN1 {
    int num = 1;
}

namespace NN2 {
    int num = 2;
}

void ok() {
    using NN2::num;

    std::cout << num << std::endl; // 2
}

int main() {
    std::cout << NN1::num << std::endl; // 1
    std::cout << NN2::num << std::endl; // 2

    ok();
}
```

---

# 14. Declaration vs definition with `extern`

If you want to **declare** a variable but not define it, use `extern`.

Example in a header file:

```cpp
// config.hpp
#pragma once

extern int globalCounter;
```

This means:

```
globalCounter exists somewhere,
but it is not defined here.
```

Then in exactly one `.cpp` file:

```cpp
// config.cpp
#include "config.hpp"

int globalCounter = 0;
```

Then other files can use it:

```cpp
// main.cpp
#include <iostream>
#include "config.hpp"

int main() {
    globalCounter++;
    std::cout << globalCounter << std::endl;
}
```

Important:

```cpp
extern int globalCounter;
```

is a declaration.

```cpp
int globalCounter = 0;
```

is a definition.

---

# 15. ODR — One Definition Rule

ODR means **One Definition Rule**.

In simple words:

```
Some entities must have exactly one definition in the whole program.
```

For example, a non-inline global function with external linkage should have one definition:

```cpp
void f() {
    // body
}
```

If you define the same function in two `.cpp` files, the linker will usually complain.

---

## ODR and translation units

A **translation unit** is roughly:

```
one .cpp file after all #include files are pasted into it by the preprocessor
```

Example:

```cpp
#include "a.hpp"
#include "b.hpp"

int main() {
}
```

After preprocessing, all included header contents become part of this `.cpp` file. That full result is one translation unit.

---

## Why classes can be defined in headers

A class definition can appear in multiple translation units, usually through a header file, as long as the definition is exactly the same.

Example:

```cpp
// user.hpp
#pragma once

class User {
public:
    int id;
};
```

This header can be included in many `.cpp` files.

That is allowed because class definitions are allowed to appear in multiple translation units if they are identical.

But inside one translation unit, this is not allowed:

```cpp
class User {
public:
    int id;
};

class User {
public:
    int id;
}; // error: redefinition in the same translation unit
```

---

# 16. Function declarations and definitions

Example:

```cpp
#include <iostream>

void fun(); // declaration

int main() {
    fun();
}

void fun() { // definition
    std::cout << "fun called\n";
}
```

The compiler needs the declaration before the function is called.

The definition can be later.

This is why this works:

```cpp
void fun();

int main() {
    fun();
}

void fun() {
}
```

But this does not work:

```cpp
int main() {
    fun(); // compiler has not seen fun yet
}

void fun() {
}
```

---

# 17. Function overloading

Function overloading means having multiple functions with the same name but different parameter types.

Example:

```cpp
#include <iostream>

int f1(double x) {
    return x + 1;
}

int f1(int x) {
    return x + 2;
}

int main() {
    std::cout << f1(0.0) << std::endl;
}
```

Here:

```cpp
f1(0.0)
```

matches:

```cpp
int f1(double x)
```

because `0.0` is a `double`.

So it is a perfect match.

---

# 18. Promotions vs conversions

When C++ chooses overloaded function, it prefers better matches.

Simplified order:

```
1. Exact match
2. Promotion
3. Conversion
```

A **promotion** is a safe standard widening in common cases.

Examples:

```
float -> double
char -> int
short -> int
bool -> int
```

A **conversion** is more general and may be less preferred.

Examples:

```
double -> int
double -> float
int -> double
```

---

## Example with `f1`

```cpp
#include <iostream>

int f1(double x) {
    return x + 1;
}

int f1(int x) {
    return x + 2;
}

int main() {
    std::cout << f1(0.0) << std::endl;  // exact match with double
    std::cout << f1(0.0f) << std::endl; // float promotes to double
}
```

Explanation:

```cpp
f1(0.0)
```

`0.0` is `double`, so this is exact:

```cpp
f1(double)
```

```cpp
f1(0.0f)
```

`0.0f` is `float`.

There is no `f1(float)`, but `float -> double` is a promotion, so:

```cpp
f1(double)
```

is chosen.

---

# 19. Ambiguous overload example

Your example:

```cpp
#include <iostream>

int f2(float x) {
    return x + 1;
}

int f2(int x) {
    return x + 2;
}

int main() {
    std::cout << f2(0.0); // compilation error
}
```

Why?

```cpp
0.0
```

is a `double`.

Available overloads:

```cpp
f2(float)
f2(int)
```

The compiler can convert:

```
double -> float
double -> int
```

Both are standard conversions.

Neither is clearly better.

So the call is ambiguous:

```
error: call to overloaded function is ambiguous
```

Fixes:

```cpp
f2(0.0f);           // calls f2(float)
f2(0);              // calls f2(int)
f2(static_cast<float>(0.0)); // calls f2(float)
f2(static_cast<int>(0.0));   // calls f2(int)
```

Corrected example:

```cpp
#include <iostream>

int f2(float x) {
    return x + 1;
}

int f2(int x) {
    return x + 2;
}

int main() {
    std::cout << f2(0.0f) << std::endl; // calls f2(float)
    std::cout << f2(0) << std::endl;    // calls f2(int)
}
```

---

# 20. Full cleaned example: namespaces and scopes

This is a corrected version of your namespace/scope examples:

```cpp
#include <iostream>

namespace N {
    int a;
    int b;
}

namespace M {
    int c;
    int d;
}

int x = 0;

void fun() {
    int x = 1;

    for (int i = 0; i < 1; ++i) {
        int x = 2;
        std::cout << x << ' ' << ::x << std::endl;
    }
}

namespace D {
    int r = 2;
}

namespace E {
    int g = 1;
}

int g = 0;

void ffun() {
    std::cout << "r from namespace D: " << ::r << std::endl;
    std::cout << "g from global scope: " << ::g << std::endl;
    std::cout << "g from namespace E: " << E::g << std::endl;
}

int main() {
    N::b = 6;

    using N::a;
    a = 10;

    std::cout << a << std::endl;

    using namespace M;
    c = 12;
    d = 13;

    std::cout << c << " " << d << std::endl;

    fun();
    ffun();
}
```

---

# 21. Interview-style summary

A **declaration** introduces a name and type to the compiler, while a **definition** actually creates the entity or provides the function/class body. **Initialization** means assigning the initial value to an object.

A **scope** defines where a name is visible. If multiple variables with the same name exist in nested scopes, C++ usually uses the most local one. The global variable can be accessed with `::name`.

A **namespace** groups names and prevents conflicts. We can access namespace members using `N::x`. A `using` declaration brings one specific name into scope, while `using namespace` makes all names from a namespace available for lookup, which can cause ambiguity.

The **One Definition Rule** says that some entities, such as ordinary functions and global variables with external linkage, must have exactly one definition in the whole program. Class definitions can appear in multiple translation units if they are identical, which is why classes are usually defined in header files.

For overloaded functions, C++ selects the best match. Exact matches are preferred over promotions, and promotions are preferred over general conversions. If two conversions are equally good, the call becomes ambiguous.