### Inheritance — compact theory sheet

**1. A derived object contains base-class subobjects.**

```cpp
struct Base { int x; };
struct Derived : Base { int y; };
```

Conceptually:

```text
Derived
├── Base subobject
│   └── x
└── Derived part
    └── y
```

A conversion such as:

```cpp
Derived* -> Base*
Derived& -> Base&
```

refers to that `Base` subobject. It does **not** create a new `Base`.

Also, the pointer value is not guaranteed to stay numerically identical. With multiple inheritance the compiler may need **pointer adjustment** because the requested base subobject may be located at a nonzero offset.

---

**2. Passing by reference/pointer does not slice; passing by value can.**

```cpp
void f(const Base&); // no slicing
void f(Base*);       // no slicing
void f(Base);        // slicing
```

For:

```cpp
Derived d;
f(d); // f(Base)
```

a new `Base` object is copy-constructed from the `Base` subobject of `d`.

The `Derived`-specific state is simply absent from the new object.

```text
Derived { Base{x}, y }
           ↓ copy
Base { x }
```

This is **object slicing**.

---

**3. Slicing uses `Base`'s constructor, not `Derived`'s copy constructor.**

```cpp
Base b = derived;
```

can work even if:

```cpp
Derived(const Derived&) = delete;
```

because the requested destination object is `Base`.

But:

```cpp
Derived d2 = d;
```

requires a `Derived` copy constructor. `Base(const Base&)` cannot construct the complete `Derived`, because it knows nothing about the derived subobjects/members.

---

**4. Name lookup happens before overload resolution.**

This is one of the most important interview rules.

```cpp
struct Base {
    void h(int, int);
};

struct Derived : Base {
    void h(int);
};
```

For:

```cpp
d.h(1, 2);
```

the compiler does approximately:

```text
1. name lookup for "h"
2. lookup finds Derived::h
3. base scope is hidden
4. overload resolution considers the found overload set
5. Derived::h(int) cannot accept two arguments
6. compile error
```

It does **not** say:

> "`Derived::h` doesn't work, so I'll try `Base::h`."

That's the wrong mental model.

Use:

```cpp
using Base::h;
```

to bring the base overloads into the derived overload set.

---

**5. Hiding also applies to data members.**

```cpp
struct Base {
public:
    int x;
};

struct Derived : Base {
private:
    int x;
};
```

For:

```cpp
Derived d;
d.x;
```

name lookup finds `Derived::x`.

Then access control is checked and fails because it is private.

The compiler does **not fall back** to `Base::x`.

You can explicitly qualify:

```cpp
d.Base::x
```

provided that the base/member is accessible in that context.

So remember:

> **Lookup first, accessibility later.**

“More specific” is useful intuition, but in an interview say **derived-scope name lookup hides the base declaration**.

---

**6. Member access and inheritance access are separate mechanisms.**

A base member itself can be:

```cpp
private
protected
public
```

and inheritance can separately be:

```cpp
public Base
protected Base
private Base
```

For base members:

* `private` — accessible only to `Base` and its friends.
* `protected` — accessible to `Base`, its friends, and appropriate derived-class contexts.
* `public` — generally accessible.

Inheritance mode then transforms how `Base`'s public/protected interface is exposed through `Derived`.

```text
public inheritance:
Base public    -> Derived public
Base protected -> Derived protected

protected inheritance:
Base public    -> Derived protected
Base protected -> Derived protected

private inheritance:
Base public    -> Derived private
Base protected -> Derived private
```

`Base`'s private members still physically belong to the base subobject, but `Derived` cannot directly access them.

---

**7. Private/protected inheritance mainly affects accessibility of the base relationship.**

For:

```cpp
struct Derived : private Base {};
```

inside `Derived`, the class can still use accessible `Base` members.

But outside:

```cpp
Derived d;
Base* p = &d; // inaccessible conversion
```

because `Base` is an inaccessible base in that context.

So instead of saying:

> "the outside world doesn't know Derived inherits Base"

prefer:

> **The derived-to-base conversion and inherited interface are inaccessible from that context.**

That's more precise C++ terminology.

---

**8. `protected` has an important object-expression restriction.**

Your example was good:

```cpp
struct Son : Mom {
    void f(Granny& g) {
        g.prot_x; // error
    }
};
```

Being inside `Son` does **not** mean you may manipulate the protected member of arbitrary `Granny` objects.

Protected access through an object has additional restrictions: from `Son`, access must go through an appropriate `Son`/derived object, not an unrelated `Granny` object.

This is a common nasty follow-up.

---

**9. A `using` declaration can change how an inherited member is exposed.**

```cpp
struct Base {
protected:
    void f();
};

struct Derived : Base {
public:
    using Base::f;
};
```

Now:

```cpp
Derived d;
d.f(); // OK
```

The original member belongs to `Base`, but `Derived` publicly introduces that name into its interface.

Similarly:

```cpp
using Base::h;
```

is commonly used to prevent overload hiding.

---

**10. Construction follows subobject structure, not initializer-list text order.**

For ordinary inheritance:

```text
construction:
base subobjects
→ data members in declaration order
→ constructor body

destruction:
constructor body ends
→ members in reverse order
→ bases in reverse order
```

For your example:

```text
Base::mb constructed
Base constructor body

Derived::md constructed
Derived constructor body

Derived destructor body
Derived::md destroyed

Base destructor body
Base::mb destroyed
```

The initializer-list order does not control this:

```cpp
Derived(...) : y(...), Base(...), md(...) {}
```

still follows the language-defined subobject order.

This guarantees that construction/destruction order is predictable independently of how someone rearranges the initializer list.

---

**11. Inherited constructors are not inherited special-member constructors.**

```cpp
struct Derived : Base {
    using Base::Base;
};
```

lets `Derived` use appropriate ordinary constructors inherited from `Base`.

But this does **not** mean:

```cpp
Base(const Base&)
```

becomes:

```cpp
Derived(const Derived&)
```

Copy/move constructors are handled separately.

The compiler can still implicitly generate:

```cpp
Derived(const Derived&);
```

which conceptually performs:

```cpp
Derived(const Derived& other)
    : Base(other),
      y(other.y) {}
```

So copying a `Derived` copy-constructs its base subobject and then its own members.

Also, if `Derived` declares its own constructor with the same effective signature as an inherited one, the `Derived` constructor wins.

---

**12. Default member initializers are fallbacks, not copy rules.**

```cpp
struct Derived : Base {
    int y = 0;
};
```

doesn't mean every new `Derived` gets `y == 0`.

During copy construction:

```cpp
Derived d2(d);
```

the implicit copy constructor copies:

```cpp
y(other.y)
```

so if `d.y == 1`:

```cpp
d2.y == 1
```

The `= 0` initializer is used when a constructor doesn't otherwise initialize that member.

---

**13. Layout: reason conceptually, but don't overpromise ABI details.**

You correctly experimented with:

```cpp
sizeof
alignof
addresses
```

A derived object contains base subobjects and its own members, but exact padding, offsets, and many layout details depend on implementation/ABI.

For example:

```cpp
struct A { int a; };
struct B : A { double b; };
```

a common implementation may produce:

```text
A::a
padding
B::b
```

with `sizeof(B) == 16`, but don't claim that number is universally required by the C++ language.

Same for **Empty Base Optimization**:

```cpp
struct Empty {};

struct C : Empty {
    int x;
};
```

implementations can usually store the empty base without making the object larger, so commonly:

```cpp
sizeof(C) == sizeof(int)
```

while:

```cpp
struct X {
    Empty e;
    int x;
};
```

the member `e` must still represent a distinct object and normally occupies addressable space.

There are also restrictions when several subobjects of the same empty type must have distinct addresses, which explains some seemingly strange multiple-inheritance layouts.

---

### 60-second interview answer

If they ask **“Explain C++ inheritance beyond the OOP definition”**, a strong answer would be:

> A derived object contains base-class subobjects. Derived-to-base pointer and reference conversions refer to those subobjects and may require pointer adjustment, especially with multiple inheritance. Passing a derived object as a base reference or pointer preserves the complete object, while passing it by value constructs a separate Base object and slices the derived part.
>
> Member name lookup is also important: if a derived scope declares a name, it normally hides declarations with the same name in the base before overload resolution happens; `using Base::f` can restore the base overload set. Access checking happens after lookup, so an inaccessible derived member doesn't cause lookup to fall back to a public base member.
>
> Public, protected, and private inheritance control accessibility of the base relationship and inherited interface, independently from the access level of individual members. Finally, construction follows the subobject model: bases first, then members in declaration order, then the derived constructor body, with destruction in reverse order. Exact physical layout is ABI-dependent.

That's already a **very solid inheritance foundation**.

There are a couple of subtleties in your comments that I intentionally tightened above—especially **“more preferred/more specific” → name lookup**, private inheritance, protected access, and ABI-dependent `sizeof`/EBO. Those are exactly the wording differences that matter in a strong C++ interview.

Now we're ready for the next stage: **I'll interview you, one question at a time, without answering for you, and use follow-ups depending on your answer.**
