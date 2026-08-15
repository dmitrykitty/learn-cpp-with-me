# Scope, storage, initialization, lifetime

## Scope

Scope tells us **where a name can be used**, not how long the object exists.
Main scopes worth remembering:

* **Namespace scope** — names declared directly inside a namespace. The so-called "global scope" is actually the **global namespace scope**.
* **Block/local scope** — names declared inside `{ ... }`, e.g. inside a function, loop, `if`, etc.
* **Class scope** — members declared inside a class.

Important:

```cpp
void f() {
    static Obj a;
    Obj b;
    Obj* c = new Obj;
}
```

All three names have local/block scope, but their lifetimes differ:

* `b` → automatic storage, destroyed when leaving the block.
* `a` → static storage, lives until program termination.
* `*c` → dynamically allocated object, normally lives until `delete c`.

So:

> **scope != storage duration != lifetime**

---

## Storage duration

Storage duration describes how long the **storage containing an object** is guaranteed to exist.Four important categories:

### 1. Automatic storage duration

Typical local variables and function parameters:

```cpp
void f(int p) {
    Obj o;
}
```

Normally their storage lasts until execution leaves the corresponding block/function invocation. Often implemented using the stack. 

---

### 2. Static storage duration

Examples:

```cpp
Obj global;

void f() {
    static Obj local;
}
```

Storage lasts for the duration of the program.
Includes ordinary namespace-scope objects and `static` objects that are not `thread_local`.

---

### 3. Thread storage duration

```cpp
thread_local Obj obj;
```

There is generally a separate object for each thread.
Its storage lasts for the duration of that thread.

---

### 4. Dynamic storage duration

Storage obtained dynamically, typically through allocation mechanisms such as:

```cpp
new Obj
```

or allocation functions. Its lifetime is controlled explicitly rather than by lexical block exit.

---

# Lifetime

Lifetime is a **runtime property of an object or reference**.
For a normal class object, a useful simplified model is:

```text
storage obtained
      ↓
initialization
      ↓
lifetime begins
      ↓
object can be used
      ↓
destruction begins
      ↓
lifetime ends
      ↓
storage may later be released/reused
```

More precisely, an object's lifetime generally begins when:

1. storage with sufficient size and alignment has been obtained, and
2. its initialization has completed.

For:

```cpp
Obj o;
```

the lifetime of `o` begins after its initialization has completed.
For a class object, lifetime normally ends when destruction begins or when its storage is reused/released.


You can have storage without a live `Obj` inside it.

---

# Initialization

```cpp
Obj o;
```

**Default-initialization.**

For a class type, usually calls the default constructor:

```cpp
Obj::Obj();
```

But for built-in types:

```cpp
int x;
```

`x` is not automatically initialized to `0`.

---

```cpp
Obj o = o1;
```

**Copy-initialization.**

For the simple same-type case it normally invokes the copy constructor:

```cpp
Obj(const Obj&);
```

But don't equate:

> `=` during declaration == assignment

It is still initialization.

```cpp
Obj o = o1; // initialization
o = o1;     // assignment
```

Copy-initialization can also involve converting constructors/conversions, so "always copy constructor" is too strong.

---

```cpp
Obj o{};
```

This uses **list-initialization** with an empty initializer list.

For a normal class with a default constructor it will normally call the default constructor.

For built-in values:

```cpp
int x{}; // x == 0
```

This is one major difference from:

```cpp
int x; // indeterminate value
```

Useful forms:

```cpp
Obj a;          // default-initialization
Obj b{};        // list-initialization
Obj c(arg);     // direct-initialization
Obj d{arg};     // direct-list-initialization
Obj e = other;  // copy-initialization
Obj f = {arg};  // copy-list-initialization
```

---

## `std::initializer_list` constructor preference

During list initialization, constructors taking `std::initializer_list` receive **special preference**.

Example:

```cpp
class X {
public:
    X(int, int);
    X(std::initializer_list<int>);
};

X x{1, 2};
```

The `initializer_list<int>` constructor is preferred.

Roughly, overload resolution happens in two phases:

1. Try `std::initializer_list` constructors.
2. Only if no viable initializer-list constructor exists, consider the other constructors.

Example:

```cpp
class X {
public:
    X(int, int);
    X(std::initializer_list<std::string>);
};

X x{1, 2};
```

`int` cannot initialize `std::string`, so the initializer-list constructor is not viable, and `X(int, int)` may be chosen.

**Important trap:** narrowing conversions can cause a compilation error instead of falling back to another constructor.

```cpp
class X {
public:
    X(int, int);
    X(std::initializer_list<bool>);
};

X x{10, 20};
```

The initializer-list constructor may win overload resolution, but `int -> bool` is narrowing in list initialization → **compile error**. C++ does not simply retry `X(int, int)`.

This is a classic interview detail.

---

# Objects

An object is a region of storage associated with a type and having properties such as:

* type,
* size,
* alignment,
* lifetime,
* potentially a value/state.

Do not confuse:

> **storage exists**
> with
> **an object exists in that storage**.

For example:

```cpp
alignas(Obj) std::byte buffer[sizeof(Obj)];
```

There is enough properly aligned **storage** for `Obj`, but this does not necessarily mean an `Obj` object is currently alive there.

That distinction becomes important for:

* placement `new`,
* allocators,
* arenas,
* unions,
* object pools,
* low-level containers.

---

## Object creation

Objects can be created by, among other mechanisms:

* definitions:

```cpp
Obj o;
```

* `new` expressions:

```cpp
Obj* p = new Obj;
```

* placement construction:

```cpp
new (buffer) Obj;
```

* throw expressions create exception objects,
* some language/library operations can implicitly create objects.

Be careful with:

```cpp
malloc(...)
calloc(...)
```

They primarily allocate **raw storage** and **do not invoke constructors**.

So don't generally write:

> "`malloc` creates a C++ object."

For example:

```cpp
void* memory = std::malloc(sizeof(std::string));
```

does not perform:

```cpp
std::string::std::string();
```

There are additional modern C++ rules around **implicit-lifetime types**, but for an interview the key distinction is:

> `new` usually combines storage allocation + object construction.
> `malloc` only provides raw storage and does not run a constructor.

---

# Size and alignment

Every complete object type has a size:

```cpp
sizeof(T)
```

and an alignment requirement:

```cpp
alignof(T)
```

Alignment describes addresses at which an object of that type may be correctly placed.

Typical example:

```cpp
struct S {
    char a; // size 1, alignment 1
    char b; // size 1, alignment 1
};

// typically:
// sizeof(S)  == 2
// alignof(S) == 1
```

Another example:

```cpp
struct X {
    int n;   // typically size 4, alignment 4
    char c;  // size 1, alignment 1

    // typically 3 bytes of tail padding
};

// typically:
// sizeof(X)  == 8
// alignof(X) == 4
```

Why `sizeof(X) == 8` instead of `5`?

Because arrays must work:

```cpp
X arr[2];
```

Both:

```cpp
arr[0].n
arr[1].n
```

must satisfy the alignment requirement of `int`.

Conceptually:

```text
X
+------+------+------+------+------+------+------+------+
|           int n            | char |    padding        |
+------+------+------+------+------+------+------+------+
0                            4                       8
```

Use **padding bytes**, not "padding bits" in this example.

Typical alignments are powers of two.


## Dangling pointer/reference

A pointer/reference can outlive the object it refers to:

```cpp
const int* p;

{
    int x = 10;
    p = &x;
} // x dies

// p still contains an address,
// but it no longer points to a live int object
```

`p` is now **dangling**.

Dereferencing it:

```cpp
std::cout << *p;
```

causes undefined behavior.
Same idea applies to references.

---

### The three concepts I would absolutely remember before an interview

```text
SCOPE
Where can I use the name?

STORAGE DURATION
How long can the storage exist?

OBJECT LIFETIME
During what interval does an actual object exist in that storage?
```

They are related, but **they are not interchangeable**.
That last distinction is probably the single most important correction to your current notes.
