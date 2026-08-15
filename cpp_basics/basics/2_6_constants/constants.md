# C++ `const` — Deep Notes

## 1. How to think about `const`

A simple mental model:

```cpp
const int x = 5;
````

means:

```text
x is an int object, but through the name x we are not allowed to modify it.
```

So operations like these are blocked:

```cpp
x = 10; // compile-time error
++x;    // compile-time error
```

You can think of `const` as a type qualifier that removes mutating operations from the interface of the object.

These two forms are identical:

```cpp
const int x = 5;
int const y = 5;
```

Both mean:

```text
constant int
```

In real C++ terminology, `const` is not a completely separate type in the same way as `int` vs `double`, but it creates a **cv-qualified type**.

```text
int        -> non-const int
const int  -> const-qualified int
```

---

# 2. Copying from `const`

Example:

```cpp
int x = 5;
const int y = x;
int z = y;
```

This is valid.

Why?

Because this line:

```cpp
int z = y;
```

creates a new object `z`.

You are not modifying `y`.

You are only reading the value from `y` and using it to initialize `z`.

Conceptually:

```text
y is read-only through y
z is a new independent int
```

After this:

```cpp
z = 10; // OK
```

does not affect `y`.

This is similar to:

```cpp
const std::string s1 = "abc";
std::string s2 = s1; // copy
```

`s2` is not const just because `s1` was const. The constness applies to the original object, not to every copy created from it.

---

# 3. `const` and pointers: the two dimensions

With pointers, there are two separate things that can be const:

```text
1. The pointed object
2. The pointer variable itself
```

That gives us several combinations.

---

## 3.1 Pointer to const object

Example:

```cpp
int x = 5;
int y = 6;

int* p = &x;

const int* pc = p;
```

This means:

```text
pc is a pointer to const int
```

More precisely:

```text
Through pc, the int is treated as read-only.
```

So this is allowed:

```cpp
pc = &y; // OK
```

because the pointer itself is not const.

But this is forbidden:

```cpp
*pc = 8; // compile-time error
```

because `*pc` has type:

```cpp
const int
```

or more precisely, expression `*pc` is a const-qualified lvalue.

Important: this does **not necessarily mean** the real object is physically immutable.

```cpp
int x = 5;

const int* pc = &x;

++x; // OK
```

Now:

```cpp
std::cout << *pc; // sees changed value
```

So `const int*` means:

```text
I promise not to modify the object through this pointer.
```

It does not always mean:

```text
The object can never change.
```

This is extremely important.

---

## 3.2 Why `int* -> const int*` is allowed

This is allowed:

```cpp
int* p = &x;
const int* pc = p;
```

Why?

Because we are reducing permissions.

Original pointer:

```text
int* p
```

allows:

```text
read + write
```

New pointer:

```text
const int* pc
```

allows only:

```text
read
```

So this conversion is safe.

It is like giving someone read-only access to an object you can modify yourself.

---

## 3.3 Why `const int* -> int*` is forbidden

This is forbidden:

```cpp
const int* pc = &x;
int* p2 = pc; // compile-time error
```

Why?

Because this would increase permissions.

If this were allowed, we could do:

```cpp
const int y = 5;

const int* pc = &y;
int* p = pc;   // imagine this were allowed
*p = 10;       // would modify const object
```

That would break const safety.

So C++ does not allow implicit conversion from:

```cpp
const int*
```

to:

```cpp
int*
```

You can force it with `const_cast`, but it is dangerous:

```cpp
int* p = const_cast<int*>(pc);
```

If the original object was truly const, modifying through `p` is undefined behavior.

---

# 4. Const pointer to non-const object

Example:

```cpp
int x = 5;
int y = 6;

int* p = &x;

int* const cp = p;
```

This means:

```text
cp is a const pointer to int
```

So the pointer itself cannot be changed:

```cpp
cp = &y; // compile-time error
```

But the object it points to can be changed:

```cpp
*cp = 8; // OK
```

Because `*cp` has type:

```cpp
int
```

not:

```cpp
const int
```

So:

```cpp
int* const cp
```

means:

```text
constant pointer, mutable pointed object
```

---

# 5. Const pointer to const object

You can also combine both:

```cpp
const int* const p = &x;
```

or equivalently:

```cpp
int const* const p = &x;
```

This means:

```text
p cannot point somewhere else
and
*p cannot be modified through p
```

So both are forbidden:

```cpp
p = &y;  // error
*p = 10; // error
```

---

# 6. How to read pointer declarations

A practical rule:

```text
Read from right to left.
```

Example:

```cpp
const int* p;
```

Read:

```text
p is a pointer to const int
```

Example:

```cpp
int* const p;
```

Read:

```text
p is a const pointer to int
```

Example:

```cpp
const int* const p;
```

Read:

```text
p is a const pointer to const int
```

These two are identical:

```cpp
const int* p;
int const* p;
```

But this is different:

```cpp
int* const p;
```

---

# 7. `new const int`

Your note:

```cpp
const int* p = new const int; // CTE - should be initialized
const int* p = new const int(1); // OK
```

Correct idea.

This is problematic:

```cpp
const int* p = new const int;
```

A `const int` object must be initialized because after creation you cannot assign to it.

For fundamental types, default-initialization of `const int` does not give it a usable initialized value.

Correct:

```cpp
const int* p = new const int(1);
```

or:

```cpp
const int* p = new const int{};
```

The second one value-initializes it to `0`.

Then:

```cpp
delete p;
```

is fine.

Important: `delete` does not require a non-const pointer. You may delete through `const int*`:

```cpp
const int* p = new const int(1);
delete p; // OK
```

The object is destroyed/deallocated. You are not modifying it as an `int`; you are ending its lifetime.

---

# 8. `new const int[10]`

Your note:

```cpp
const int* p = new const int[10]{}; // OK
```

Correct.

This creates an array of 10 const ints, value-initialized to zero.

You cannot modify elements:

```cpp
p[0] = 5; // error
```

You must deallocate with:

```cpp
delete[] p;
```

because it was allocated with `new[]`.

---

# 9. The `const int**` problem

Your code:

```cpp
int* p = new int(0);
int** pp = &p;

const int** cpp = pp; // compile-time error
```

This is a classic const-safety problem.

At first glance, it may look like this should be okay:

```text
int* converts to const int*
so maybe int** should convert to const int**
```

But it is not safe.

Let’s see why.

Imagine C++ allowed this:

```cpp
int x = 0;
int* p = &x;
int** pp = &p;

const int** cpp = pp; // imagine this were allowed
```

Now create a truly const object:

```cpp
const int c = 42;
```

Through `cpp`, we could write:

```cpp
*cpp = &c;
```

Is this allowed?

`cpp` has type:

```cpp
const int**
```

So `*cpp` has type:

```cpp
const int*
```

Assigning `&c` to `*cpp` looks legal.

But remember:

```text
cpp points to the same place as pp
```

So this operation actually changes `p`.

Now `p` points to `c`.

But `p` has type:

```cpp
int*
```

So now we can write:

```cpp
*p = 10;
```

That would modify a `const int`.

Full dangerous chain:

```cpp
const int c = 42;

int* p;
int** pp = &p;

const int** cpp = pp; // if allowed

*cpp = &c; // stores address of const int into p
*p = 10;  // modifies const int through int*
```

This would break the type system.

Therefore C++ forbids:

```cpp
int** -> const int**
```

The key lesson:

```text
Adding const one level deep is safe.
Adding const through multiple pointer levels can be unsafe.
```

Safe:

```cpp
int* -> const int*
```

Not safe:

```cpp
int** -> const int**
```

---

# 10. `const` with references

Example:

```cpp
int x = 5;

const int& r = x;
```

This means:

```text
r is a reference to const int
```

or:

```text
through r, x is read-only
```

So:

```cpp
r = 10; // error
```

But:

```cpp
x = 10; // OK
```

because the original object is not const.

After this:

```cpp
std::cout << r; // sees 10
```

Again, `const int&` does not necessarily mean the object can never change. It means this access path cannot modify it.

This is the same idea as:

```cpp
const int* p = &x;
```

but with reference syntax.

These are identical:

```cpp
const int& r = x;
int const& r = x;
```

---

# 11. Why `int& r2 = r` is forbidden

Example:

```cpp
int x = 5;

const int& r = x;
int& r2 = r; // error
```

Why?

Because `r` provides read-only access.

If this were allowed:

```cpp
int& r2 = r;
r2 = 10;
```

then we could modify through a supposedly const reference.

So this would increase permissions:

```text
const int& -> int&
```

C++ forbids it.

Again, constness can be added implicitly:

```cpp
int& -> const int&
```

but cannot be removed implicitly:

```cpp
const int& -> int&
```

---

# 12. Function parameter options

Assume we want to write a function taking `std::string`.

Ignoring move semantics and rvalue references for now, we have four main options:

```cpp
void f(std::string s);
void f(const std::string s);
void f(std::string& s);
void f(const std::string& s);
```

Let’s analyze them.

---

## 12.1 Pass by value: `std::string s`

```cpp
void f(std::string s) {
    // s is a local copy
}
```

The function receives its own `std::string`.

The caller’s string is not modified.

```cpp
std::string x = "abc";
f(x);
```

Inside `f`, `s` is a separate object.

Use this when:

```text
1. You need your own copy anyway.
2. The object is small and cheap to copy.
3. You want to store or modify the local copy.
4. In modern C++, you may combine pass-by-value with move semantics.
```

For large objects, pass by value can be expensive because it copies.

---

## 12.2 Pass by const value: `const std::string s`

```cpp
void f(const std::string s) {
    // s is a local copy, but cannot be modified
}
```

This is rarely useful.

Why?

You still pay for a copy.

And the `const` only protects the local copy inside the function.

The caller does not care whether your local copy is const or not.

From the caller’s point of view:

```cpp
void f(std::string s);
void f(const std::string s);
```

are the same interface.

In function declarations, top-level `const` on by-value parameters is ignored for overloading.

So these cannot be overloaded:

```cpp
void f(std::string s);
void f(const std::string s); // same function signature
```

However, `const` by value can sometimes be useful inside function definitions to prevent accidental modification of a local parameter:

```cpp
void f(const int n) {
    // n cannot be accidentally changed here
}
```

Some programmers use this style for clarity, but it is not common for class types like `std::string`.

Important distinction:

```text
Top-level const on by-value parameter affects only the function body.
It is not part of the function signature.
```

---

## 12.3 Pass by non-const reference: `std::string&`

```cpp
void f(std::string& s) {
    s += "!";
}
```

Use this when the function should modify the caller’s object.

Example:

```cpp
std::string x = "abc";
f(x);

// x == "abc!"
```

This requires an lvalue `std::string`.

This works:

```cpp
std::string s = "abc";
f(s);
```

This does not:

```cpp
f("abc"); // error
```

because `"abc"` is not a `std::string` lvalue. It can be used to create a temporary `std::string`, but a non-const lvalue reference cannot bind to a temporary.

---

## 12.4 Pass by const reference: `const std::string&`

```cpp
void f(const std::string& s) {
    std::cout << s;
}
```

Use this when:

```text
1. You do not want to modify the argument.
2. You want to avoid copying.
3. You want to accept both lvalues and temporaries.
```

Examples:

```cpp
std::string s = "abc";
const std::string cs = "cde";

f(s);      // OK
f(cs);     // OK
f("abc");  // OK, temporary std::string is created
```

This is one of the most common C++ parameter styles.

---

# 13. Why `const T&` can bind to rvalues

This works:

```cpp
const int& r = 5;
```

A temporary `int` object is created and the const reference binds to it.

This also works:

```cpp
const std::string& s = std::string("abc");
```

and this:

```cpp
const std::string& s = "abc";
```

In the second case, `"abc"` is a string literal of type:

```cpp
const char[4]
```

Then a temporary `std::string` is constructed from it:

```cpp
std::string("abc")
```

Then `s` binds to that temporary.

The important rule:

```text
A const lvalue reference can bind to a temporary.
The lifetime of the temporary is extended to the lifetime of the reference,
if the reference is directly initialized from the temporary.
```

This is called **lifetime extension**.

---

# 14. Lifetime extension example

Example:

```cpp
const std::string& s = "aaaaaa";
```

What actually happens?

Step by step:

```text
1. "aaaaaa" is a string literal.
2. The string literal has type const char[7].
3. std::string has a constructor from const char*.
4. A temporary std::string object is created.
5. The const reference s binds to that temporary.
6. The temporary's lifetime is extended to the lifetime of s.
```

Conceptually:

```cpp
const std::string hidden_temp("aaaaaa");
const std::string& s = hidden_temp;
```

When is the temporary destroyed?

At the end of the scope where `s` was created:

```cpp
{
    const std::string& s = "aaaaaa";
    std::cout << s;
} // temporary std::string is destroyed here
```

The compiler knows this because the lifetime extension rule is part of the language.

---

# 15. Where is the string stored in `const std::string& s = "aaaaaa";`?

There are two different objects involved.

## 1. String literal

```cpp
"aaaaaa"
```

This literal has static storage duration.

It is usually stored in `.rodata`:

```text
.rodata:
    'a' 'a' 'a' 'a' 'a' 'a' '\0'
```

## 2. Temporary `std::string`

This object is created from the literal.

```cpp
const std::string& s = "aaaaaa";
```

creates a temporary `std::string`.

The `std::string` object itself contains something like:

```text
size
capacity or metadata
pointer to characters OR small internal buffer
```

Modern `std::string` usually uses **SSO**, Small String Optimization.

For small strings like `"aaaaaa"`, the characters may be stored directly inside the `std::string` object, without heap allocation.

For longer strings, the `std::string` object may allocate memory on the heap and copy the characters there.

So depending on implementation and string length:

```text
small string  -> stored inside std::string object
large string  -> stored in heap buffer owned by std::string
```

The temporary `std::string` object has automatic-like lifetime extended by the const reference.

When the reference scope ends, the temporary `std::string` destructor runs. If it owns heap memory, it frees it.

---

# 16. Important limitation of lifetime extension

Lifetime extension works here:

```cpp
const std::string& s = std::string("abc");
```

But be careful with function returns.

Bad:

```cpp
const std::string& bad() {
    return std::string("abc");
}
```

This returns a reference to a temporary that is destroyed at the end of the return expression.

The caller receives a dangling reference.

Also bad:

```cpp
const std::string& bad() {
    std::string local = "abc";
    return local;
}
```

`local` is destroyed when the function returns.

Lifetime extension does not magically make local objects live forever.

Good:

```cpp
std::string good() {
    return "abc";
}
```

Return by value is correct. Modern C++ optimizes this well.

---

# 17. Why non-const references cannot bind to temporaries

This is forbidden:

```cpp
int& r = 5;
```

Also:

```cpp
std::string& s = std::string("abc");
```

Why?

Because if non-const references could bind to temporaries, this would be possible:

```cpp
void g(size_t& y) {
    ++y;
}

int x = 0;
g(x);
```

Your note says this would create a local copy and `x` would remain `0`.

But actually in C++, this code does **not compile**.

Why?

`x` is an `int`, but `g` expects `size_t&`.

To call `g`, the compiler would need to convert `int` to `size_t`, creating a temporary `size_t`.

If non-const references could bind to temporaries, `g` would modify only that temporary, not `x`.

That would be very misleading.

So C++ forbids this.

Correct behavior:

```cpp
void g(size_t& y) {
    ++y;
}

int x = 0;
g(x); // compile-time error
```

This rule protects you from thinking you modified `x` when actually only a temporary was modified.

If the function were:

```cpp
void g(size_t y) {
    ++y;
}
```

then it would compile because `y` is a copy.

If the function were:

```cpp
void g(const size_t& y) {
    // read-only
}
```

then it would also compile, because binding a const reference to a temporary is allowed.

But since it is const, the function cannot modify the temporary.

---

# 18. `std::string_view` vs `const std::string&`

You asked:

> How can we use `std::string_view` instead of `const string&`?

`std::string_view` is a lightweight non-owning view:

```text
pointer + length
```

It does not own characters.

Example:

```cpp
void f(std::string_view s) {
    std::cout << s;
}
```

This can accept:

```cpp
std::string str = "abc";

f(str);      // OK
f("abc");    // OK
```

Why is `string_view` useful?

Unlike C-strings, it stores length:

```text
pointer + size
```

So it can represent substrings and strings with embedded `'\0'`.

Unlike `const std::string&`, it does not require constructing a temporary `std::string` from a string literal.

Compare:

```cpp
void f(const std::string& s);
f("abc");
```

This may construct a temporary `std::string`.

But:

```cpp
void f(std::string_view s);
f("abc");
```

creates a `string_view` pointing directly to the literal.

No allocation. No copying.

---

## 18.1 Danger of `std::string_view`

`std::string_view` does not own the memory.

So this is dangerous:

```cpp
std::string_view bad() {
    std::string s = "abc";
    return s;
}
```

After the function returns, `s` is destroyed, and the returned `string_view` dangles.

Also dangerous:

```cpp
std::string_view sv;

{
    std::string s = "abc";
    sv = s;
}

std::cout << sv; // dangling
```

So use `string_view` mostly for parameters:

```cpp
void parse(std::string_view input);
```

Do not store it unless you are sure the underlying characters outlive the view.

---

# 19. Function overloads with `const string&` and `string&`

Your code:

```cpp
void f(const std::string&); // 1
void f(std::string&);       // 2
```

These are two different functions.

Why?

Because:

```cpp
std::string&
```

and:

```cpp
const std::string&
```

are different parameter types.

One allows modification, the other does not.

Example:

```cpp
std::string s = "abc";
const std::string cs = "cde";

f(s);      // calls f(std::string&)
f("abc");  // calls f(const std::string&)
f(cs);     // calls f(const std::string&)
```

---

## 19.1 Why does `f(s)` call `std::string&`?

`s` is a non-const lvalue `std::string`.

Both overloads are viable:

```cpp
f(std::string&);        // can bind to s
f(const std::string&);  // can also bind to s
```

But:

```cpp
std::string&
```

is a better match because it does not add const.

So:

```cpp
f(s);
```

calls:

```cpp
f(std::string&);
```

---

## 19.2 Why does `f(cs)` call `const std::string&`?

`cs` is const:

```cpp
const std::string cs = "cde";
```

A non-const reference cannot bind to a const object:

```cpp
std::string& // cannot bind to const std::string
```

So only this overload works:

```cpp
f(const std::string&);
```

---

## 19.3 Why does `f("abc")` call `const std::string&`?

`"abc"` is a string literal:

```cpp
const char[4]
```

There is no non-const `std::string` lvalue.

To call `f`, the compiler may create a temporary `std::string`.

A temporary can bind to:

```cpp
const std::string&
```

but cannot bind to:

```cpp
std::string&
```

So:

```cpp
f("abc");
```

calls:

```cpp
f(const std::string&);
```

---

## 19.4 What about this?

```cpp
const std::string& cps = s;
f(cps);
```

Even though the original object `s` is non-const, the expression `cps` has type:

```cpp
const std::string
```

through this access path.

So overload resolution sees a const lvalue.

Therefore:

```cpp
f(cps);
```

calls:

```cpp
f(const std::string&);
```

This is important:

```text
Constness is attached to the expression/access path, not only to the underlying object.
```

---

# 20. Why can we overload `string&` and `const string&`, but not `string` and `const string`?

This works:

```cpp
void f(std::string&);
void f(const std::string&);
```

because these parameter types are genuinely different.

But this does not work:

```cpp
void f(std::string);
void f(const std::string); // redefinition / same signature
```

Why?

Because for by-value parameters, top-level const is ignored in function type.

When a function takes by value:

```cpp
void f(std::string s);
```

the caller gives an argument, and the function receives its own copy.

Whether the local copy is const or not is an implementation detail of the function body.

From the caller’s point of view, these are the same:

```cpp
void f(std::string);
void f(const std::string);
```

But for references:

```cpp
void f(std::string&);
void f(const std::string&);
```

the const changes what arguments can bind and whether the function may modify the original object. Therefore it is part of the function type.

---

# 21. Top-level const vs low-level const

This distinction is essential.

## Top-level const

Top-level const means the object itself is const.

Example:

```cpp
int* const p;
```

`p` is const.

The pointer itself cannot change.

Another example:

```cpp
const int x;
```

`x` itself is const.

For function parameters passed by value:

```cpp
void f(const int x);
```

the const is top-level. It affects only the local variable `x` inside `f`.

So this is same signature as:

```cpp
void f(int x);
```

## Low-level const

Low-level const applies to the object being pointed/referred to.

Example:

```cpp
const int* p;
```

The pointer is not const, but the pointed int is treated as const.

Example:

```cpp
const int& r;
```

The referred int is treated as const.

Low-level const matters for overloads:

```cpp
void f(int*);
void f(const int*);
```

These are different.

Also:

```cpp
void f(int&);
void f(const int&);
```

These are different.

---

# 22. `const` and member functions

This was not in your notes, but it is important for C++.

Example:

```cpp
struct StringWrapper {
    std::string s;

    size_t size() const {
        return s.size();
    }

    void append(char c) {
        s.push_back(c);
    }
};
```

The `const` after function name:

```cpp
size_t size() const
```

means:

```text
This member function can be called on const objects.
Inside it, *this is treated as const.
```

Example:

```cpp
const StringWrapper w{"abc"};

w.size();      // OK
w.append('x'); // error
```

This is why containers have both const and non-const overloads:

```cpp
T& operator[](size_t i);
const T& operator[](size_t i) const;
```

For a non-const vector:

```cpp
v[i] = 10; // returns T&
```

For a const vector:

```cpp
const std::vector<int> v = {1, 2, 3};
v[i] = 10; // error, operator[] returns const T&
```

This is how constness propagates through APIs.

---

# 23. `mutable`

Sometimes a class wants to allow internal cache modification even in a const member function.

Example:

```cpp
struct Data {
    std::string text;
    mutable bool cached = false;
    mutable size_t cached_hash = 0;

    size_t hash() const {
        if (!cached) {
            cached_hash = std::hash<std::string>{}(text);
            cached = true;
        }

        return cached_hash;
    }
};
```

`mutable` means:

```text
This field may be modified even through const access.
```

Use carefully. It is usually for caches, logging, mutexes, or lazy evaluation.

---

# 24. `const` does not mean thread-safe

Important practical point:

```cpp
const int& r = x;
```

means:

```text
this code path cannot modify x
```

It does not mean:

```text
no one else can modify x
```

Example:

```cpp
int x = 5;
const int& r = x;

x = 10;

std::cout << r; // 10
```

In multithreaded code, `const` does not automatically protect from data races.

If another thread modifies the object while you read it through a const reference, you still need synchronization.

---

# 25. Practical parameter guidelines

For now, before move semantics:

## Small cheap types

Use pass by value:

```cpp
void f(int x);
void f(double x);
void f(char c);
```

No need for:

```cpp
void f(const int& x);
```

because reference may be implemented as a pointer, and passing an `int` by value is cheaper.

## Large read-only objects

Use const reference:

```cpp
void f(const std::string& s);
void f(const std::vector<int>& v);
void f(const Matrix& m);
```

No copy, no modification.

## Objects to modify

Use non-const reference:

```cpp
void normalize(Matrix& m);
void trim(std::string& s);
```

## Optional object

Use pointer:

```cpp
void set_logger(Logger* logger);
```

where `nullptr` can mean no logger.

## Text input only

Often prefer:

```cpp
void parse(std::string_view s);
```

instead of:

```cpp
void parse(const std::string& s);
```

if you only need to read characters and do not need ownership or null-termination.

---

# 26. Corrected code examples

## Pointer to const

```cpp
int x = 5;
int y = 6;

int* p = &x;

const int* pc = p;

pc = &y;  // OK
//*pc = 8; // error

++x; // OK, object itself is not const
```

## Const pointer

```cpp
int x = 5;
int y = 6;

int* p = &x;
int* const cp = p;

//cp = &y; // error
*cp = 8;   // OK
```

## Const pointer to const

```cpp
int x = 5;
const int* const p = &x;

//p = nullptr; // error
//*p = 10;    // error
```

## Reference to const

```cpp
int x = 5;

const int& r = x;

//r = 10; // error

x = 10; // OK
```

## Lifetime extension

```cpp
const std::string& s = "aaaaaa";

std::cout << s;

// temporary std::string is destroyed at end of scope
```

## Overloads

```cpp
void f(const std::string&);
void f(std::string&);

int main() {
    std::string s = "abc";
    const std::string cs = "cde";

    f(s);      // string&
    f(cs);     // const string&
    f("abc");  // const string&
}
```

## `size_t&` example

```cpp
void g(size_t& y) {
    ++y;
}

int main() {
    int x = 0;
    // g(x); // error: cannot bind size_t& to int
}
```

This is intentionally forbidden because otherwise the function could modify only a temporary converted `size_t`, not the original `int`.

---

# 27. Interview-style summary

## What does `const` mean?

`const` means the object cannot be modified through this access path.

It blocks mutating operations such as assignment and increment.

## Is `const int*` a const pointer?

No.

```cpp
const int* p;
```

means pointer to const int.

The pointer can change, but `*p` cannot be modified through `p`.

## What is `int* const p`?

It is a const pointer to int.

The pointer cannot change, but the pointed int can be modified.

## Why is `int*` convertible to `const int*`?

Because it reduces permissions: from read-write access to read-only access.

## Why is `const int*` not convertible to `int*`?

Because that would increase permissions and could allow modifying a const object.

## Why is `int**` not convertible to `const int**`?

Because it would allow storing a pointer to a const int inside an `int*`, and then modifying a const object through that `int*`.

## Why is `const std::string` by value rarely useful?

Because the function already receives a copy. Making that local copy const usually does not affect the caller and does not avoid copying.

## Why is `const std::string&` useful?

It avoids copying and prevents modification. It can also bind to temporaries and literals converted to temporary strings.

## What is lifetime extension?

When a const reference is directly initialized from a temporary, the temporary lives as long as the reference.

```cpp
const std::string& s = "abc";
```

The temporary `std::string` lives until `s` goes out of scope.

## Why can we overload `T&` and `const T&`, but not `T` and `const T`?

Because for references, const changes the referred type and affects binding/modification.

For by-value parameters, top-level const affects only the local copy inside the function and is not part of the function signature.

---

# 28. Practical engineering lesson

`const` is one of the most important tools in C++ API design.

It communicates:

```text
what can be modified
what is read-only
what can bind to temporaries
what ownership/lifetime assumptions exist
```

Good const usage makes code safer and easier to optimize mentally.

The most important mental models:

```text
const does not always mean the object can never change.
It means this access path cannot modify it.

const T& means read-only non-owning access.
T& means mutable non-owning access.
T* const means the pointer cannot change.
const T* means the pointee cannot be changed through this pointer.
```

This topic is deeply connected to references, pointers, overload resolution, object lifetime, API design, and performance.