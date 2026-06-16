# Control Statements in C++

Control statements change the normal top-to-bottom execution flow of a program.

Main groups:

```text id="97ad73"
if / else
switch
while
do while
for
break
continue
goto
```

---

# 1. `if / else if / else`

Basic form:

```cpp id="kjp0ea"
if (condition) {
    // executed if condition is true
} else if (anotherCondition) {
    // executed if first condition is false and this one is true
} else {
    // executed if all previous conditions are false
}
```

Example from code:

```cpp id="f3w3lu"
if (true) {

} else if (true) {

} else {

}
```

The condition must be convertible to `bool`.

Examples:

```cpp id="7v2euj"
int x = 5;

if (x > 0) {
    std::cout << "positive";
}

if (x) {
    std::cout << "non-zero";
}
```

In C++, numeric values can be used in conditions:

```text id="2e1j5e"
0     -> false
non-0 -> true
```

---

## `if` with initializer since C++17

Since C++17, `if` can have an initializer:

```cpp id="6lv1yv"
if (int x = getValue(); x > 0) {
    std::cout << x;
}
```

This means:

```text id="cyv53e"
1. Create x.
2. Check condition x > 0.
3. x exists only inside the if / else chain.
```

Example:

```cpp id="gnz8z7"
if (int value = 10; value > 5) {
    std::cout << value;
} else {
    std::cout << "small";
}
```

This is useful when a temporary variable is needed only for the condition.

---

# 2. `switch`

`switch` is used when we compare one value against many constant cases.

Basic form:

```cpp id="8o986l"
switch (expression) {
case value1:
    // code
    break;

case value2:
    // code
    break;

default:
    // code if no case matched
}
```

Example from code:

```cpp id="owwmvz"
int x = 5;

switch (x) {
case 1:
    std::cout << 1;
    break;

case 2:
    std::cout << 2;
    break;

default:
    std::cout << x;
}
```

Output:

```text id="y4k4k9"
5
```

because `x` is not `1` or `2`, so `default` runs.

---

## What types can be used in `switch`?

The switch expression should be an integral-like type:

```text id="sw0jbj"
int
char
short
long
long long
enum
bool
types convertible to integral or enum
```

Examples:

```cpp id="7d2d9v"
char command = 'a';

switch (command) {
case 'a':
    std::cout << "add";
    break;
case 'd':
    std::cout << "delete";
    break;
}
```

Not allowed directly:

```cpp id="99n10v"
std::string s = "hello";

switch (s) { // error
}
```

For strings, use `if / else` or a map.

---

## Why `switch` can be faster than many `if`s

Sometimes compilers can optimize `switch` very well.

For example, for dense integer cases:

```cpp id="e5iwqd"
switch (x) {
case 1: ...
case 2: ...
case 3: ...
case 4: ...
}
```

the compiler may generate a **jump table**.

A jump table is roughly:

```text id="mex4tl"
Use x as an index to jump directly to the correct code block.
```

This can be faster than checking:

```cpp id="q7kxw8"
if (x == 1) ...
else if (x == 2) ...
else if (x == 3) ...
```

But it is not guaranteed. The compiler chooses optimization depending on:

```text id="3l69ti"
number of cases
density of case values
target architecture
optimization level
code size trade-offs
```

Good practical rule:

```text id="gajwt6"
Use switch when one integral value is compared against many constant cases.
Use if / else when conditions are complex or not based on one value.
```

---

## `break` in `switch`

`break` exits the switch.

```cpp id="bwegrz"
switch (x) {
case 1:
    std::cout << "one";
    break;

case 2:
    std::cout << "two";
    break;
}
```

Without `break`, execution continues into the next case. This is called **fallthrough**.

Example:

```cpp id="boqcw3"
switch (x) {
case 1:
    std::cout << "one";

case 2:
    std::cout << "two";
}
```

If `x == 1`, this prints:

```text id="v53w01"
onetwo
```

In modern C++, intentional fallthrough should be marked:

```cpp id="h1chpz"
switch (x) {
case 1:
    std::cout << "one";
    [[fallthrough]];

case 2:
    std::cout << "two";
    break;
}
```

---

# 3. `while`

`while` checks the condition before each iteration.

```cpp id="btqmxw"
while (condition) {
    // loop body
}
```

Example:

```cpp id="3aa30w"
int i = 0;

while (i < 5) {
    std::cout << i << " ";
    ++i;
}
```

Output:

```text id="vi5tw4"
0 1 2 3 4
```

From your code:

```cpp id="kbeygx"
while (true) {
}
```

This is an infinite loop.

It runs forever unless:

```text id="t6sa02"
break is executed
return is executed
exception is thrown
program is terminated externally
```

Example with `break`:

```cpp id="p639th"
int x = 0;

while (true) {
    ++x;

    if (x == 5) {
        break;
    }
}
```

---

# 4. `do while`

`do while` checks the condition after the loop body.

```cpp id="nsh9bk"
do {
    // loop body
} while (condition);
```

Important difference:

```text id="c0slzv"
while      -> condition checked before first iteration
do while   -> body executes at least once
```

Example:

```cpp id="iyyto6"
int x = 10;

do {
    std::cout << x << std::endl;
} while (x < 5);
```

Output:

```text id="vciny5"
10
```

Even though `x < 5` is false, the body runs once.

---

# 5. `for` loop

Basic form:

```cpp id="e0hq6j"
for (init; condition; iteration) {
    // loop body
}
```

Example:

```cpp id="4z1n07"
for (int i = 0; i < 5; ++i) {
    std::cout << i << " ";
}
```

Output:

```text id="w9qo71"
0 1 2 3 4
```

Meaning:

```text id="rb111s"
1. init executes once
2. condition is checked
3. body executes if condition is true
4. iteration expression executes
5. repeat from condition
```

---

## Parts of `for`

```cpp id="5c8xsv"
for (/* init */; /* condition */; /* iteration */) {
}
```

The first part can be:

```text id="ymf2rq"
empty
declaration
expression statement
```

Examples:

```cpp id="d1uz2o"
for (int i = 0; i < 10; ++i) {
}
```

```cpp id="syzdk9"
int i = 0;

for (; i < 10; ++i) {
}
```

```cpp id="94jvry"
for (;;) {
    // infinite loop
}
```

This:

```cpp id="ogyw60"
for (;;) {
}
```

is equivalent to:

```cpp id="xs00ar"
while (true) {
}
```

---

# 6. `continue`

`continue` skips the rest of the current loop iteration and moves to the next iteration.

Example:

```cpp id="2in7ax"
for (int i = 0; i < 5; ++i) {
    if (i == 2) {
        continue;
    }

    std::cout << i << " ";
}
```

Output:

```text id="fa9m7c"
0 1 3 4
```

When `i == 2`, the `std::cout` line is skipped.

In a `for` loop, after `continue`, the iteration expression still runs.

```cpp id="4sr64c"
for (int i = 0; i < 5; ++i) {
    continue;
    // ++i still happens after continue
}
```

---

# 7. `break`

`break` exits the nearest loop or switch.

Example in loop:

```cpp id="sgv5n7"
for (int i = 0; i < 10; ++i) {
    if (i == 5) {
        break;
    }

    std::cout << i << " ";
}
```

Output:

```text id="dkryzt"
0 1 2 3 4
```

Example in switch:

```cpp id="84tqk5"
switch (x) {
case 1:
    std::cout << "one";
    break;
}
```

`break` exits only the nearest loop/switch, not all nested loops.

Example:

```cpp id="thkw7q"
for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
        break; // exits only inner loop
    }
}
```

---

# 8. `goto` and labels

A label marks a place in code:

```cpp id="wev5id"
label:
    statement;
```

`goto` jumps to a label:

```cpp id="whimda"
goto label;
```

Example from your code:

```cpp id="lo0l0d"
label:
    x++;
    std::cout << x;
    goto label;
```

This creates an infinite loop.

Equivalent idea:

```cpp id="qd6j9k"
while (true) {
    x++;
    std::cout << x;
}
```

---

## Why `goto` is usually avoided

`goto` can make code hard to read and maintain.

Bad:

```cpp id="ui2d26"
goto error;

error:
    cleanup();
```

The problem is that control flow becomes less structured.

Usually prefer:

```text id="to3jdl"
if / else
loops
functions
RAII
exceptions
early return
```

In C++, RAII often solves cleanup better than `goto`.

Example:

```cpp id="2grkeg"
void process() {
    File file("data.txt"); // destructor handles cleanup

    if (!file.isValid()) {
        return;
    }

    // work
}
```

---

## When `goto` may appear in real code

`goto` is rare in modern C++, but it can appear in:

```text id="u53qhi"
low-level C code
kernel code
error cleanup paths in C
generated code
some parser/state-machine code
```

In modern C++, it is usually avoided.

---

# 9. Your full code with comments

```cpp id="tz0l46"
#include <iostream>

int main() {
    if (true) {
        // executed if condition is true
    } else if (true) {
        // checked only if first condition was false
    } else {
        // executed if all conditions above were false
    }

    int x = 5;

    switch (x) {
    case 1:
        std::cout << 1;
        break;

    case 2:
        std::cout << 2;
        break;

    default:
        std::cout << x;
    }

    while (true) {
        break; // added to avoid infinite loop
    }

    do {
        break; // body executes at least once
    } while (true);

    for (int i = 0; i < 10; ++i) {
        if (i == 3) {
            continue; // skip this iteration
        }

        if (i == 5) {
            break; // exit the loop
        }
    }

label:
    x++;
    std::cout << x;
    goto label; // infinite loop
}
```

Note: the last `goto` creates an infinite loop, so code after it would not be reached.

---

# 10. Interview-style summary

Control statements change the execution flow of a C++ program. `if / else` is used for conditional branching with boolean conditions. Since C++17, `if` and `switch` can have initializers, which are useful for limiting variable scope.

`switch` is useful when one integral or enum value is compared against many constant cases. It can sometimes be optimized by the compiler into a jump table, but this depends on the case values and compiler decisions. `break` is usually needed to avoid fallthrough.

`while` checks the condition before each iteration, while `do while` executes the body at least once and checks the condition afterward. A `for` loop contains initialization, condition, and iteration expressions, making it convenient for counter-based loops.

`continue` skips the rest of the current iteration, while `break` exits the nearest loop or switch. `goto` jumps to a label, but in modern C++ it is usually avoided because it makes control flow harder to understand. RAII, functions, loops, and early returns are usually better alternatives.
