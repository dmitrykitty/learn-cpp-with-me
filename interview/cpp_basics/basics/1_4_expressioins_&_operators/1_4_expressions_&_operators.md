# Expressions and Statements in C++

## 1. Elements of a C++ program

A C++ program is made from several syntactic elements:

```text id="qlrz4d"
tokens
expressions
statements
declarations
definitions
translation units
```

At the top level of a file, meaning at **namespace/global scope**, we mostly write **declarations**.

A declaration may or may not be a definition.

Examples:

```cpp id="x3idqw"
int x = 10;        // declaration + definition + initialization
extern int y;      // declaration only
void f();          // function declaration
void g() {}        // function declaration + definition
class User;        // class declaration
class Account {};  // class declaration + definition
```

At global/namespace scope, we cannot write arbitrary expression statements like this:

```cpp id="02n8x3"
int x = 10;

x + 5;        // error at global scope
std::cout << x; // error at global scope
```

These are allowed inside a function:

```cpp id="hhl0wu"
#include <iostream>

int x = 10;

int main() {
    x + 5;              // expression statement, allowed but useless
    std::cout << x;     // expression statement with side effect
}
```

So a simplified rule is:

```text id="w2ug31"
At global/namespace scope:
    declarations are allowed.

Inside function/local scope:
    declarations, expressions, and control statements are allowed.
```

More precise version:

```text id="20krmc"
Namespace scope accepts declarations.
Block scope accepts statements.
Some statements are declaration statements.
```

---

# 2. Are declarations, expressions, statements, etc. C++ grammar parts?

Yes, but they are not all “tokens”.

C++ source code is first split into **tokens**.

Examples of tokens:

```text id="s8yz6q"
keywords:       int, if, return, namespace
identifiers:    x, value, Solution
literals:       10, 3.14, "hello"
operators:      +, -, *, =, ==, &&
punctuation:    ;, {, }, (, ), ,
```

Then the compiler parses tokens according to the C++ grammar.

Grammar categories include:

```text id="oagvu5"
declaration
statement
expression
type-specifier
function-definition
namespace-definition
compound-statement
selection-statement
iteration-statement
```

So:

```cpp id="fwno6x"
x + 5;
```

is made from tokens:

```text id="udb1e2"
identifier x
operator +
literal 5
semicolon ;
```

The expression is:

```cpp id="63jmc7"
x + 5
```

The full statement is:

```cpp id="oochxz"
x + 5;
```

Important distinction:

```text id="eybgmi"
x + 5       -> expression
x + 5;      -> expression statement
;           -> part of the statement, not part of the expression itself
```

---

# 3. Statements in C++

A **statement** is an instruction executed by the program.

Common groups of statements:

```text id="5dflyw"
declaration statements
expression statements
compound statements
selection statements
iteration statements
jump statements
try blocks
```

Examples:

```cpp id="jkk6x6"
int x = 10;       // declaration statement

x + 5;            // expression statement

{
    int y = 20;   // compound statement / block
}

if (x > 0) {      // selection statement
}

for (int i = 0; i < 10; ++i) { // iteration statement
}

return 0;         // jump statement
```

---

# 4. Expressions

An **expression** is a piece of code that usually computes a value and may produce side effects.

Examples:

```cpp id="xqzw8y"
x + 5
std::cout << x
x = 10
++x
a && b
condition ? x : y
```

An expression can have:

```text id="3wpp9r"
a type
a value category
side effects
```

For example:

```cpp id="d9loas"
x + 5
```

has a value, but no side effect.

This:

```cpp id="ojlsdj"
x = 10
```

has a side effect because it changes `x`.

This:

```cpp id="np9uqs"
std::cout << x
```

has a side effect because it writes to output.

---

# 5. Expression statement

An **expression statement** is an expression followed by `;`.

```cpp id="31mr2n"
x + 5;
std::cout << x;
x = 10;
++x;
```

The semicolon ends the statement.

Important:

```cpp id="xkbagx"
x + 5;
```

is valid C++, but usually useless because the result is discarded.

But:

```cpp id="g2p639"
std::cout << x;
```

is useful because it has a side effect.

---

# 6. Value categories: lvalue and rvalue

This is very important for operators.

Simplified version:

```text id="243wu7"
lvalue  -> has identity, usually can appear on the left side of assignment
rvalue  -> temporary value, usually cannot be assigned to
```

Example:

```cpp id="mbqee9"
int x = 10;

x = 20;      // x is lvalue
10 = x;      // error, 10 is rvalue
```

Expression:

```cpp id="bbk6tz"
x
```

is an lvalue.

Expression:

```cpp id="e4a5oj"
x + 1
```

is an rvalue/prvalue.

Expression:

```cpp id="7oxlj2"
(x = 5)
```

is an lvalue, because assignment returns the left operand.

This is why this is legal:

```cpp id="2gtiuz"
int x;
int y = 10;

(x = y) = 20;
```

After `x = y`, the expression refers to `x`, so we can assign to it again.

---

# 7. Binary operators that usually return rvalues

Most built-in arithmetic, comparison, bitwise, and logical operators return temporary values.

Examples:

```cpp id="101yib"
a + b
a - b
a * b
a / b
a % b

a < b
a <= b
a > b
a >= b
a == b
a != b

a & b
a | b
a ^ b

a && b
a || b
```

Usually these produce rvalues/prvalues.

Example:

```cpp id="mmjtf5"
int a = 10;
int b = 20;

(a + b) = 5; // error, a + b is not an lvalue
```

Comparison operators return `bool`:

```cpp id="r07db3"
int a = 10;
int b = 20;

bool result = a < b; // true
```

---

# 8. Bitwise operators

Bitwise operators work directly on bits.

```cpp id="qac66x"
&   bitwise AND
|   bitwise OR
^   bitwise XOR
~   bitwise NOT
<<  left shift
>>  right shift
```

Example:

```cpp id="5nhvc4"
#include <iostream>

int main() {
    unsigned int a = 0b1100;
    unsigned int b = 0b1010;

    std::cout << (a & b) << std::endl; // 8  -> 1000
    std::cout << (a | b) << std::endl; // 14 -> 1110
    std::cout << (a ^ b) << std::endl; // 6  -> 0110
}
```

Shifts:

```cpp id="zrpqec"
int x = 1;

x << 3; // 8, because 0001 becomes 1000
x >> 1; // divide by 2 for many positive integers
```

Important:

```cpp id="3dgdgs"
x << n
```

is roughly multiplication by `2^n` for non-overflowing unsigned/integer cases.

```cpp id="7dmq7q"
x >> n
```

is roughly division by `2^n`, but be careful with signed negative numbers.

---

# 9. Fast inverse square root and bit-level tricks

The famous fast inverse square root computes approximately:

```text id="ltztjm"
1 / sqrt(x)
```

It became famous from Quake III.

The idea was roughly:

```text id="nttlbm"
1. Treat float bits as an integer.
2. Manipulate exponent/mantissa bits with a magic constant.
3. Convert bits back to float.
4. Improve result with Newton-Raphson iteration.
```

Very simplified idea:

```cpp id="83migo"
float fastInvSqrt(float x) {
    float xhalf = 0.5f * x;

    int i = *(int*)&x;              // old unsafe type punning style
    i = 0x5f3759df - (i >> 1);      // magic bit manipulation
    x = *(float*)&i;

    x = x * (1.5f - xhalf * x * x); // Newton-Raphson step

    return x;
}
```

Important modern note:

```text id="2oyrbq"
This is historically interesting, but not usually recommended in modern production C++.
```

Reasons:

```text id="azbfmg"
It uses unsafe type punning in the old form.
Modern CPUs often have fast sqrt/rsqrt instructions.
Compilers can optimize math heavily.
The approximation may not be accurate enough.
```

Modern safer bit manipulation would use `std::bit_cast` since C++20:

```cpp id="0ww2l5"
#include <bit>
#include <cstdint>

float f = 1.0f;
std::uint32_t bits = std::bit_cast<std::uint32_t>(f);
```

This topic is useful because it shows that floating-point numbers are just bits:

```text id="c73aer"
sign bit + exponent bits + mantissa bits
```

Bit tricks exploit this representation.

---

# 10. Logical operators `&&` and `||`

Logical operators use **short-circuit evaluation**.

For `&&`:

```cpp id="3o3x7w"
if (ptr != nullptr && ptr->value > 10) {
}
```

If:

```cpp id="5g8k1y"
ptr != nullptr
```

is false, then:

```cpp id="5xhlg9"
ptr->value > 10
```

is not evaluated.

This prevents dereferencing `nullptr`.

For `||`:

```cpp id="tma2m1"
if (x == 0 || expensiveCheck()) {
}
```

If:

```cpp id="rl83aa"
x == 0
```

is true, then:

```cpp id="a8sq2h"
expensiveCheck()
```

is not evaluated.

This is called:

```text id="wkjzo1"
lazy evaluation
```

or:

```text id="pzg038"
short-circuit evaluation
```

---

# 11. Short-circuiting and branch prediction

This part is important for performance-sensitive C++.

A statement like:

```cpp id="zpsifm"
if (x > 0 && y > 0) {
    doSomething();
}
```

usually becomes machine code with branches.

A branch is an instruction where the CPU must choose which path to execute:

```text id="7a0iyz"
condition true  -> jump here
condition false -> continue there
```

Modern CPUs are pipelined. They execute many instructions in overlapping stages.

The problem:

```text id="724wqd"
The CPU often reaches a branch before it knows the final condition result.
```

Waiting would waste cycles, so the CPU guesses.

This is called:

```text id="tt8dzx"
branch prediction
```

If the CPU guesses correctly, performance is good.

If it guesses incorrectly, the CPU has executed instructions from the wrong path. Then it must throw away speculative work and restart from the correct path.

This is called:

```text id="4fceo8"
branch misprediction
```

Misprediction can cost many CPU cycles.

In low-latency/HFT-style code, this matters because it creates:

```text id="z52pp1"
higher latency
latency spikes
less predictable performance
pipeline flushes
wasted speculative work
```

---

## Does the processor compute before the compiler generates code?

No.

The compiler first generates machine code.

The CPU executes that machine code at runtime.

But during runtime, the CPU may execute instructions **speculatively** before it knows whether they are definitely needed.

So the correct idea is:

```text id="j8gxrr"
The CPU does not compute before compilation.
The CPU speculatively executes predicted runtime paths before branch conditions are fully resolved.
```

Example:

```cpp id="j0nx80"
if (likelyCondition) {
    fastPath();
} else {
    slowPath();
}
```

At runtime, the CPU may predict that `likelyCondition` is true and start executing `fastPath()` before the condition is fully resolved.

If the prediction was right, great.

If wrong, speculative work is discarded.

---

## Why unpredictable branches are bad

Example:

```cpp id="i9t2tk"
if (randomValue % 2 == 0) {
    x += 1;
} else {
    x -= 1;
}
```

If the condition is random, the branch predictor has a hard time.

A predictable branch:

```cpp id="xhkdde"
if (almostAlwaysTrue) {
    fastPath();
}
```

is easier.

A random branch:

```cpp id="r75min"
if (randomCondition) {
    pathA();
} else {
    pathB();
}
```

is harder.

For performance-sensitive code, developers sometimes use branchless style:

```cpp id="p89jm0"
int add = condition ? 1 : 0;
x += add;
```

or:

```cpp id="m3n75b"
x += (value > threshold);
```

But branchless code is not always faster. It depends on:

```text id="gyehiz"
CPU architecture
compiler optimization
data distribution
branch predictability
cost of both paths
```

Important interview/performance summary:

```text id="v4eg41"
Branches are not always bad.
Unpredictable branches are bad.
Predictable branches are usually cheap.
```

---

# 12. Assignment operators return lvalues

Assignment operators:

```cpp id="rxyiyy"
=
+=
-=
*=
/=
%=
&=
|=
^=
<<=
>>=
```

return the left operand as an lvalue.

Example:

```cpp id="kr5prl"
int x = 1;
int y = 2;
int z = 3;

(x = y) = z;
```

Step by step:

```text id="2o8ddj"
x = y    -> assigns 2 to x and returns x as lvalue
(x = y) = z -> assigns 3 to x
```

Final value:

```cpp id="bd2bq2"
x == 3
```

Compound assignment also returns lvalue:

```cpp id="icp1zt"
int x = 10;

(x += 5) += 2;

std::cout << x << std::endl; // 17
```

This works because:

```cpp id="cr4l60"
x += 5
```

returns `x`.

---

# 13. Prefix and postfix increment/decrement

Operators:

```cpp id="wcb93k"
++a
--a
a++
a--
```

## Prefix increment

```cpp id="vuts7o"
++a
```

means:

```text id="418mx3"
increment first, then return the updated object
```

For built-in types, prefix `++a` returns an lvalue.

Example:

```cpp id="u9ho5r"
int a = 1;

++a = 10;

std::cout << a << std::endl; // 10
```

This is possible because `++a` refers to `a`.

Also:

```cpp id="h7y9ri"
++++a;
```

is parsed as:

```cpp id="d02h67"
++(++a);
```

This is valid for built-in integers because `++a` returns lvalue.

---

## Postfix increment

```cpp id="rpt31e"
a++
```

means:

```text id="z5ga4g"
return old value, then increment
```

For built-in types, postfix `a++` returns a prvalue/rvalue.

Example:

```cpp id="2bnixl"
int a = 1;

a++ = 10; // error
```

Why?

Because `a++` returns the old value as a temporary.

---

## Example

```cpp id="cdmse0"
#include <iostream>

int main() {
    int a = 1;

    std::cout << ++a << std::endl; // 2, returns new value
    std::cout << a++ << std::endl; // 2, returns old value
    std::cout << a << std::endl;   // 3
}
```

---

# 14. Greedy parsing: `a+++++b`

C++ lexer uses the “maximal munch” rule.

It tries to make the longest possible token.

Expression:

```cpp id="b424pq"
a+++++b
```

is not parsed as:

```cpp id="m0bmx3"
a++ + ++b
```

It is tokenized greedily, approximately like:

```text id="byn72f"
a ++ ++ + b
```

which leads to an error.

Better write explicitly:

```cpp id="o9de8n"
a++ + ++b
```

Example:

```cpp id="u68mxq"
int a = 1;
int b = 2;

int c = a++ + ++b;
```

Do not write clever code like this in production.

---

# 15. Conditional operator `?:`

The conditional operator has three operands:

```cpp id="vegj7r"
condition ? expr1 : expr2
```

Example:

```cpp id="8m76fz"
int maxValue = a > b ? a : b;
```

It is also lazy:

```cpp id="hewd41"
condition ? expr1 : expr2
```

If condition is true:

```text id="8fhtzq"
expr1 is evaluated
expr2 is not evaluated
```

If condition is false:

```text id="e46i1h"
expr2 is evaluated
expr1 is not evaluated
```

Example:

```cpp id="s2zh95"
int x = 10;
int y = 0;

int result = y != 0 ? x / y : 0; // safe
```

If `y == 0`, then `x / y` is not evaluated.

---

# 16. Is `?:` lvalue or rvalue?

It depends on the second and third operands.

If both branches are lvalues of compatible/same type, the result can be an lvalue.

Example:

```cpp id="w7obg9"
int a = 1;
int b = 2;

(true ? a : b) = 10;

std::cout << a << std::endl; // 10
```

Here:

```cpp id="p3q0a5"
a
b
```

are both lvalues of type `int`.

So:

```cpp id="lqcgic"
true ? a : b
```

is also an lvalue.

But if one branch is an rvalue, the result is usually an rvalue/prvalue.

Example:

```cpp id="qqqh32"
int a = 1;
int b = 2;

(true ? ++a : b) = 10; // OK, both branches are lvalues
```

Because:

```cpp id="39jvw9"
++a
```

returns lvalue, and:

```cpp id="d9hym4"
b
```

is lvalue.

But:

```cpp id="74py52"
(true ? a++ : b) = 10; // error
```

Because:

```cpp id="y22pdo"
a++
```

returns rvalue/prvalue.

Another example:

```cpp id="dwrku2"
int a = 1;
int b = 2;

(true ? a : b++) = 10; // error
```

because `b++` returns rvalue.

Important simplified rule:

```text id="feub6e"
If both second and third operands are lvalues of the same type,
the conditional expression can be an lvalue.

Otherwise it often becomes an rvalue after conversions.
```

The full C++ rule is more complex, especially with classes, references, conversions, and `void`.

---

# 17. Conditional operator and type conversion

The two branches must produce compatible types, or one must be convertible to the other/common type.

Example:

```cpp id="3e3687"
auto x = true ? 1 : 2.5;
```

Here:

```text id="ydjk82"
1     -> int
2.5   -> double
```

The common type is `double`.

So:

```cpp id="r94yye"
x
```

becomes `double`.

Example:

```cpp id="lmd4rr"
#include <iostream>
#include <string>

int main() {
    bool flag = true;

    auto x = flag ? 10 : 20.5;

    std::cout << x << std::endl;
}
```

But this is problematic:

```cpp id="sbjf66"
auto x = true ? 10 : "hello"; // error
```

because `int` and string literal do not have a reasonable common type for this expression.

---

# 18. Comma operator

The comma operator evaluates the left expression, discards its result, then evaluates and returns the right expression.

```cpp id="bzqzay"
(a, b)
```

means:

```text id="6ujsfe"
evaluate a
discard result of a
evaluate b
return b
```

Example:

```cpp id="dyhhgg"
#include <iostream>

int main() {
    int a = 1;
    int b = 2;

    int x = (a += 10, b += 20);

    std::cout << a << std::endl; // 11
    std::cout << b << std::endl; // 22
    std::cout << x << std::endl; // 22
}
```

Important:

```cpp id="0om9ii"
(a, b) = 100;
```

can be valid if `b` is an lvalue.

Example:

```cpp id="lv4soz"
int a = 1;
int b = 2;

(a, b) = 100;

std::cout << b << std::endl; // 100
```

Why?

Because the built-in comma operator returns the right operand with its value category.

Here, `b` is lvalue, so `(a, b)` is lvalue.

But:

```cpp id="jpqzcb"
(a, b + 1) = 100; // error
```

because `b + 1` is rvalue.

---

# 19. `sizeof`

`sizeof` is an operator, not a normal function.

It returns the size in bytes as `std::size_t`.

Examples:

```cpp id="lvn8pp"
sizeof(int)
sizeof(double)
sizeof(x)
sizeof(std::vector<int>)
```

Example:

```cpp id="5h1qyt"
#include <iostream>
#include <vector>

int main() {
    int x = 10;
    std::vector<int> v = {1, 2, 3};

    std::cout << sizeof(x) << std::endl;
    std::cout << sizeof(v) << std::endl;
}
```

Important:

```cpp id="ldo2c4"
sizeof(v)
```

for `std::vector<int>` does **not** return the size of all elements.

It returns the size of the vector object itself.

Usually a vector object contains something like:

```text id="yzedyp"
pointer to beginning
pointer to end
pointer to capacity end
```

On a 64-bit system, this is often:

```text id="uqp8fk"
3 pointers * 8 bytes = 24 bytes
```

So:

```cpp id="vw2j9o"
sizeof(std::vector<int>)
```

is often `24`, but this is implementation-dependent.

The heap-allocated elements are separate.

Example:

```cpp id="kk95r7"
std::vector<int> v(1000);

std::cout << sizeof(v) << std::endl; // maybe 24, not 4000
```

---

# 20. `sizeof` and arrays

For raw arrays, `sizeof` gives the total size of the array in bytes.

```cpp id="o8d33k"
#include <iostream>

int main() {
    int arr[5];

    std::cout << sizeof(arr) << std::endl;
    std::cout << sizeof(arr[0]) << std::endl;
    std::cout << sizeof(arr) / sizeof(arr[0]) << std::endl;
}
```

If `int` is 4 bytes:

```text id="84gcus"
sizeof(arr)     = 20
sizeof(arr[0])  = 4
array length    = 5
```

But be careful with function parameters.

```cpp id="6wa1x0"
#include <iostream>

void f(int arr[]) {
    std::cout << sizeof(arr) << std::endl; // size of pointer, not array
}

int main() {
    int arr[5];

    std::cout << sizeof(arr) << std::endl; // full array size
    f(arr);
}
```

In function parameters:

```cpp id="453dih"
int arr[]
```

decays to:

```cpp id="crw8v6"
int*
```

So `sizeof(arr)` inside `f` gives pointer size.

---

# 21. Is `sizeof` `constexpr`?

`sizeof` produces a compile-time constant in normal standard C++.

Example:

```cpp id="sb8hsk"
int arr[sizeof(int) * 2];
```

This works because `sizeof(int)` is known at compile time.

`sizeof` is evaluated in an **unevaluated context**.

Example:

```cpp id="fh36k8"
int f() {
    std::cout << "called\n";
    return 10;
}

int main() {
    std::cout << sizeof(f()) << std::endl;
}
```

`f()` is not actually called.

`sizeof(f())` asks:

```text id="jgl9tq"
What is the size of the type returned by f()?
```

Since `f()` returns `int`, this is:

```cpp id="01nza4"
sizeof(int)
```

Important distinction:

```text id="nv9oss"
constexpr expression:
    expression that can be evaluated at compile time

constexpr keyword:
    tells the compiler that a variable/function can be used in constant expressions
```

Example:

```cpp id="g0zz0k"
constexpr int n = sizeof(int);

int arr[n];
```

---

# 22. Floating-point associativity problem

For real mathematical numbers:

```text id="ajg7i5"
(a + b) + c == a + (b + c)
```

But for floating-point numbers:

```text id="nf6a7v"
(a + b) + c may not equal a + (b + c)
```

Why?

Because floating-point numbers have limited mantissa precision.

A floating-point number is stored roughly as:

```text id="to0737"
sign + exponent + mantissa
```

When adding two floats/doubles, the CPU must align exponents.

Example:

```text id="ifksjs"
large number:  1.000000000000000 * 2^100
small number:  1.000000000000000 * 2^0
```

To add them, the small number must be shifted so exponents match.

During this shift, small bits may fall out of the mantissa and disappear.

So the result is rounded after the operation.

---

## Example

```cpp id="ie4whn"
#include <iostream>
#include <iomanip>

int main() {
    double a = 1e20;
    double b = -1e20;
    double c = 3.14;

    std::cout << std::setprecision(17);

    std::cout << ((a + b) + c) << std::endl;
    std::cout << (a + (b + c)) << std::endl;
}
```

Conceptually:

```text id="9bq7fq"
(a + b) + c
= (1e20 - 1e20) + 3.14
= 0 + 3.14
= 3.14
```

But:

```text id="k8mhtu"
a + (b + c)
= 1e20 + (-1e20 + 3.14)
```

When adding `3.14` to `-1e20`, the `3.14` may be too small to affect the large number, so:

```text id="86f3eu"
-1e20 + 3.14 ≈ -1e20
```

Then:

```text id="yx5s4e"
1e20 + (-1e20) = 0
```

So you can get different results.

Important:

```text id="46lmg6"
Floating-point addition is not associative because each operation may round the result.
```

This matters in:

```text id="8j2gwe"
numerical algorithms
finance
statistics
physics engines
trading systems
parallel reductions
```

Parallel code can produce different floating-point sums because values are grouped differently.

---

# 23. Left-associative operators

Associativity describes how operators with the same precedence are grouped during parsing.

Example:

```cpp id="vzqlku"
a - b - c
```

Subtraction is left-associative, so the compiler parses it as:

```cpp id="60ogul"
(a - b) - c
```

not:

```cpp id="3h5x58"
a - (b - c)
```

Example:

```cpp id="9df0xe"
int result = 10 - 3 - 2;
```

Parsed as:

```cpp id="fwwxdr"
(10 - 3) - 2 = 5
```

If it were right-associative:

```cpp id="1j8lsd"
10 - (3 - 2) = 9
```

So associativity affects parsing.

---

## Associativity is not the same as evaluation order

This is very important in C++.

Example:

```cpp id="fj2v3m"
a() + b() + c()
```

Because `+` is left-associative, it is parsed as:

```cpp id="c9s8ex"
(a() + b()) + c()
```

But this does not necessarily mean that `a()` is called before `b()` and `b()` before `c()`.

Associativity controls grouping, not always runtime evaluation order.

Better interview sentence:

```text id="wb5m74"
Operator associativity tells the compiler how to group operators of the same precedence.
It does not generally guarantee the order in which subexpressions are evaluated.
```

---

# 24. Examples of associativity

Most arithmetic operators are left-associative:

```cpp id="xejtqd"
a + b + c   // (a + b) + c
a * b * c   // (a * b) * c
a / b / c   // (a / b) / c
a - b - c   // (a - b) - c
```

Assignment is right-associative:

```cpp id="qv2vho"
a = b = c;
```

Parsed as:

```cpp id="f39a5w"
a = (b = c);
```

This works because assignment returns the left operand as lvalue.

Example:

```cpp id="z7y6rg"
int a;
int b;
int c = 10;

a = b = c;
```

Step by step:

```text id="7n5184"
b = c -> b becomes 10, expression returns b
a = b -> a becomes 10
```

---

# 25. Spaces do not always help parsing

C++ tokenization is greedy.

For example:

```cpp id="f5cbyb"
a+++++b
```

is a bad idea.

Write explicitly:

```cpp id="f8gifn"
a++ + ++b
```

Or better, split into readable statements:

```cpp id="wjs7ho"
int oldA = a;
++a;
++b;
int result = oldA + b;
```

In interview and production code, readability matters more than showing off parser tricks.

---

# 26. Practical production advice

For normal production C++:

```text id="65s23s"
Prefer simple expressions.
Avoid clever increment tricks.
Avoid global using namespace.
Be explicit with namespaces.
Do not rely on unspecified evaluation order.
Be careful with floating-point equality.
Prefer readable conditionals over over-optimized branchless tricks unless profiling proves a need.
```

Good:

```cpp id="lafqjz"
if (ptr != nullptr && ptr->isActive()) {
    process(*ptr);
}
```

Bad:

```cpp id="3mvr4l"
ptr && ptr->isActive() ? process(*ptr) : void();
```

Good:

```cpp id="3a6mek"
double result = computePrice(order);
```

Dangerous:

```cpp id="hhp8z3"
if (price == expectedPrice) {
}
```

Better for floating-point comparison:

```cpp id="hux3w1"
#include <cmath>

bool almostEqual(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) < eps;
}
```

---

# 27. Interview-style summary

An **expression** is a piece of code that computes a value and may have side effects. An **expression statement** is an expression followed by a semicolon. Statements are executable instructions, such as expression statements, declaration statements, loops, conditionals, and return statements.

At namespace/global scope, C++ mostly allows declarations, not arbitrary expression statements. Inside function scope, we can write declarations, expression statements, and control statements.

Most arithmetic and comparison operators return temporary rvalues. Assignment and compound assignment operators return the left operand as an lvalue, which is why expressions like `(x = y) = z` are legal. Prefix increment returns an lvalue for built-in types, while postfix increment returns the old value as a temporary.

Logical operators `&&` and `||` use short-circuit evaluation. This is useful for safety and performance, but it can also introduce branches. Modern CPUs use branch prediction and speculative execution to avoid waiting for branch results. Predictable branches are usually cheap, while unpredictable branches can cause pipeline flushes and latency spikes.

The conditional operator `?:` evaluates only one selected branch. Its value category depends on the second and third operands. If both are lvalues of the same type, the whole conditional expression can be an lvalue. Otherwise, it often becomes an rvalue after conversions.

`sizeof` is a compile-time operator that returns the size of a type or object in bytes as `std::size_t`. For raw arrays, it gives the total array size, but for `std::vector`, it gives the size of the vector object itself, not the heap-allocated elements.

Floating-point addition is not associative because every operation may require exponent alignment and rounding. Therefore, `(a + b) + c` may differ from `a + (b + c)`.

Operator associativity defines how expressions are parsed, for example `a - b - c` is parsed as `(a - b) - c`. However, associativity is not the same as runtime evaluation order.
