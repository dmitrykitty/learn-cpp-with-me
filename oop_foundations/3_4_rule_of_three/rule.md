# Rule of Three and Copy-and-Swap Idiom

## 1. Rule of Three

If a class manually manages a resource, usually dynamic memory, file descriptor, socket, mutex handle, etc., then if you define one of:

```text
destructor
copy constructor
copy assignment operator
```

you probably need all three.

For this class:

```cpp
class String {
    char* arr = nullptr;
    size_t sz = 0;
    size_t cap = 0;
};
```

`arr` owns dynamic memory, so default compiler-generated copy is dangerous.

Default copy constructor / assignment would copy only fields:

```text
copy.arr = original.arr
copy.sz  = original.sz
copy.cap = original.cap
```

Then two objects own the same `arr`, and both destructors call:

```cpp
delete[] arr;
```

Result:

```text
double free / undefined behavior
```

So owning raw pointer requires custom copy logic.

---

## 3. Copy constructor

Your version:

```cpp
String(const String& other) : String(other.sz) {
    memcpy(arr, other.arr, sz);
}
```

This copies only `sz` characters.

Because `String(other.sz)` already does:

```cpp
arr[sz] = '\0';
```

it works for null terminator, but clearer version is:

```cpp
String(const String& other) : String(other.sz) {
    memcpy(arr, other.arr, sz + 1);
}
```

because logically we copy the whole C-string buffer, including `'\0'`.

Important: `memcpy` is fine here because `char` is trivial.

If `arr` contained objects, use `std::copy`.

---

## 4. Naive copy assignment operator

```cpp
String& operator=(const String& other) {
    if (this == &other) {
        return *this;
    }

    delete[] arr;

    sz = other.sz;
    cap = other.cap;
    arr = new char[cap];

    memcpy(arr, other.arr, cap);

    return *this;
}
```

This handles self-assignment:

```cpp
s = s;
```

Without this check, we would delete `arr`, and then try to copy from the same deleted memory.

So self-assignment check is important in this implementation.

But this implementation has a deeper problem.

---

## 5. Exception safety problem

The dangerous part:

```cpp
delete[] arr;

sz = other.sz;
cap = other.cap;
arr = new char[cap];
```

If `new char[cap]` throws `std::bad_alloc`, the object is already damaged:

```text
old memory deleted
sz/cap maybe changed
arr still points to freed memory
destructor later may double delete
```

So this assignment operator is not exception-safe.

Better manual version:

```cpp
String& operator=(const String& other) {
    if (this == &other) {
        return *this;
    }

    char* new_arr = new char[other.cap];
    memcpy(new_arr, other.arr, other.cap);

    delete[] arr;

    arr = new_arr;
    sz = other.sz;
    cap = other.cap;

    return *this;
}
```

Now allocation and copy happen before destroying old data.

If `new` throws, current object remains unchanged.

This gives stronger safety than the previous version.

---

## 6. Copy-and-swap idiom

```cpp
void swap(String& other) {
    std::swap(sz, other.sz);
    std::swap(cap, other.cap);
    std::swap(arr, other.arr);
}
```

`swap` just exchanges fields.

For this class, swapping is cheap:

```text
swap pointer
swap size
swap capacity
```

No allocation, no copying of characters.

---

## 7. Assignment with copy-and-swap

```cpp
String& operator=(String other) {
    swap(other);
    return *this;
}
```

Important detail:

```cpp
String other
```

is passed by value, so a copy of the right-hand side is created before the function body starts.

Example:

```cpp
a = b;
```

Steps:

```text
1. other is copy-constructed from b
2. this->swap(other)
3. now a owns copied data from b
4. other owns old data of a
5. other is destroyed at end of function
6. destructor frees old data of a
```

So copy-and-swap automatically releases old memory safely.

---

## 8. Why copy-and-swap is exception-safe

If copying `other` fails before entering function body:

```cpp
String& operator=(String other)
```

then assignment does not start, and `*this` remains unchanged.

If copy succeeds, `swap` itself should not throw.

For this class, `swap` only swaps primitive fields and pointer, so it is safe.

Therefore copy-and-swap gives strong exception safety:

```text
assignment either fully succeeds or object remains unchanged
```

---

## 9. Self-assignment with copy-and-swap

For:

```cpp
s = s;
```

copy-and-swap works without special `if`.

Steps:

```text
1. create copy of s
2. swap s with its copy
3. destroy old state now stored in copy
```

Result is logically the same string.

So copy-and-swap does not need:

```cpp
if (this == &other)
```

That is one of its advantages.

---

## 10. Cost of copy-and-swap

Copy-and-swap is simple and safe, but may do extra work.

Naive optimized assignment can sometimes reuse existing capacity:

```cpp
if (cap >= other.sz + 1) {
    memcpy(arr, other.arr, other.sz + 1);
    sz = other.sz;
}
```

Copy-and-swap always creates a new copy.

For learning and correctness, copy-and-swap is excellent.

For highly optimized containers, real `std::string`-like classes often implement more specialized assignment logic.

---

## 11. Correct compact class before move semantics

```cpp
class String {
    char* arr = nullptr;
    size_t sz = 0;
    size_t cap = 0;

    String(size_t n) : arr(new char[n + 1]), sz(n), cap(n + 1) {
        arr[sz] = '\0';
    }

public:
    String() = default;

    String(size_t n, char c) : String(n) {
        memset(arr, c, sz);
        std::cout << "String(sz, ch)\n";
    }

    String(std::initializer_list<char> list) : String(list.size()) {
        std::copy(list.begin(), list.end(), arr);
        std::cout << "String(init_list)\n";
    }

    String(const String& other) : String(other.sz) {
        memcpy(arr, other.arr, sz + 1);
    }

    void swap(String& other) noexcept {
        std::swap(arr, other.arr);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    String& operator=(String other) {
        swap(other);
        return *this;
    }

    ~String() {
        delete[] arr;
    }
};
```

`noexcept` on `swap` is useful because swap should not throw. It also matters for some generic code and containers.

---

## 12. Short summary

Rule of Three says: if a class owns a resource and defines destructor/copy constructor/copy assignment, it probably needs all three.

Default copy is shallow field-by-field copy.

For owning raw pointers, shallow copy causes double free.

Copy constructor creates a new independent resource.

Copy assignment must handle self-assignment and exceptions.

Naive assignment with `delete` before `new` is not exception-safe.

Copy-and-swap creates a copy first, swaps fields, and lets destructor clean old state.

Copy-and-swap is simple, safe, and self-assignment-proof, but may be less optimized than custom assignment that reuses capacity.
