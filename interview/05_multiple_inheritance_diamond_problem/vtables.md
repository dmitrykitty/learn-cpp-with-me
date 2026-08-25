# Deep Dive: C++ vtables, Multiple Inheritance, Virtual Inheritance, Thunks, VTTs, and Construction vtables

I went through all four parts of Shahar Mike's series:

[Part 1 — Basics](https://shaharmike.com/cpp/vtable-part1/?utm_source=chatgpt.com)
[Part 2 — Multiple Inheritance](https://shaharmike.com/cpp/vtable-part2/?utm_source=chatgpt.com)
[Part 3 — Virtual Inheritance](https://shaharmike.com/cpp/vtable-part3/?utm_source=chatgpt.com)
[Part 4 — Compiler-Generated Code](https://shaharmike.com/cpp/vtable-part4/?utm_source=chatgpt.com)

The series is very useful for exactly what we are studying because it does not stop at "`virtual` means dynamic dispatch." It looks at actual Clang-generated object layouts, vtables, RTTI records, thunks, construction vtables, and VTTs using GDB. The author explicitly warns that all of this is **implementation-specific**, which is essential: the C++ language specifies behavior, not that an implementation must contain something literally called a `vptr` or `vtable`.

For the low-level parts below I will use the same general **Itanium C++ ABI model** used by Clang/GCC on common Unix-like x86-64 systems. I also checked the series against the current Itanium ABI specification. The ABI explicitly defines vtables as structures containing offsets, RTTI information and function pointers, and says that they are also used for locating virtual base subobjects.

---

# 1. First separate three different levels

Before doing anything else, keep these layers separate:

```text
C++ language
    |
    | specifies semantics
    v
"call the final overrider"
"there is one shared virtual base"
"most-derived class initializes virtual bases"

        ↓ implementation

Compiler + ABI
    |
    v
vptr
vtable
secondary vtable
thunk
vbase offset
vcall offset
construction vtable
VTT

        ↓ execution

CPU instructions
    |
    v
load pointer
add/subtract offsets
indirect call
jump
```

If an interviewer asks:

> Does every polymorphic C++ class contain a vptr?

The precise answer is not simply “yes.”

A better answer is:

> C++ does not prescribe vtables or vptrs. Mainstream implementations use them, and under the Itanium ABI a dynamic class is represented using virtual tables and virtual pointers.

That distinction matters throughout this report.

---

# 2. Part 1 — what a vtable actually is

Start with the simplest hierarchy:

```cpp
struct Parent {
    virtual void f();
    virtual void g();
};

struct Derived : Parent {
    void f() override;
};
```

Conceptually, under the ABI used in the article, a `Derived` object contains a hidden pointer:

```text
Derived object
+------------------+
| vptr ------------+----+
+------------------+    |
| Parent data      |    |
+------------------+    |
| Derived data     |    |
+------------------+    |
                         |
                         v
                  Derived vtable
```

The article demonstrates this directly with GDB: different `Parent` objects share the same vtable, different `Derived` objects share another vtable, and the inherited vptr in a `Derived` object points at the `Derived` table.

## 2.1 The vptr does not necessarily point to the beginning of the vtable

This is one of the most interesting details from Part 1.

The simplified table looks like:

```text
                 lower addresses

+---------------------------+
| offset-to-top             |   vptr[-2]
+---------------------------+
| RTTI / typeinfo pointer   |   vptr[-1]
+===========================+  <-- vptr points here
| virtual function #0       |   vptr[0]
+---------------------------+
| virtual function #1       |   vptr[1]
+---------------------------+
| ...                       |
+---------------------------+
```

The ABI calls the place where the object's vptr points the **virtual table address point**.

So when the blog observes:

```text
vtable for Derived + 16
```

on a 64-bit target, that makes sense:

```text
8 bytes offset-to-top
8 bytes RTTI pointer
--------------------
16 bytes

address point begins here
```

Part 1 shows exactly this layout for `Parent` and `Derived`: an offset field, a typeinfo pointer, then the virtual function entries. Overridden `Parent::Foo()` is replaced by `Derived::Foo()`, while the non-overridden virtual function still points to the parent's implementation.

The official ABI uses the name **offset-to-top**, rather than the article's `top_offset`. It is the displacement from the subobject containing that vptr back to the top of the complete object.

---

# 3. How a virtual call actually happens

Consider:

```cpp
Parent* p = new Derived;
p->f();
```

Ignoring optimizations, the caller conceptually does:

```text
1. p already points to the Parent subobject.

2. Read vptr from *p.

3. Read the function pointer from the correct vtable slot.

4. Pass p as `this`.

5. Indirectly call that function.
```

Very rough x86-64 pseudocode:

```asm
; rdi = p = this

mov rax, [rdi]        ; rax = vptr
mov rax, [rax + 0]    ; rax = virtual function slot
call rax              ; indirect call
```

If `f` is slot number two:

```asm
mov rax, [rdi]
mov rax, [rax + 16]
call rax
```

assuming 8-byte entries.

Compare that with a statically resolved call:

```asm
call Parent::non_virtual_function
```

The important difference is:

```text
non-virtual call:
target normally known by compiler

virtual call:
target may be loaded from object at runtime
```

But remember that optimization changes this dramatically. If the compiler proves the dynamic type, it may **devirtualize** the call and perhaps inline the function, eliminating the vtable access entirely.

---

# 4. What overriding really means at the ABI level

For:

```cpp
struct Parent {
    virtual void f();
    virtual void g();
};

struct Derived : Parent {
    void f() override;
};
```

you can picture:

```text
Parent vtable
+----------------+
| offset-to-top  |
| RTTI Parent    |
+================+
| Parent::f      |
| Parent::g      |
+----------------+

Derived vtable
+----------------+
| offset-to-top  |
| RTTI Derived   |
+================+
| Derived::f     |  <-- same slot
| Parent::g      |  <-- inherited implementation
+----------------+
```

That is why a caller compiled against only:

```cpp
Parent* p;
```

doesn't need to know anything about `Derived`.

It knows only:

> "`f` is at slot N."

The object's vptr determines whose implementation is stored at slot N.

That is the fundamental decoupling dynamic dispatch provides.

---

# 5. RTTI lives next to this mechanism

Part 1 also investigates the `typeinfo` structures. In the observed ABI, the vtable contains a pointer to RTTI immediately before its address point. Derived RTTI describes its relationship to its base RTTI.

This later supports operations such as:

```cpp
typeid(*p);
dynamic_cast<Derived*>(p);
```

The exact RTTI structures such as:

```text
__class_type_info
__si_class_type_info
__vmi_class_type_info
```

are ABI details, not C++ language objects you normally manipulate yourself.

But the conceptual relationship is useful:

```text
object
  |
  v
vptr
  |
  +----> virtual function entries
  |
  +----> RTTI describing dynamic type
```

---

# 6. Part 2 — why multiple inheritance suddenly requires multiple vptrs

Now we reach the part that directly connects to the work we just did.

Use:

```cpp
struct Mother {
    virtual void mother();
    int m;
};

struct Father {
    virtual void father();
    int f;
};

struct Child : Mother, Father {
    virtual void child();
    int c;
};
```

The layout observed in Part 2 is conceptually:

```text
Child
+------------------------+
| Mother vptr            |  <-- primary
+------------------------+
| Mother data            |
| padding                |
+------------------------+
| Father vptr            |  <-- secondary
+------------------------+
| Father data            |
+------------------------+
| Child data             |
+------------------------+
```

The article observes **two vptrs**, not three. The `Child` portion shares the primary table with one base, while `Father` needs a secondary polymorphic view.

Why can't we simply have one vptr?

Because this must work independently:

```cpp
void foo(Mother*);
void bar(Father*);

Child child;

foo(&child);
bar(&child);
```

`foo()` is compiled with knowledge of `Mother`.

`bar()` is compiled with knowledge of `Father`.

Neither needs to know that the real object is a `Child`.

Each base subobject therefore has to look like a valid instance of the ABI representation expected for that base.

---

# 7. Primary and secondary vtables

Assume this layout:

```text
Child @ 0x1000

0x1000    Mother subobject
           vptr_Mother

0x1010    Father subobject
           vptr_Father
```

Then:

```cpp
Child* pc = &child;

Mother* pm = pc;
Father* pf = pc;
```

could produce:

```text
pc = 0x1000
pm = 0x1000
pf = 0x1010
```

which is exactly the pointer-adjustment mechanism we studied previously.

The primary vtable may conceptually look like:

```text
Child / Mother table

offset-to-top = 0
RTTI = Child

Mother::mother
Child::child
...
```

while the secondary `Father-in-Child` table might contain:

```text
offset-to-top = -16
RTTI = Child

Father::father
...
```

Part 2 demonstrates exactly this pattern, including a negative offset-to-top for the secondary `Father` subobject.

Why `-16`?

Because from:

```text
Father subobject @ 0x1010
```

the complete `Child` begins at:

```text
0x1010 - 16 = 0x1000
```

So:

```text
offset-to-top = -16
```

---

# 8. The really important multiple-inheritance problem: overrides

Now change:

```cpp
struct Father {
    virtual void f();
};

struct Child : Mother, Father {
    void f() override;
};
```

Consider this function:

```cpp
void invoke(Father* p) {
    p->f();
}
```

and:

```cpp
Child c;
invoke(&c);
```

Before `invoke` is entered, the upcast has already produced:

```text
Father*
   |
   v

Child
+-------------+
| Mother      |
+-------------+
| Father <---- p
+-------------+
```

Therefore inside `invoke`:

```text
this = address of Father subobject
```

But the final overrider is:

```cpp
Child::f()
```

and compiled `Child::f()` conceptually wants:

```text
this = Child*
```

We have a mismatch:

```text
caller has:

Father* this
    |
    | wrong address for direct Child::f
    v

need:

Child* this
```

This is where **thunks** appear.

---

# 9. Non-virtual thunk

Part 2 shows that the `Father-in-Child` vtable does not necessarily point directly at `Child::f`.

Instead:

```text
Father secondary vtable
      |
      v
non-virtual thunk to Child::f
      |
      | adjust this
      v
Child::f
```

The article even disassembles the thunk. In its example the relevant operation adjusts `%rdi`, which carries `this`, by a fixed negative offset before calling the actual `Child` implementation.

Conceptually:

```asm
; rdi = Father* inside Child

sub rdi, 8
jmp Child::f
```

or:

```asm
add rdi, -8
call Child::f
ret
```

depending on generated code.

The crucial mental model is:

```text
Father*                   Child*
   |                         ^
   | fixed adjustment        |
   +-------------------------+
                             |
                         Child::f
```

### Why is it called a *non-virtual* thunk?

The name is confusing if you think it means the function call isn't virtual.

It means the required **`this` adjustment itself is non-virtual**, i.e. it can be expressed as a statically known constant.

The dispatch that got us to the thunk was still virtual.

---

# 10. Important refinement of the blog's explanation

The blog connects the adjustment with `top_offset`. Conceptually that value indeed describes how to reach the complete object from a secondary base.

However, don't conclude:

> Every non-virtual thunk loads `offset-to-top` from the vtable.

The actual disassembly in the article uses a constant adjustment encoded directly in the thunk.

For ordinary non-virtual inheritance:

```text
Father-in-Child offset
```

is known statically.

The ABI defines `offset-to-top` for other purposes too, most notably the ability to recover the top of the complete object from an arbitrary polymorphic base subobject; `dynamic_cast<void*>` specifically needs this.

So distinguish:

```text
offset-to-top
    = information stored in vtable

fixed thunk adjustment
    = may simply be embedded in machine code
```

They can represent numerically related facts without the thunk necessarily reading one from the other.

---

# 11. Part 3 — now virtual inheritance changes everything

The article uses approximately this hierarchy:

```cpp
struct Grandparent {
    virtual void grandparent_f();
    int g;
};

struct Parent1 : virtual Grandparent {
    virtual void p1_f();
    int p1;
};

struct Parent2 : virtual Grandparent {
    virtual void p2_f();
    int p2;
};

struct Child : Parent1, Parent2 {
    virtual void child_f();
    int c;
};
```

Conceptually:

```text
         Grandparent
         /         \
     virtual      virtual
       /             \
   Parent1          Parent2
       \             /
        \           /
            Child
```

There is **one shared `Grandparent` subobject** inside the complete `Child`. That is the core language guarantee of virtual inheritance.

The concrete Clang layout observed by the blog is:

```text
Child
+---------------------------+
| Parent1 vptr              |
+---------------------------+
| parent1_data              |
| padding                   |
+---------------------------+
| Parent2 vptr              |
+---------------------------+
| parent2_data              |
+---------------------------+
| child_data                |
+---------------------------+
| Grandparent vptr          |
+---------------------------+
| grandparent_data          |
| padding                   |
+---------------------------+
```

So the shared virtual `Grandparent` appears after the non-virtual part of `Child`, despite being conceptually at the top of the hierarchy.

Again: this exact placement is ABI-specific.

---

# 12. The central problem virtual inheritance creates

With ordinary inheritance:

```cpp
struct Parent : Base {};
```

the displacement:

```text
Parent* -> Base*
```

can normally be known from the static class layout.

If `Base` begins at offset `+8`:

```asm
lea rax, [rdi + 8]
```

works for every `Parent` object.

Now consider:

```cpp
struct Parent1 : virtual Grandparent {};
```

What is the offset from:

```text
Parent1*
```

to:

```text
Grandparent*
```

?

There is no single answer.

For a complete `Parent1`:

```text
Parent1
+-------------+
| Parent1     |
+-------------+
| Grandparent | perhaps +16
+-------------+
```

but for `Parent1` embedded in `Child`:

```text
Child
+-------------+
| Parent1     | +0
+-------------+
| Parent2     | +16
+-------------+
| Grandparent | +32
+-------------+
```

and in another most-derived class:

```text
OtherDerived
+-------------+
| X           |
| ...         |
| Parent1     |
| ...         |
| Grandparent |
+-------------+
```

the relative distance can be different again.

This is the fundamental reason virtual inheritance needs runtime layout information.

---

# 13. Virtual-base offsets

The Itanium ABI solves this by putting **virtual-base offsets (`vbase offsets`)** into virtual tables.

The ABI explicitly states that a vbase offset is added to the address of the subobject containing the relevant vptr to obtain the virtual base subobject.

So:

```cpp
Parent1* p = ...;
Grandparent* g = p;
```

is conceptually more like:

```text
Parent1*
    |
    | read vptr
    v
Parent1's currently active vtable
    |
    | read Grandparent vbase offset
    v
+32, +16, ... depending on actual layout
    |
    v
Parent1* + offset
    |
    v
Grandparent*
```

Rough pseudo-assembly:

```asm
; rdi = Parent1*

mov rax, [rdi]          ; vptr
mov rcx, [rax - K]      ; load vbase offset from known vtable position
add rdi, rcx            ; now rdi = Grandparent*
```

The important part isn't the exact `K`.

The important part is:

```text
non-virtual base:
offset can normally be immediate constant

virtual base:
offset may need to be loaded from runtime type-layout metadata
```

That is the major runtime cost of navigating virtual inheritance.

---

# 14. The actual offsets from the article

For the article's `Child`, the tables contain virtual-base offsets corresponding conceptually to:

```text
Parent1 -> Grandparent : +32

Parent2 -> Grandparent : +16

Grandparent -> Grandparent : 0
```

which makes sense given:

```text
Child start         +0
Parent1             +0
Parent2             +16
Grandparent         +32
```

The blog's dumped `Child` table contains these virtual-base-offset values together with different negative offset-to-top values for the different subobjects.

Now the same virtual base can be reached through both paths:

```cpp
Parent1* p1 = &child;
Parent2* p2 = &child;

Grandparent* g1 = p1;
Grandparent* g2 = p2;

assert(g1 == g2);
```

but:

```text
Parent1 -> Grandparent
```

and:

```text
Parent2 -> Grandparent
```

require different pointer adjustments.

---

# 15. `vbase offset` versus `offset-to-top`

These are related but represent different directions.

Suppose:

```text
Child start       = 0
Parent1           = 0
Parent2           = 16
Grandparent       = 32
```

For `Parent2`:

```text
offset-to-top = -16

Parent2*
   |
   | -16
   v
Child*
```

For its virtual `Grandparent`:

```text
vbase offset = +16

Parent2*
   |
   | +16
   v
Grandparent*
```

So:

```text
offset-to-top
    tells us how to move toward complete-object top

vbase offset
    tells us how to move toward a specific virtual base
```

Do not conflate them.

---

# 16. Construction now becomes surprisingly hard

This is, in my opinion, the deepest part of Part 3.

The C++ language says the **most-derived class initializes virtual bases**. Virtual bases are initialized before non-virtual direct bases.

Therefore constructing:

```cpp
Child child;
```

conceptually follows:

```text
Grandparent virtual base
        ↓
Parent1
        ↓
Parent2
        ↓
Child members
        ↓
Child constructor body
```

Suppose constructors contain:

```cpp
Parent1() : Grandparent(1) {}

Parent2() : Grandparent(2) {}

Child() : Grandparent(3), Parent1(), Parent2() {}
```

For a complete `Child`, `Grandparent(3)` wins.

The `Grandparent(...)` initializers inside `Parent1` and `Parent2` do not initialize the virtual base in that situation.

But:

```cpp
Parent1 p;
```

is different.

Now `Parent1` itself is most-derived, so `Parent1` is responsible for constructing `Grandparent`.

This is why an intermediate class constructor must work in multiple contexts:

```text
Parent1 complete object

Parent1-in-Child

Parent1-in-SomeOtherDerived
```

and the position of `Grandparent` can differ among them.

---

# 17. Why an ordinary `Parent1` vtable is not enough during construction

Imagine executing the `Parent1` constructor while constructing a `Child`.

The constructor may do:

```cpp
grandparent_data = 42;
```

or call something that requires finding the virtual `Grandparent`.

The standalone `Parent1` vtable might say:

```text
Grandparent is +16 from Parent1
```

but in a `Child`, it might actually be:

```text
Grandparent is +32 from Parent1
```

So standalone `Parent1` metadata is wrong.

Could we simply use `Child`'s final vtable?

No.

During execution of the `Parent1` constructor, virtual dispatch must behave as if the object is currently a `Parent1`, not as a fully constructed `Child`.

Therefore we need contradictory-looking information:

```text
Virtual dispatch identity:
    Parent1

Physical virtual-base layout:
    Parent1-in-Child
```

This is exactly what **construction vtables** solve.

---

# 18. Construction vtable

A construction vtable is essentially a temporary ABI view appropriate for:

```text
"Parent1 while it is being constructed as part of Child"
```

The official Itanium ABI describes this explicitly: a construction virtual table contains function addresses, RTTI and offset-to-top information associated with the base being constructed, but virtual-base offsets and adjustment information appropriate to the layout of the complete derived object.

That is a beautiful mechanism.

Conceptually:

```text
Normal Parent1 vtable
---------------------
identity: Parent1
layout:   standalone Parent1


Final Child vtable
------------------
identity: Child
layout:   Child


Construction Parent1-in-Child vtable
------------------------------------
identity/dispatch: Parent1
virtual-base layout: Child
```

So during `Parent1` construction:

```text
Parent1's vptr
       |
       v
construction vtable for Parent1-in-Child
```

Then when its constructor needs the shared `Grandparent`, it sees the correct:

```text
+32
```

rather than the offset from standalone `Parent1`.

Part 3 dumps these construction vtables explicitly for both `Parent1-in-Child` and `Parent2-in-Child`.

---

# 19. VTT — Virtual Table Table

Now we need some way of choosing which vtable or construction vtable should be installed while constructing each part of the object.

Enter the **VTT**.

The blog describes VTT as a “virtual-table table.” Its example contains pointers to the final `Child` tables and to the construction tables for `Parent1-in-Child`, `Parent2-in-Child`, and the appropriate `Grandparent` views.

A useful conceptual picture is:

```text
VTT for Child
+----------------------------------+
| final Child / Parent1 address pt |
+----------------------------------+
| Parent1-in-Child construction VT |
+----------------------------------+
| Grandparent in Parent1 ctor view |
+----------------------------------+
| Parent2-in-Child construction VT |
+----------------------------------+
| Grandparent in Parent2 ctor view |
+----------------------------------+
| final Grandparent table          |
+----------------------------------+
| final Parent2 table              |
+----------------------------------+
```

The exact entries/order follow ABI rules.

The important point is:

> **VTT is not another pointer stored in every object.**

It is compiler-generated static metadata used during construction/destruction.

The Itanium ABI says the complete-class constructor passes appropriate locations in the VTT to base constructors that need them, allowing those constructors to install the right construction vptrs.

This explains how one compiled implementation of:

```cpp
Parent1::Parent1()
```

can work both for:

```cpp
Parent1 p;
```

and:

```cpp
Child c;
```

without knowing at source level which most-derived class is being created.

One correction to keep in mind: the article observed its VTT placed immediately after the main vtable in that binary. Don't treat physical adjacency as a portable guarantee. The current ABI specification describes the VTT and construction tables as generated structures and even notes that the relevant structures can be emitted “in no particular order.”

---

# 20. The complete construction picture

For:

```text
          Grandparent
          /         \
     virtual       virtual
        /             \
    Parent1          Parent2
        \             /
            Child
```

think of construction approximately as:

```text
Allocate storage for complete Child

        ↓

Construct shared Grandparent
using its appropriate construction state

        ↓

Set Parent1 vptr to
Parent1-in-Child construction vtable

        ↓

run Parent1 constructor

        ↓

Set Parent2 vptr to
Parent2-in-Child construction vtable

        ↓

run Parent2 constructor

        ↓

Install final Child/secondary/virtual-base vptrs

        ↓

initialize Child members

        ↓

run Child constructor body
```

This is a conceptual implementation model, not literal C++ abstract-machine pseudocode, but it explains the ABI machinery.

---

# 21. One level beyond the blog: `vcall` offsets

The series goes deeply into **vbase offsets**, but the full Itanium model also has another important field:

```text
vcall offset
```

You should know the distinction.

A `vbase offset` solves:

> “Given this subobject, where is its virtual base?”

A `vcall offset` helps solve:

> “I am making a virtual call through a virtual-base subobject. Where is the subobject corresponding to the final overrider's expected `this`?”

Suppose:

```cpp
struct V {
    virtual void f();
};

struct A : virtual V {
    void f() override;
};

struct D : A {};
```

If you call:

```cpp
V* p = new D;
p->f();
```

the `V*` points to the shared virtual-base subobject.

But the final overrider may require an `A*` or `D*`-appropriate `this`.

Unlike the ordinary `Father -> Child` case, this adjustment is not necessarily a single constant valid for every most-derived object containing the virtual base.

The Itanium ABI therefore supports **vcall offsets** associated with virtual bases; an adjusting entry point can load the necessary runtime adjustment and then transfer control to the real overrider.

So we now have:

```text
ordinary secondary base override:

Father*
   |
   | fixed constant
   v
Child*
   |
   v
Child::f

=> non-virtual thunk
```

versus potentially:

```text
virtual base override:

VirtualBase*
    |
    | load vcall offset
    | from active vtable
    v
correct overrider this
    |
    v
Derived::f

=> virtual adjustment/thunk
```

This difference is very interview-worthy.

---

# 22. Part 4 — why virtual calls change during construction

The fourth article moves from table layout to compiler-generated behavior.

It considers:

```text
Parent
  ↓
Child
  ↓
Grandchild
```

where every class overrides `Foo()` and calls `Foo()` in its constructor.

Constructing:

```cpp
Grandchild g;
```

prints conceptually:

```text
Parent
Child
Grandchild
```

not:

```text
Grandchild
Grandchild
Grandchild
```

The blog explains this through successive changes of the active vptr during construction.

This behavior is not merely an ABI accident. It is required by C++ semantics: during construction or destruction, a virtual call dispatches to the final overrider in the class whose constructor/destructor is currently executing, not to an override in a more-derived class whose lifetime has not yet begun or has already ended.

So:

```text
Parent constructor running
    ↓
dynamic dispatch limited to Parent hierarchy

Child constructor running
    ↓
dynamic dispatch limited to Child hierarchy

Grandchild constructor running
    ↓
Grandchild now participates
```

The vptr changes are an implementation mechanism that naturally enforces this semantic rule.

---

# 23. Destruction is the mirror image

During destruction:

```text
~Grandchild
    ↓
~Child
    ↓
~Parent
```

As each more-derived portion disappears, virtual dispatch is correspondingly restricted.

Conceptually:

```text
inside ~Grandchild:
vptr behaves as Grandchild

then Grandchild part dies

inside ~Child:
vptr behaves as Child

then Child part dies

inside ~Parent:
vptr behaves as Parent
```

That prevents a base destructor from dispatching into an already-destroyed derived subobject.

This is why “don't call virtual methods from constructors/destructors” is too simplistic as a rule.

They **can** be called.

You just need to understand that their dynamic-dispatch behavior is intentionally restricted.

---

# 24. Important correction to Part 4: pure virtual calls

The article informally says that calling an unimplemented pure virtual function during construction will probably result in a segfault or runtime failure.

At the language level, the stronger statement is:

> **Making a virtual call to a pure virtual function from the constructor or destructor of the abstract class is undefined behavior.**

cppreference states this explicitly, and the current draft's UB annex says the same.

You should not reason:

```text
pure virtual call
    =>
always __cxa_pure_virtual
    =>
always crash
```

An implementation may produce such behavior, but C++ doesn't guarantee it.

---

# 25. `dynamic_cast` and RTTI

Part 4 connects `dynamic_cast` to the RTTI structures introduced in Part 1.

For example:

```cpp
Parent* p = ...;

Derived* d = dynamic_cast<Derived*>(p);
```

cannot generally be reduced to one fixed pointer adjustment because the runtime must determine:

* the dynamic type,
* whether the requested relationship exists,
* whether it is public,
* whether it is unambiguous,
* and which target subobject is correct.

In an Itanium-style implementation, RTTI associated with the vtable provides the inheritance graph information needed for this search.

And:

```cpp
dynamic_cast<void*>(p)
```

is particularly connected to **offset-to-top** because its result is a pointer to the most-derived object. The ABI explicitly identifies this as a reason `offset-to-top` is always present.

---

# 26. This connects directly to the pointer-to-member-function issue we just studied

Part 4 mentions that a pointer to a virtual member function still preserves virtual dispatch.

Now we can make that precise.

Under the Itanium ABI, a member-function pointer is conceptually represented as two fields:

```cpp
struct conceptual_member_pointer {
    function_information ptr;
    std::ptrdiff_t adj;
};
```

The ABI specifies essentially this two-word representation. For a non-virtual member, `ptr` can hold the actual function pointer. For a virtual member, the standard Itanium representation can encode **the vtable entry offset rather than the final implementation address**. `adj` stores the adjustment applied to `this`.

This explains your earlier experiment:

```cpp
sizeof(void (*)())        // perhaps 8
sizeof(void (B::*)())     // perhaps 16
```

on an Itanium-ABI x86-64 system.

For:

```cpp
void (B::*pf)() = &B::non_virtual;
```

conceptually:

```text
pf.ptr = B::non_virtual address
pf.adj = perhaps 0
```

For:

```cpp
void (B::*pf)() = &B::virtual_f;
```

conceptually:

```text
pf.ptr = information identifying virtual slot N
pf.adj = this adjustment
```

Then:

```cpp
(obj.*pf)();
```

works roughly as the ABI specifies:

```text
1. Adjust `this` using pf.adj.

2. If pf represents a virtual slot:
       load adjusted_this->vptr
       fetch slot encoded by pf.ptr
       call function in that slot.

3. Otherwise:
       directly/indirectly call function stored in pf.ptr.
```

The official ABI gives essentially exactly this call algorithm.

This is why:

```cpp
Base b;
Derived d;

void (Base::*pf)() = &Base::virtual_f;

(d.*pf)();
```

can still reach:

```cpp
Derived::virtual_f
```

even though `pf` was created from `&Base::virtual_f`.

The member pointer identifies the **virtual dispatch slot**, not permanently the `Base` implementation.

---

# 27. Part 4's final “WTF” example

The article finishes with two interfaces:

```cpp
struct FooInterface {
    virtual ~FooInterface() = default;
    virtual void Foo() = 0;
};

struct BarInterface {
    virtual ~BarInterface() = default;
    virtual void Bar() = 0;
};

struct Concrete : FooInterface, BarInterface {
    void Foo() override;
    void Bar() override;
};
```

Then:

```cpp
FooInterface* foo = &c;
```

points to the first subobject.

A proper:

```cpp
BarInterface* bar = &c;
```

would require pointer adjustment:

```text
Concrete start
      |
      | + Bar offset
      v
BarInterface subobject
```

But the article deliberately does something dangerous:

```cpp
BarInterface* bar = (BarInterface*)foo;
```

This does **not** perform a safe sibling-base navigation.

The resulting pointer can still contain the address of the `FooInterface` subobject while being statically typed as `BarInterface*`.

Then:

```cpp
bar->Bar();
```

interprets the memory as though it were a real `BarInterface`.

At the ABI level, the call uses the vptr physically located there — which is actually the `FooInterface`/primary vptr. Because the virtual slot layouts happen to line up in the demonstrated binary, the lookup lands on `Concrete::Foo()`, producing the surprising output described by the article.

But this should **not** be understood as valid C++ behavior worth relying on.

The pointer does not correctly designate a `BarInterface` subobject, so using it that way leads outside the guarantees of the language.

The proper polymorphic side-cast is:

```cpp
BarInterface* bar = dynamic_cast<BarInterface*>(foo);
```

which uses RTTI to find the actual `BarInterface` subobject and returns the correctly adjusted pointer.

This is a fantastic example because it shows that:

```text
"same complete object"
```

does **not** imply:

```text
"all base-class pointers have the same address"
```

---

# 28. Put all pointer adjustments together

This is the central picture I want you to remember.

| Operation                            | How the target address can be found                                                                 |
| ------------------------------------ | --------------------------------------------------------------------------------------------------- |
| `Derived* -> ordinary Base*`         | Usually compile-time constant adjustment                                                            |
| secondary base virtual override      | Fixed `this` adjustment; often a non-virtual thunk                                                  |
| `Derived* -> virtual Base*`          | May load a `vbase offset` from the active vtable                                                    |
| virtual call through virtual base    | May require a runtime `vcall offset` / adjusting thunk                                              |
| arbitrary polymorphic side/down cast | RTTI-driven `dynamic_cast`                                                                          |
| pointer-to-member invocation         | Apply member-pointer `this` adjustment, then direct or virtual dispatch depending on encoded member |

This is a much better understanding than simply saying:

> “Multiple inheritance uses pointer offsets.”

There are **different kinds of offsets solving different problems**.

---

# 29. What information exists per object and what exists only once per class

Another very important performance distinction:

```text
Per object
----------
vptr(s)
ordinary members
base subobjects


Static compiler-generated metadata
----------------------------------
vtable groups
secondary vtables
RTTI records
construction vtables
VTT
thunks as machine-code functions
```

A VTT is therefore not something that adds another pointer to every `Child`.

Likewise, every object does not contain its entire vtable.

Objects generally contain only pointer(s) to shared static tables.

This matters when discussing space cost.

---

# 30. Runtime cost model

For interview purposes, separate the possible costs.

## Ordinary virtual dispatch

Potentially:

```text
load vptr
load function target
indirect branch
```

plus normal function-call cost.

The important performance issue is often not the two loads themselves but:

* inability to inline if not devirtualized,
* indirect-branch prediction,
* instruction/cache effects,
* resulting loss of optimization opportunities.

But if the compiler proves the dynamic type:

```text
virtual call
    ↓
devirtualization
    ↓
direct call
    ↓
possibly inline
```

the runtime dispatch cost may disappear entirely.

## Ordinary multiple inheritance

Potential costs:

```text
additional vptrs in secondary polymorphic bases
fixed pointer adjustment
adjusting thunk for some overridden calls
```

A constant adjustment such as:

```asm
add rdi, -16
```

is tiny by itself.

## Virtual inheritance

Potentially:

```text
additional object-layout complexity
additional vptr(s), depending on hierarchy/ABI
runtime load of virtual-base offset
pointer addition
more complex thunks for some virtual calls
construction vtables/VTT static metadata
more complex construction/destruction
```

So saying:

> “Virtual inheritance costs one pointer.”

is not generally correct.

And saying:

> “Virtual inheritance makes every method call slower.”

is also not correct.

The cost depends on which operation is being performed.

---

# 31. Why virtual inheritance exists despite all this complexity

Because it solves a real semantic problem.

Without virtual inheritance:

```text
        A
       / \
      B   C
       \ /
        D

D contains:

B::A
C::A
```

Two separate states.

With:

```cpp
struct B : virtual A {};
struct C : virtual A {};
```

the complete object has:

```text
        A
       / \
      B   C
       \ /
        D
```

but physically:

```text
D
├── B
├── C
└── shared A
```

Now:

```cpp
B* b = &d;
C* c = &d;

A* a1 = b;
A* a2 = c;

assert(a1 == a2);
```

Both paths reach one state.

The standard iostream hierarchy is the classic real example: `std::istream` and `std::ostream` virtually derive from `std::basic_ios`, allowing `std::iostream` to contain one shared `basic_ios` subobject rather than two independent copies.

---

# 32. The hierarchy of complexity

You can now mentally arrange everything we've studied:

```text
LEVEL 1
non-virtual member function

known function
known this
    ↓
direct call


LEVEL 2
single-inheritance virtual function

this
 ↓
vptr
 ↓
slot
 ↓
indirect call


LEVEL 3
ordinary multiple inheritance

derived pointer
 ↓
fixed adjustment
 ↓
base subobject
 ↓
vptr
 ↓
slot / possible thunk
 ↓
adjust this
 ↓
final overrider


LEVEL 4
virtual inheritance

subobject pointer
 ↓
vptr
 ↓
runtime vbase/vcall offset
 ↓
shared virtual base / correct overrider subobject
 ↓
possibly thunk
 ↓
final function


LEVEL 5
construction/destruction with virtual inheritance

most-derived object layout
 +
temporarily restricted dynamic dispatch
    ↓
construction vtables
    ↓
VTT
    ↓
correct temporary vptrs
```

If you can derive these five levels rather than memorize them, you understand the mechanism.

---

# 33. A useful experiment to reproduce the article yourself

Take this hierarchy:

```cpp
#include <iostream>

struct A {
    virtual void a() {}
    int x = 1;
};

struct B : virtual A {
    virtual void b() {}
    int y = 2;
};

struct C : virtual A {
    virtual void c() {}
    int z = 3;
};

struct D : B, C {
    void a() override {}
    virtual void d() {}
    int q = 4;
};

int main() {
    D obj;

    D* pd = &obj;
    B* pb = pd;
    C* pc = pd;
    A* pab = pb;
    A* pac = pc;

    std::cout << static_cast<void*>(pd) << '\n';
    std::cout << static_cast<void*>(pb) << '\n';
    std::cout << static_cast<void*>(pc) << '\n';
    std::cout << static_cast<void*>(pab) << '\n';
    std::cout << static_cast<void*>(pac) << '\n';

    std::cout << std::boolalpha << (pab == pac) << '\n';
}
```

Before running it, predict:

```text
D address
B address
C address
A-through-B address
A-through-C address

A-through-B == A-through-C ?
```

Then inspect the compiler's actual class/vtable layout.

With Clang, useful compiler/debugging tools include record-layout dumps, symbol inspection and disassembly. The blog itself uses GDB memory inspection throughout the series.

Typical commands worth experimenting with are:

```bash
clang++ -std=c++20 -O0 -g -fno-inline \
    -Xclang -fdump-record-layouts \
    -Xclang -fdump-vtable-layouts \
    main.cpp -o main
```

Then:

```bash
nm -C main | grep -E 'vtable|VTT|thunk|typeinfo'
```

and:

```bash
objdump -Cd main
```

or:

```bash
gdb ./main
```

Do the first experiments at `-O0`. Afterwards compile with `-O2` and notice how much of the apparently complicated machinery can disappear when the optimizer knows enough.

---

# 34. What I would expect you to explain in an interview after studying this

You should be able to derive answers to questions such as:

1. **What is a vtable?**
   An implementation structure commonly used for dynamic dispatch and, under ABIs such as Itanium, RTTI and virtual-base navigation. It is not mandated by the C++ standard.

2. **What is stored in a vtable?**
   Depending on the ABI and hierarchy: virtual function entries, RTTI pointer, offset-to-top, vbase offsets, vcall offsets, and potentially adjusting function entry points.

3. **Why can one object contain multiple vptrs?**
   Because separate polymorphic base subobjects must independently present the representation expected by code operating through those base types.

4. **Why does `Derived* -> Base*` sometimes change the numerical address?**
   The base subobject may begin at a nonzero offset inside the complete derived object.

5. **Why does an override through a secondary base sometimes need a thunk?**
   The caller passes a pointer to the secondary base subobject, while the final overrider expects a different `this`; the thunk adjusts it first.

6. **Why can't the offset to a virtual base always be compile-time constant?**
   Its location relative to an intermediate base can depend on the most-derived class.

7. **What is a vbase offset?**
   Runtime metadata used to locate a virtual base from a particular subobject.

8. **What is offset-to-top?**
   The displacement from a subobject's vtable-pointer location back to the complete object.

9. **What is a vcall offset?**
   Runtime adjustment information used when dispatch through a virtual base requires converting the incoming `this` to the subobject expected by the final overrider.

10. **Why do construction vtables exist?**
    During a base constructor we need the base's restricted virtual-dispatch semantics but the virtual-base offsets corresponding to the final complete object's physical layout.

11. **What is a VTT?**
    Compiler-generated metadata containing appropriate virtual-table address points used during construction/destruction of hierarchies involving virtual bases.

12. **Who constructs a virtual base?**
    The most-derived constructor.

13. **Why doesn't a base constructor dispatch to a derived override?**
    During construction, more-derived portions are not yet considered active for virtual dispatch.

14. **What does a pointer-to-member for a virtual function contain?**
    ABI-specific information; in the Itanium representation it may encode a vtable slot offset plus a `this` adjustment rather than just a raw implementation address.

15. **What is the performance cost of virtual inheritance?**
    Potential extra space/layout metadata, runtime address calculation for virtual bases, potentially more complicated adjusting calls, and construction complexity. The exact costs depend on the ABI and code path.

---

# 35. The single most important mental picture

Don't memorize the giant GDB dumps from Part 3.

Remember **why every field has to exist**:

```text
vptr
    "Which runtime view of this subobject is active?"

virtual-function slot
    "Which implementation should I call?"

offset-to-top
    "Where is the complete object?"

vbase offset
    "Where is my shared virtual base?"

vcall offset
    "How do I adjust this from a virtual-base view
     to the subobject expected by the final overrider?"

thunk
    "Transform this, then transfer control."

construction vtable
    "While constructing Base-in-Derived, use Base's
     dispatch semantics but Derived's physical layout."

VTT
    "Which temporary/final vtable should each subobject
     use at this phase of construction?"
```

Once those questions are clear, vtables stop looking like compiler magic. They become a collection of very practical answers to one fundamental problem:

> **A pointer may designate only one subobject of a larger object, while C++ must still locate other subobjects, determine the dynamic type, adjust `this`, and invoke the correct final overrider without the caller knowing the complete concrete type.**

That is the real mechanism behind the entire four-part series.
