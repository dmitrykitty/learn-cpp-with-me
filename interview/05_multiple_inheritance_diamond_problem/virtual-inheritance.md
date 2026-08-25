Use this hierarchy:

```cpp
#include <iostream>

struct Granny {
    int g;

    explicit Granny(int value)
        : g(value) {
        std::cout << "Granny(" << value << ")\n";
    }
};

struct Parent1 : virtual Granny {
    int p1;

    explicit Parent1(int value)
        : Granny(100 + value),
          p1(value) {
        std::cout << "Parent1, g = " << g << '\n';
    }
};

struct Parent2 : virtual Granny {
    int p2;

    explicit Parent2(int value)
        : Granny(200 + value),
          p2(value) {
        std::cout << "Parent2, g = " << g << '\n';
    }
};

struct Son : Parent1, Parent2 {
    int s;

    Son()
        : Granny(999),
          Parent1(1),
          Parent2(2),
          s(3) {
        std::cout << "Son, g = " << g << '\n';
    }
};
```

The language-level hierarchy is:

```text
             Granny
             /    \
        virtual  virtual
           /        \
      Parent1      Parent2
           \        /
              Son
```

But the most useful **object model** is:

```text
Son complete object

+--------------------+
| Parent1 subobject  | -----\
+--------------------+       \
| Parent2 subobject  | -------+----> one shared Granny
+--------------------+       /
| Son members        |      /
+--------------------+     /
| Granny subobject   | <--/
+--------------------+
```

Those arrows are conceptual, not literal pointers mandated to exist in the object.

---

## 1. First: what does `virtual` inheritance actually mean?

Without virtual inheritance:

```cpp
struct Parent1 : Granny {};
struct Parent2 : Granny {};
struct Son : Parent1, Parent2 {};
```

we get:

```text
Son
├── Parent1
│   └── Granny #1
│
└── Parent2
    └── Granny #2
```

There are two `Granny` subobjects.

With:

```cpp
struct Parent1 : virtual Granny {};
struct Parent2 : virtual Granny {};
```

we get:

```text
Son
├── Parent1 ──┐
├── Parent2 ──┼── Granny
└─────────────┘
```

There is exactly **one shared `Granny` virtual-base subobject in the most-derived object**. cppreference states this in terms of the most-derived object containing one subobject for each distinct virtually inherited base. ([Cppreference][1])

The words **most-derived object** are extremely important.

A standalone:

```cpp
Parent1 p;
```

also contains a `Granny`.

A standalone:

```cpp
Parent2 p;
```

also contains a `Granny`.

A:

```cpp
Son s;
```

contains another `Granny` — shared by its `Parent1` and `Parent2` subobjects.

So virtual inheritance does **not** mean that all `Parent1`s globally share one Granny. Sharing exists only inside one complete most-derived object.

---

# 2. Constructing a plain `Granny`

This is trivial:

```cpp
Granny g{7};
```

The complete object is simply:

```text
Granny
+---------+
| int g   |
+---------+
```

In our sample `Granny` has neither virtual functions nor virtual bases, so there is no need for virtual-inheritance metadata inside a standalone `Granny`.

---

# 3. Now construct a standalone `Parent1`

This is where the first important rule appears:

```cpp
Parent1 p{1};
```

`Parent1` is now the **most-derived class**.

Although its source says:

```cpp
struct Parent1 : virtual Granny
```

the complete object still needs an actual `Granny` somewhere.

Conceptually:

```text
Parent1 complete object

+---------------------+
| Parent1's own part  |
|   p1                |
+---------------------+
| Granny virtual base |
|   g                 |
+---------------------+
```

Exact physical ordering is ABI-dependent. Do not memorize this drawing as the C++ layout.

The construction order is:

```text
1. Granny virtual base
2. Parent1's members
3. Parent1 constructor body
```

Therefore:

```cpp
Parent1(int value)
    : Granny(100 + value),
      p1(value) {
}
```

**does use**:

```cpp
Granny(101)
```

when we're constructing a standalone `Parent1`.

Result:

```text
Granny(101)
Parent1, g = 101
```

Why?

Because right now:

```text
most-derived class = Parent1
```

and the most-derived constructor is responsible for virtual bases. ([Cppreference][1])

---

# 5. Now construct `Son`

This is the interesting one:

```cpp
Son s;
```

Now:

```text
most-derived class = Son
```

Therefore **Son is responsible for every virtual base reachable in the hierarchy**.

The construction order is:

```text
Granny
   ↓
Parent1
   ↓
Parent2
   ↓
Son::s
   ↓
Son constructor body
```

Not:

```text
Parent1
 └─ Granny

Parent2
 └─ Granny
```

The virtual base is always initialized **before the non-virtual direct bases**. Direct bases are then initialized in their declaration order, members in declaration order, and finally the constructor body runs. Changing the order of the mem-initializer list does not change this. ([Cppreference][2])

So this:

```cpp
Son()
    : Granny(999),
      Parent1(1),
      Parent2(2),
      s(3) {
}
```

first executes:

```cpp
Granny(999);
```

There is now one living `Granny` subobject.

Then `Parent1` is constructed.

---

# 6. What happens to `Parent1`'s `Granny(101)`?

This is the subtle rule:

```cpp
Parent1(int value)
    : Granny(100 + value),
      p1(value) {
}
```

You might expect `Granny(101)`.

It does **not happen**.

Why?

Because `Parent1` is not the most-derived object anymore.

It is only:

```text
Parent1-in-Son
```

and its virtual-base initializer is ignored during this construction. The language explicitly says that a mem-initializer naming a virtual base is ignored when the constructor's class is not the most-derived class. ([Cppreference][3])

So:

```text
Son constructs Granny(999)

then Parent1 constructor executes
but Parent1's Granny(101) initializer does nothing
```

Inside the `Parent1` constructor body:

```cpp
std::cout << g;
```

we see:

```text
999
```

because `Parent1` is accessing the already-existing shared `Granny`.

Then exactly the same thing happens to `Parent2`:

```cpp
Parent2(int value)
    : Granny(200 + value),
      p2(value) {
}
```

`Granny(202)` is ignored.

`Parent2` also sees:

```text
g == 999
```

So the complete output begins approximately:

```text
Granny(999)
Parent1, g = 999
Parent2, g = 999
Son, g = 999
```

There was exactly **one call to a Granny constructor**.

---

# 7. Why must the most-derived class construct Granny?

This rule isn't arbitrary.

Imagine that `Parent1` controlled the shared Granny:

```cpp
Parent1() : Granny(101) {}
```

while `Parent2` controlled it differently:

```cpp
Parent2() : Granny(202) {}
```

But inside `Son` there is only **one** Granny.

Then what should happen?

```text
Parent1 says Granny(101)
Parent2 says Granny(202)

but we only have one Granny object
```

There is no reasonable independent answer.

C++ resolves this by moving ownership of virtual-base construction upward:

```text
                 Son
                  |
        "I am the complete object,
         therefore I decide."
                  |
                  v
             Granny(999)
             /         \
        Parent1       Parent2
```

That is the semantic reason for the rule.

---

# 8. So why do `Parent1` and `Parent2` even write `Granny(...)`?

Excellent question.

Because they can themselves be most-derived:

```cpp
Parent1 p1{1};  // Parent1 must construct Granny
Parent2 p2{2};  // Parent2 must construct Granny
```

The exact same constructor source must work in two situations:

```text
Parent1 as complete object

Parent1
   |
 Granny
```

and:

```text
Parent1 as a subobject of Son

Son
├── Parent1
├── Parent2
└── Granny
```

This difference is one of the reasons virtual inheritance gets interesting at implementation level.

---

# 9. Here is the real implementation problem

Suppose `Parent1` contains:

```cpp
void set_granny(int value) {
    g = value;
}
```

Ignore virtual methods — this is just an ordinary non-virtual method.

When called on a standalone `Parent1`:

```cpp
Parent1 p{1};
p.set_granny(5);
```

the object might physically look, on some ABI, roughly like:

```text
Parent1 @ 0x1000

0x1000    hidden metadata
0x1008    p1
0x100C    Granny::g
```

So relative to `Parent1*`:

```text
Granny is at +12
```

But now put `Parent1` inside `Son`.

A typical x86-64 Clang/GCC Itanium-ABI layout for our stripped-down example can look like:

```text
Son @ +0

+0     Parent1 hidden virtual-inheritance pointer
+8     Parent1::p1

+16    Parent2 hidden virtual-inheritance pointer
+24    Parent2::p2

+28    Son::s

+32    Granny::g

sizeof(Son) = 40
```

Again, these numbers are **an implementation example, not C++ guarantees**.

Now from `Parent1*`:

```text
Parent1 -> Granny = +32
```

but for standalone `Parent1`:

```text
Parent1 -> Granny = +12
```

That gives us the critical problem:

```cpp
void Parent1::set_granny(int value) {
    g = value;
}
```

is compiled once.

What constant can the compiler generate?

```asm
mov [this + ???], value
```

It cannot use `+12`:

```text
standalone Parent1: correct
Parent1-in-Son:     wrong
```

It cannot use `+32` either:

```text
standalone Parent1: wrong
Parent1-in-Son:     correct
```

Therefore:

> **The location of a virtual base cannot in general be encoded as one fixed offset relative to an intermediate base class.**

This is the fundamental implementation problem virtual inheritance creates.

---

# 10. This is why runtime layout information is required

For ordinary inheritance:

```cpp
struct Parent1 : Granny {};
```

the relationship between a `Parent1` subobject and its `Granny` subobject is fixed by `Parent1`'s layout.

Conceptually:

```asm
; Parent1* -> Granny*
lea rax, [rdi + CONSTANT]
```

With virtual inheritance:

```cpp
struct Parent1 : virtual Granny {};
```

`Parent1` needs to ask:

> “Which complete object am I currently part of, and where did that complete object put Granny?”

The object therefore needs some way of reaching metadata saying:

```text
standalone Parent1:
    Granny offset = +12

Parent1-in-Son:
    Granny offset = +32

Parent1-in-SomeOtherClass:
    Granny offset = perhaps something else
```

This is exactly the concept the article demonstrates with its `virtual-base offset`. ([Shahar Mike][4])

---

# 12. How does `Parent1* -> Granny*` work?

Suppose:

```cpp
Parent1* p = ...;
Granny* g = p;
```

With ordinary inheritance the compiler might do:

```asm
lea rax, [rdi + 12]
```

With virtual inheritance, conceptually:

```text
Parent1*
    |
    | read hidden metadata pointer
    v
virtual-inheritance table
    |
    | obtain Granny vbase offset
    v
offset
    |
    v
Parent1* + offset
    |
    v
Granny*
```

Pseudo-assembly:

```asm
mov rax, [rdi]       ; load metadata pointer
mov rcx, [rax - K]   ; load Granny's virtual-base offset
add rdi, rcx         ; rdi now points to Granny
```

The Itanium ABI explicitly defines vbase offsets as entries added to the address of the subobject containing the virtual pointer to obtain the virtual-base subobject. ([Itanium C++ ABI][5])

This is where virtual inheritance introduces a genuine runtime address-calculation cost.

---

# 13. `Parent1` and `Parent2` use different offsets to reach the SAME Granny

For our illustrative `Son` layout:

```text
Son @ 0

+0     Parent1
+16    Parent2
+32    Granny
```

From `Parent1`:

```text
0 + 32 = Granny
```

so:

```text
vbase offset = +32
```

From `Parent2`:

```text
16 + 16 = 32
```

so:

```text
vbase offset = +16
```

Therefore:

```cpp
Son son;

Parent1* p1 = &son;
Parent2* p2 = &son;

Granny* g1 = p1;
Granny* g2 = p2;
```

can perform:

```text
p1 + 32
      ↓
   Granny

p2 + 16
      ↓
   Granny
```

and:

```cpp
assert(g1 == g2);
```

Both paths produce the exact same `Granny*`.

That's the runtime manifestation of the language rule:

```text
one shared Granny subobject
```

---

# 14. Now construction creates an even more subtle problem

Remember:

```text
standalone Parent1:
Granny offset = +12

Parent1-in-Son:
Granny offset = +32
```

When the body of:

```cpp
Parent1::Parent1(...)
```

executes, it is the **same compiled constructor source**.

And it is legal for it to do:

```cpp
Parent1::Parent1(...)
    : Granny(...) {
    g = 123;
}
```

When constructing standalone `Parent1`:

```text
this = Parent1 complete object
Granny = this + 12
```

When constructing Parent1-in-Son:

```text
this = Parent1 subobject
Granny = this + 32
```

Therefore the constructor itself also needs the correct virtual-base layout for the particular most-derived object.

This leads us to **construction virtual tables**.

Still no virtual functions involved.

---

# 15. Construction table for `Parent1-in-Son`

While building a complete `Son`, the ABI can temporarily arrange for `Parent1`'s metadata to say:

```text
You are currently:

Parent1-in-Son

Your shared Granny is:
this + 32
```

Conceptually:

```text
Parent1 constructor
      |
      | this = Parent1 subobject
      |
      v
temporary Parent1-in-Son metadata
      |
      | Granny vbase offset = +32
      v
shared Granny
```

Whereas when constructing a standalone `Parent1`, its ordinary table says:

```text
Granny vbase offset = +12
```

The blog's virtual-inheritance article demonstrates exactly this distinction using `construction vtable for Parent1-in-Child`: the construction table contains the virtual-base offset appropriate to the layout of the complete `Child`, not standalone `Parent1`. ([Shahar Mike][4])

The Itanium ABI explains the reason directly: a complete base object and that same base embedded in a derived object can place their virtual bases at different offsets, so special construction tables are needed. ([Itanium C++ ABI][6])

---

# 16. Even deeper: the compiler may have two conceptual Parent1 constructors

This is an ABI implementation detail, but it's beautifully aligned with the language rule.

Under the Itanium ABI, constructors may have separate forms roughly corresponding to:

```text
Parent1 complete-object constructor

and

Parent1 base-subobject constructor
```

The ABI even gives them distinct mangling categories, conventionally `C1` and `C2`. ([Itanium C++ ABI][6])

Conceptually, imagine:

```cpp
// NOT real C++; conceptual generated code.

void Parent1_complete(Parent1* self) {
    // Parent1 is most-derived here.
    construct_Granny(correct_virtual_base_address(self), 101);

    Parent1_base(self, metadata_for_standalone_Parent1);
}

void Parent1_base(Parent1* self, ConstructionMetadata metadata) {
    // Do NOT construct Granny here.
    // Someone above us is responsible for it.

    install_metadata(metadata);

    self->p1 = 1;

    // Access already-constructed virtual Granny
    Granny* granny =
        self + metadata.granny_offset;

    // constructor body...
}
```

Then standalone:

```cpp
Parent1 p{1};
```

conceptually calls:

```text
Parent1_complete
    ↓
construct Granny
    ↓
Parent1_base
```

But:

```cpp
Son s;
```

conceptually does:

```text
Son complete constructor
    |
    +--> construct Granny once
    |
    +--> Parent1_base
    |
    +--> Parent2_base
    |
    +--> construct Son members
```

That is an exceptionally clean implementation of:

> **Only the most-derived constructor initializes virtual bases.**

C++ does not require compilers to implement constructors this way, but the Itanium ABI does distinguish complete-object and base-object constructor variants. ([Itanium C++ ABI][6])

---

# 17. VTT: why another structure exists during construction

Now suppose the compiler calls the `Parent1` base constructor from `Son`.

`Parent1` needs to know:

```text
I am Parent1-in-Son,
therefore Granny is +32 from me.
```

The Itanium ABI uses a **VTT — Virtual Table Table** to pass the appropriate construction-table addresses to base constructors.

Conceptually:

```text
VTT for Son

+--------------------------------------+
| final metadata for Son               |
+--------------------------------------+
| construction metadata Parent1-in-Son |
+--------------------------------------+
| construction metadata Parent2-in-Son |
+--------------------------------------+
| ...                                  |
+--------------------------------------+
```

Then:

```text
Son constructor
    |
    +--> Parent1_base(
             this = Parent1 address,
             metadata = Parent1-in-Son entry
         )

    +--> Parent2_base(
             this = Parent2 address,
             metadata = Parent2-in-Son entry
         )
```

The ABI explicitly says that complete-object constructors locate the VTT and pass the relevant sub-VTT entry to base-object constructors. ([Itanium C++ ABI][6])

Again, the reason here has nothing inherently to do with virtual-function dispatch.

It solves:

> **Where are my virtual bases in the complete object I'm currently part of?**

---

# 18. A complete mental construction trace

For:

```cpp
Son son;
```

keep this mental execution:

```text
Storage for complete Son exists
but subobject lifetimes haven't all begun yet.

                    ↓

Son is most-derived.

                    ↓

Find all virtual bases reachable from Son.

                    ↓

Construct shared Granny once:
Granny(999)

                    ↓

Construct Parent1 subobject.

Parent1's Granny(101) initializer is ignored.

Parent1 receives/uses layout metadata saying:
"Granny is at the Son-specific location."

Parent1 initializes p1.

Parent1 constructor body runs.

                    ↓

Construct Parent2 subobject.

Parent2's Granny(202) initializer is ignored.

Its metadata points to the SAME Granny,
but with a different relative offset.

Parent2 initializes p2.

Parent2 constructor body runs.

                    ↓

Initialize Son::s.

                    ↓

Execute Son constructor body.

                    ↓

Complete Son lifetime/construction state.
```

That is the mechanism I want you to be able to derive.

---

# 19. Important terminology correction

Sometimes explanations, including informal explanations of virtual inheritance, say:

> “Son constructs Granny directly.”

The intended idea is correct, but technically `Granny` has not suddenly become a **direct base class** of `Son`.

In:

```cpp
struct Son : Parent1, Parent2 {};
```

the direct bases are still only:

```text
Parent1
Parent2
```

`Granny` is an **indirect virtual base**.

The special rule is that the **most-derived constructor initializes virtual bases**, including indirect ones.

That's a better interview formulation.

---

# 20. What if `Son` doesn't mention `Granny`?

Consider:

```cpp
struct Granny {
    Granny(int);
};

struct Parent1 : virtual Granny {
    Parent1() : Granny(1) {}
};

struct Parent2 : virtual Granny {
    Parent2() : Granny(2) {}
};

struct Son : Parent1, Parent2 {
    Son()
        : Parent1(),
          Parent2() {}
};
```

This is **ill-formed**.

Why?

When constructing `Son`, these are ignored:

```cpp
Parent1(): Granny(1)
Parent2(): Granny(2)
```

Therefore `Son` must initialize `Granny`.

Since it doesn't explicitly do so, C++ attempts:

```cpp
Granny()
```

But there is no default constructor.

Compilation fails.

cppreference explicitly calls out this rule: if the most-derived constructor doesn't specify a virtual base, that virtual base is default-initialized; if no accessible default constructor exists, the construction is ill-formed. ([Open Standards][7])

So:

```cpp
struct Son : Parent1, Parent2 {
    Son()
        : Granny(42),
          Parent1(),
          Parent2() {}
};
```

is required.

---

# 21. Constructor-list ordering is not construction ordering

You could perversely write:

```cpp
Son()
    : s(3),
      Parent2(2),
      Parent1(1),
      Granny(999) {
}
```

The actual order remains:

```text
Granny
Parent1
Parent2
s
body
```

because construction order is determined by the class hierarchy and declaration order, not by the textual order of the initializer list. ([Cppreference][2])

This is particularly important for virtual inheritance because the virtual base must already exist before any direct base constructor begins.

---

# 22. Mixed virtual and ordinary inheritance: a nasty edge case

Suppose:

```cpp
struct Parent1 : virtual Granny {};

struct Parent2 : Granny {};

struct Son : Parent1, Parent2 {};
```

Now don't say:

> “Granny is virtual, therefore there is one Granny.”

Wrong.

We have:

```text
Son
├── Parent1
│      \
│       shared virtual Granny #1
│
└── Parent2
       └── ordinary Granny #2
```

There are **two Granny subobjects**.

Virtual paths share one virtual-base subobject, but ordinary inheritance paths still produce ordinary independent subobjects.

cppreference gives exactly this kind of mixed example: two virtual paths share one base while a separate non-virtual path contributes another base subobject. ([Cppreference][1])

This is one of the best interview traps.

---

# 24. What if the virtual inheritance chain is deeper?

For example:

```cpp
struct GreatGranny {};

struct Granny : virtual GreatGranny {};

struct Parent1 : virtual Granny {};
struct Parent2 : virtual Granny {};

struct Son : Parent1, Parent2 {};
```

`Son` is the most-derived class, so it is ultimately responsible for all virtual-base subobjects reachable in its hierarchy.

Virtual bases are initialized before non-virtual bases, in the language-defined depth-first left-to-right traversal order, with each virtual base visited once. ([Cppreference][2])

The key principle remains:

```text
most-derived object decides the complete virtual-base graph
```

rather than each intermediate parent recursively constructing its own independent copies.

---

# 25. An upcast to Granny may now be runtime work

Ordinary inheritance:

```cpp
Parent1* p;
Granny* g = p;
```

might be approximately:

```asm
lea rax, [rdi + constant]
```

Virtual inheritance:

```cpp
Parent1* p;
Granny* g = p;
```

may require:

```asm
load metadata
load vbase offset
add offset
```

So this:

```cpp
static_cast<Granny*>(p)
```

can involve runtime address calculation even though there is **no virtual function involved at all**.

That's a useful performance fact for low-level interviews.

---

# 26. But the reverse `static_cast` is restricted

If:

```cpp
Parent1* -> Granny*
```

travels through a virtual base, trying to statically reverse it is fundamentally different.

Something like:

```cpp
Granny* g = ...;

// not a valid ordinary static downcast through virtual base
// Parent1* p = static_cast<Parent1*>(g);
```

is not allowed as the normal `static_cast` base-to-derived downcast when the base is virtual.

A `Granny` subobject is potentially shared by several branches, and its relationship to one particular derived subobject is no longer described by a simple inverse fixed offset.

Later, RTTI and `dynamic_cast` will give us a runtime mechanism for navigating polymorphic object graphs. We don't need that mechanism yet.

---

# 27. Destruction is the reverse story

If:

```cpp
Son son;
```

was constructed as:

```text
Granny
Parent1
Parent2
Son member
Son body
```

then destruction goes in reverse:

```text
Son destructor body
    ↓
Son members
    ↓
Parent2
    ↓
Parent1
    ↓
Granny
```

Most importantly:

> `Parent1` and `Parent2` must not independently destroy their shared Granny.

Exactly like construction, the complete/most-derived destruction machinery is responsible for destroying virtual bases once.

The Itanium ABI similarly distinguishes complete-object and base-object destructor behavior: base-object destruction excludes virtual bases, while complete-object destruction includes them. ([Itanium C++ ABI][6])

This is the destruction-side mirror of:

```text
complete constructor constructs virtual bases
base constructor doesn't
```

---

# 28. Constructor failure

Suppose construction is:

```text
Granny ✓
Parent1 ✓
Parent2 throws
Son not constructed
```

C++ destroys already successfully constructed subobjects in reverse order.

Conceptually:

```text
Parent2 construction fails
        ↓
destroy whatever Parent2 itself had successfully constructed
        ↓
destroy Parent1
        ↓
destroy Granny
```

There is still only one Granny, and it is destroyed exactly as part of unwinding the partially constructed complete object.

`Son`'s constructor body never executes because construction never gets that far.

---

# 29. A particularly obscure assignment edge case

Construction is well-defined: a shared virtual base is constructed once.

Assignment has a strange corner.

For:

```cpp
struct Granny {};

struct Parent1 : virtual Granny {};
struct Parent2 : virtual Granny {};

struct Son : Parent1, Parent2 {};
```

the standard does not require an implicitly-defined copy/move assignment operator to call the shared virtual-base assignment exactly once. Whether a virtual base reachable by multiple paths gets assigned more than once is unspecified. ([Cppreference][8])

This is obscure and not something I'd prioritize for an internship interview, but it is a real virtual-inheritance edge case worth knowing exists.

Construction/destruction do **not** have that uncertainty.

---

# 30. The model I want you to keep

For ordinary inheritance:

```text
Parent
  |
  | fixed structural relationship
  v
Granny

Parent* + compile-time offset -> Granny*
```

For virtual inheritance:

```text
             Granny
              /  \
             /    \
        Parent1  Parent2
             \    /
               Son
```

the **most-derived object owns the placement and lifetime of Granny**.

Therefore an intermediate class cannot assume:

```text
"Granny is always N bytes from me."
```

Instead:

```text
Parent1*
    |
    | consult current complete-object layout
    v
virtual-base offset
    |
    | pointer adjustment
    v
shared Granny*
```

And during construction:

```text
standalone Parent1
    |
    | Parent1 is most-derived
    v
Parent1 constructs Granny


Parent1-in-Son
    |
    | Son is most-derived
    v
Son already constructed Granny
    |
    v
Parent1 constructor merely finds and uses it
```

Under the Itanium ABI, that need for **“find my virtual base according to the layout of the current complete object”** is why virtual inheritance can give a class a vptr and virtual table even when the program contains **zero virtual functions**; why the tables contain `vbase_offset` entries; and why construction may require construction tables plus a VTT. ([Itanium C++ ABI][5])

That is the virtual-inheritance mechanism I would get completely comfortable with **before** introducing virtual functions. Once this part is solid, adding virtual dispatch to the same tables becomes much easier rather than mixing two unrelated uses of the word `virtual`.

[1]: https://en.cppreference.com/cpp/language/derived_class?utm_source=chatgpt.com "Derived classes - cppreference.com"
[2]: https://en.cppreference.com/cpp/language/constructor?utm_source=chatgpt.com "Constructors and member initializer lists - cppreference.com"
[3]: https://en.cppreference.com/cpp/language/initializer_list?utm_source=chatgpt.com "Constructors and member initializer lists - cppreference.com"
[4]: https://shaharmike.com/cpp/vtable-part3/ "C++ vtables - Part 3 - Virtual Inheritance | Shahar Mike's Web Spot"
[5]: https://itanium-cxx-abi.github.io/cxx-abi/abi.html?utm_source=chatgpt.com "Itanium C++ ABI"
[6]: https://itanium-cxx-abi.github.io/cxx-abi/abi.html "Itanium C++ ABI"
[7]: https://www.open-std.org/jtc1/sc22/wg21/docs/wp/html/oct97/special.html?utm_source=chatgpt.com "special"
[8]: https://en.cppreference.com/cpp/language/copy_assignment?utm_source=chatgpt.com "Copy assignment operator - cppreference.com"
