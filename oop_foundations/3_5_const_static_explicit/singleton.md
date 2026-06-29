# Singleton classes in C++

Singleton means: we want exactly one instance of a class and one global access point to it.

Typical pattern:

```cpp
private constructor
deleted copy constructor
deleted copy assignment
public static get_instance()
```

---

## 1. Heap-based singleton

```cpp
class SingletonObject {
    SingletonObject() {}

    static SingletonObject* INSTANCE;

    SingletonObject(const SingletonObject&) = delete;
    SingletonObject& operator=(const SingletonObject&) = delete;

public:
    static SingletonObject& get_instance() {
        if (INSTANCE == nullptr) {
            INSTANCE = new SingletonObject();
        }
        return *INSTANCE;
    }
};

SingletonObject* SingletonObject::INSTANCE = nullptr;
```

## How it works

```cpp
static SingletonObject* INSTANCE;
```

This is a static data member. It belongs to the class, not to a specific object.

At program start:

```cpp
SingletonObject* SingletonObject::INSTANCE = nullptr;
```

So no singleton object exists yet.

First call:

```cpp
SingletonObject& obj = SingletonObject::get_instance();
```

enters:

```cpp
if (INSTANCE == nullptr) {
    INSTANCE = new SingletonObject();
}
```

So the object is created dynamically on the heap.

After that, every next call returns the same object:

```cpp
return *INSTANCE;
```

So:

```cpp
SingletonObject& a = SingletonObject::get_instance();
SingletonObject& b = SingletonObject::get_instance();
```

`a` and `b` refer to the same object.

---

## Problem 1: no `delete`

The object is created by:

```cpp
new SingletonObject();
```

but there is no:

```cpp
delete INSTANCE;
```

So technically this creates a memory leak.

In singleton code this is sometimes intentional: the singleton is supposed to live until the process ends, and the OS will reclaim memory after program termination.

But the important problem is:

```text
the destructor will not be called
```

That can matter if the singleton owns:

```text
file handle
socket
database connection
log buffer
mutex-related resource
temporary files
```

So this version is okay for learning, but not ideal as default production style.

---

## Problem 2: not thread-safe

This code is not thread-safe:

```cpp
if (INSTANCE == nullptr) {
    INSTANCE = new SingletonObject();
}
```

Two threads can enter at the same time:

```text
Thread 1 sees INSTANCE == nullptr
Thread 2 sees INSTANCE == nullptr
Thread 1 creates object
Thread 2 also creates object
```

Result:

```text
more than one object may be created
one object may be leaked
data race occurs
```

To fix this version, we would need `std::mutex` or `std::call_once`.

---

## 2. Meyers Singleton: local static object

```cpp
class SingletonObject2 {
private:
    SingletonObject2() = default;

public:
    SingletonObject2(const SingletonObject2&) = delete;
    SingletonObject2& operator=(const SingletonObject2&) = delete;

    static SingletonObject2& get_instance() {
        static SingletonObject2 instance;
        return instance;
    }
};
```

This is the standard modern C++ singleton pattern, called **Meyers Singleton**.

---

## What does `static` mean here?

```cpp
static SingletonObject2 instance;
```

This is a local static variable.

It has:

```text
local scope
static lifetime
```

Meaning:

```text
the name `instance` is visible only inside get_instance()
but the object lives until the end of the program
```

It is not a normal stack object.

If we wrote:

```cpp
SingletonObject2 instance;
return instance;
```

that would be wrong, because `instance` would be destroyed when the function returns.

But with:

```cpp
static SingletonObject2 instance;
```

the object is created once and survives after the function ends.

---

## When is it created?

This object is created lazily.

That means it is created on the first call:

```cpp
SingletonObject2::get_instance();
```

Not necessarily at program start.

Sequence:

```text
first get_instance() call -> constructor runs
later get_instance() calls -> same object is returned
program termination -> destructor runs automatically
```

Since C++11, initialization of local static variables is thread-safe.

So two threads cannot accidentally create two instances.

---

## Why delete copy constructor and copy assignment?

```cpp
SingletonObject2(const SingletonObject2&) = delete;
SingletonObject2& operator=(const SingletonObject2&) = delete;
```

Without this, someone could try to copy the singleton:

```cpp
SingletonObject2 copy = SingletonObject2::get_instance();
```

That would create another object and break the singleton idea.

Deleting copy operations prevents this.

---

## Main difference between both versions

### Heap-based singleton

```cpp
static SingletonObject* INSTANCE;
INSTANCE = new SingletonObject();
```

Characteristics:

```text
object is allocated on heap
manual lifetime
usually no delete
destructor may never run
not thread-safe without synchronization
```

### Meyers Singleton

```cpp
static SingletonObject2 instance;
```

Characteristics:

```text
no new
no delete
automatic lifetime management
created on first use
destroyed at program termination
thread-safe since C++11
```

For normal modern C++, prefer Meyers Singleton.

---

## Why would someone still use heap-based singleton?

Sometimes people intentionally allocate singleton with `new` and never delete it to avoid the static destruction order problem.

Problem:

```text
static objects from different translation units may be destroyed in an order that is hard to control
```

If one static object uses the singleton in its destructor, but the singleton was already destroyed, we get undefined behavior.

Heap-based singleton without `delete` avoids destruction, so this exact problem disappears.

But the cost is:

```text
controlled memory leak
destructor is not called
manual lifetime design
```

This is a special case, not the default choice.

---

## Recommended version

```cpp
class SingletonObject {
private:
    SingletonObject() = default;

public:
    SingletonObject(const SingletonObject&) = delete;
    SingletonObject& operator=(const SingletonObject&) = delete;

    static SingletonObject& get_instance() {
        static SingletonObject instance;
        return instance;
    }
};
```

Use:

```cpp
SingletonObject::get_instance();
```

No `new`, no `delete`, no manual memory management.

---

## Short summary

The first singleton stores a static pointer and creates the object using `new`.

The second singleton stores a local static object inside `get_instance()`.

The first version has manual heap lifetime, possible memory leak, and is not thread-safe.

The second version is simpler, thread-safe since C++11, and automatically destroyed at program termination.

In modern C++, the local static version is usually the correct default.
