# Jak przerabiamy każdy temat

Standard dla każdego dnia:

1. **Teoria** — czytasz wskazane fragmenty.
2. **Rozmowa ze mną** — tłumaczymy mechanizmy głębiej, jeżeli coś jest niejasne.
3. **Interview questions** — pytam bez podpowiadania.
4. **Praktyka** — implementacja, problem algorytmiczny, debugging albo design.
5. **Follow-ups** — schodzimy poziom głębiej.
6. **Podsumowanie** — masz umieć w 2–5 minut wyjaśnić temat po angielsku.

Temat jest zaliczony dopiero, kiedy potrafisz **bez notatek wyjaśnić najważniejsze mechanizmy i obronić swoje decyzje**. Nie chodzi o zapamiętywanie definicji.

Do C++ będziemy regularnie używać cppreference jako **reference**, a nie podręcznika; zawiera ono osobne sekcje m.in. o lifetime, ODR, UB, initialization, inheritance, virtual functions, templates, concepts i special member functions. ([Cppreference][2]) Dla systemów OSTEP będzie szczególnie wygodne, bo autorzy udostępniają bezpłatnie poszczególne rozdziały, w tym Processes, Address Spaces, Paging, TLB, Threads, Locks, Condition Variables i Concurrency Bugs. ([UW Computer Sciences][3]) Networking będziemy podpierać m.in. Beej's Guide, który jest bezpośrednio nastawiony na programowanie socketów w C/C++. ([Beej][4])

---

# BLOK I — MODERN C++ I OBJECT MODEL


## Aktualna kolejność nauki — synchronizacja z kursem C++

Kolejność w **Bloku I** jest celowo ustawiona tak, aby materiał z kursu C++ i przygotowanie interview wzajemnie się wzmacniały, zamiast wprowadzać kilka nowych mechanizmów naraz. Po Topics 1–2 przechodzimy przez klasy i dziedziczenie, następnie virtual dispatch / virtual inheritance, exceptions i RTTI, potem templates, move semantics, smart pointers i dopiero później perfect forwarding.

**Status:** Topics 1–2 są zakończone. Smart pointers zostały już wstępnie omówione, ale temat nie jest zaliczony; wracamy do niego w nowej kolejności po templates i move semantics. **Następny pełny temat: Constructors, destructors i Rule of 0/3/5.**

---

## 1. C++ objects: storage, initialization i lifetime

**Źródła**

* *A Tour of C++, 3rd ed.* — Basics, Classes, Essential Operations
* cppreference: `Object`, `Lifetime`, `Storage duration`, `Initialization`
* opcjonalnie *Effective Modern C++* — Item 7

**Musimy zrozumieć**

* object vs storage,
* automatic/static/thread/dynamic storage duration,
* kiedy lifetime obiektu faktycznie się zaczyna i kończy,
* initialization vs assignment,
* default/value/direct/list initialization,
* temporaries,
* dangling objects/references,
* UB związane z lifetime.

**Praktyka**
Dostaniesz kilkanaście fragmentów kodu i dla każdego określisz: gdzie znajduje się obiekt, kiedy powstaje, kiedy umiera i czy kod ma UB.

**Interview target**
Umieć odpowiedzieć głębiej niż „stack variable disappears when function returns”.

---

## 2. Pointers, references, arrays, `const` i casts

**Źródła**

* *A Tour of C++* — Basics
* cppreference: Pointers, References, Arrays, Implicit conversions, Casts
* C++ Core Guidelines — pointer/resource rules

**Zakres**

* `T*`, `T&`, `T&&`,
* `const T*`, `T* const`,
* pointer arithmetic,
* array decay,
* null pointers,
* aliasing,
* `static_cast`, `const_cast`, `reinterpret_cast`,
* `dynamic_cast` tylko jako zapowiedź — pełny mechanizm przeniesiony do RTTI po virtual functions,
* kiedy cast oznacza potencjalny design smell.

**Praktyka**
Operacje na surowym buforze oraz kilka zadań typu „what does this code do / is this UB?”.

---

## 3. Constructors, destructors i Rule of 0/3/5

**Źródła**

* *A Tour of C++* — Classes, Essential Operations
* *Effective Modern C++* — Item 17
* cppreference: special member functions, Rule of three/five/zero

**Zakres**

* compiler-generated special members,
* copy constructor/assignment,
* move constructor/assignment,
* destructor,
* `=default`,
* `=delete`,
* initialization order,
* podstawowa intuicja: co oznacza nieudana konstrukcja; pełny mechanizm wyjątków i stack unwinding omawiamy później.

**Praktyka**
Rozbieramy Twój `String` i odpowiadamy, które special members compiler wygenerowałby automatycznie i dlaczego.

---

## 4. Inheritance: object model, lookup, accessibility and slicing

Public/protected/private inheritance; base subobjects; accessibility versus visibility; name lookup and hiding; using Base::f; constructor/destructor order; casts in hierarchies; slicing. These are the first four items of Ilia's inheritance section.

Quant/interview emphasis

You should be able to reason about actual object layout rather than say merely "Derived inherits Base."

Typical follow-ups:

Is Derived* -> Base* always just a reinterpretation of the same address?
What happens when Base isn't the first base?
Why does Derived::f(int) hide Base::f(int,int)?
Name lookup vs overload resolution?
What exactly gets sliced in Base b = derived;?
Does slicing invoke a copy constructor?
Construction/destruction order and why it cannot depend on initializer-list order.

Literature

A Tour of C++: Classes / Class Hierarchies.
cppreference: Derived classes, Member name lookup, Overload resolution.

Practice

Build several small hierarchies and print:

sizeof
addresses of complete object/base subobjects
constructor/destructor traces.

Then implement an API where passing by value accidentally slices a polymorphic object and diagnose it.
Existing-code debugging / API extension. Find slicing, hiding and ownership bugs in a supplied codebase. IMC/HRT style.

---

## 5. Multiple inheritance and virtual inheritance


Multiple inheritance, ambiguity, multiple base subobjects, diamond inheritance, virtual bases, construction of virtual bases. Ilia also places multiple and virtual inheritance before the virtual-function section.

Quant/interview emphasis

This is really an object layout + pointer adjustment chapter.

You must be able to draw:

        A
       / \
      B   C
       \ /
        D

for both ordinary and virtual inheritance and answer:

How many A subobjects exist?
Why can B* and C* pointing into the same D have different numerical addresses?
Who constructs the virtual base?
Why does the most-derived class initialize virtual bases?
What can virtual inheritance cost in space and address calculation?

Practice

Write a hierarchy explorer that prints addresses after:

D* -> B*
D* -> C*
B* -> A*
C* -> A*

Compare ordinary versus virtual diamond inheritance.
C++ hierarchy/layout exercise + one arrays/hash OA problem as independent timed practice.
---

## 6. Virtual functions and runtime polymorphism

Only now do we introduce dynamic dispatch.

Ilia material

Virtual functions, overriding, more complex dispatch cases, abstract classes, pure virtual functions, virtual destructors and corner cases.

Interview emphasis

static type vs dynamic type;
overriding vs hiding;
covariant returns;
override / final;
pure virtual functions;
pure virtual destructor;
virtual calls inside constructors/destructors;
default arguments of virtual functions;
deleting through Base*;
when a virtual destructor is necessary.

A particularly important question:

What does a virtual call cost?

For now we answer semantically. Implementation comes next.

Practice

Implement a small pricing/order-processing hierarchy twice:

conventional runtime polymorphism;
deliberately broken version without virtual destruction.

Instrument construction, dispatch and destruction.
Implement an extensible event/market-message handler. Follow-up: add message types without breaking old code.

---

## 7. RTTI, dynamic_cast, typeid, vptrs and vtables

This comes immediately after virtual functions, as you requested.

Ilia likewise places RTTI, dynamic_cast, virtual-function implementation and multiple-inheritance virtual dispatch together.

Scope

polymorphic types;
dynamic_cast;
pointer vs reference failure;
downcast;
cross-cast;
typeid;
std::type_info;
RTTI metadata;
typical vptr/vtable implementation;
multiple inheritance;
this-pointer adjustment;
thunks;
virtual destructors in vtables;
devirtualization.

Quant angle

Now we can properly answer:

Why can virtual dispatch be undesirable on a hot path?

Not because "virtual is slow", but because of possible:

pointer load;
indirect branch;
inhibited inlining;
branch-prediction effects;
additional pointer adjustment;
loss of optimization opportunities.

And then discuss when the cost is completely irrelevant.

Practice

Use Compiler Explorer and compare:

obj.f();
ptr->f();
base_ref.f();

with optimization on/off and with/without final.

Also explore dynamic_cast across a multiple-inheritance hierarchy.
Runtime-polymorphism debugging + compare dispatch strategies. Independent strings/parsing timed problem.

---

## 8. Templates I: fundamentals, instantiation and overload resolution

Scope

function templates;
class templates;
member templates;
instantiation;
implicit vs explicit instantiation;
template argument deduction basics;
explicit template arguments;
template overloads;
ordinary overload vs template overload;
compilation model;
why definitions normally live in headers.

Interview

Why can't I normally put a template implementation in .cpp?

Does every template create machine code?

When exactly does instantiation happen?

f(T) vs f(T&) vs f(const T&).

Literature

A Tour of C++, Templates.
Effective Modern C++, especially Items 1–4 for deduction.
For our deepest template work: C++ Templates: The Complete Guide, 2nd ed. as an additional reference.

Practice

Make part of the matrix library generic over scalar type.
Implement a generic binary heap / priority queue. Good bridge between templates and real DS.

---

## 9. Templates II: specialization, dependent names, variadics and compile-time machinery

This is deliberately a full second template chapter, not a tiny appendix.

Scope

full specialization;
partial specialization;
function overload vs function-template specialization;
non-type template parameters;
dependent names;
typename;
dependent template;
two-phase lookup conceptually;
variadic templates;
parameter packs;
pack expansion;
fold expressions;
elementary type_traits;
elementary compile-time computation.

Interview

Given nasty template code, explain why lookup happens when it does, instead of memorizing where to type typename.

Practice

Implement:

StaticMatrix<T, Rows, Cols>;
compile-time dimension checking;
a small is_same;
remove_reference;
conditional;
variadic all_same.

No std::type_traits internally for the first iteration.
Generic DS exercise + one graph/BFS problem under interview conditions.

---

## 10. Exceptions, stack unwinding and exception safety

Exactly where you wanted them.

Ilia's course covers exception mechanics, RAII, construction/destruction interaction, exception safety and exception specifications.

Scope

throw, try, catch;
exception object;
handler matching;
stack unwinding;
constructors that throw;
partially constructed objects;
destructors during unwinding;
std::terminate;
noexcept;
conditional noexcept;
basic / strong / no-throw guarantees.

Quant angle

We go beyond "HFT doesn't use exceptions."

Questions:

Is throwing an exception expensive?
Is merely compiling with exceptions necessarily expensive on the normal path?
Why may exceptions still be avoided in latency-critical code?
Why does tail latency matter more than average latency?
Why does std::vector care whether your move constructor is noexcept?

Literature

A Tour of C++: Error Handling.
cppreference exception handling / noexcept.

Practice

Create a vector-like resource owner with deliberately failing construction and prove whether it offers strong exception safety.

Special Stack: push/pop/top + min/max/sum in required complexities, then make operations exception-safe. This preserves the historical IMC-style task from our original plan.

---

## 11. RAII as a general resource-management mechanism

I don't want to teach RAII as simply "unique_ptr is good."

Scope

resource acquisition;
deterministic destruction;
scope guards;
RAII + exceptions;
file descriptors;
mutex locks;
sockets;
memory;
Rule of Zero connection.

Quant/system interview

Explain why this:

lock();
do_work();
unlock();

is fundamentally inferior to an ownership object even if do_work() "normally doesn't fail."

Practice

Implement three RAII wrappers:

file descriptor;
allocated aligned buffer;
generic scope guard.

Resource wrapper exercise + LRU cache design/implementation.

---

## 12. Iterator model and generic algorithms

Scope

iterator abstraction;
input/output/forward/bidirectional/random-access;
contiguous iterators;
iterator traits;
sentinel idea;
const iterators;
reverse iterators;
stream iterators;
algorithm requirements.

Quant angle

Difference between an operation being O(1) and being cheap.

Random-access iterator arithmetic versus pointer chasing.

Practice

Implement proper iterators for one structure from the matrix library.

Implement lower_bound, merge/partition-style algorithms; timed binary-search-on-answer problem.

---

## 13. std::vector internals

This deserves its own serious chapter because it is one of the highest-value C++ interview topics.

Scope

size vs capacity;
contiguous allocation;
growth policy;
placement construction;
destruction;
reserve;
resize;
push_back;
emplace_back;
reallocation;
iterator invalidation;
exception safety;
copy vs move during growth.

Quant questions

Why is vector often faster than list even for operations whose asymptotic complexity looks worse?
Cache locality.
Prefetching.
Memory bandwidth.
Why reserve() can matter for latency.
Why excessive reserve() can also be undesirable.

Practice

MiniVector<T> + two pointers / sliding window problem. Amortized analysis is mandatory.

---

## 14. deque, list and node-based structures

Scope

segmented storage of deque;
stable references;
list node layout;
pointer chasing;
splice;
iterator invalidation;
memory overhead.

Quant angle

list is a wonderful example of why Big-O alone is insufficient for performance engineering.

Practice

Benchmark sequential traversal:

vector
deque
list

for increasing data sizes and explain the result mechanistically.
Monotonic stack + monotonic queue problems; benchmark vector/deque/list traversal.

---

## 15. Trees and std::map

Scope

ordered associative containers;
balanced BST idea;
node layout;
logarithmic operations;
ordering requirements;
transparent comparisons;
iterator stability.

Interview

Compare:

std::map
std::unordered_map
sorted std::vector

for a read-heavy trading lookup table.

Practice

Implement a minimal BST/map-like container or a simplified red-black-tree exploration depending on how much algorithm work we've already done.

Implement binary heap/BST operations + priority-queue scheduling problem.

---

## 16. Hash tables and std::unordered_map

Scope

buckets;
hashing;
collisions;
load factor;
rehashing;
chaining;
average vs worst-case complexity;
iterator invalidation.

We'll also go beyond the STL implementation and discuss open addressing / flat hash maps because it matters greatly for performance-oriented C++.

Quant angle

Why can a flat/open-addressed map massively outperform node-based unordered_map despite both being "O(1)"?

Practice

Implement an open-addressing hash table with linear or quadratic probing.

This is highly interview-relevant.

Implement HashMap + one difficult array/hash problem.
---

## 17. Object lifetime, raw storage and allocator model

Scope

memory allocation vs object construction;
raw storage;
placement construction;
allocator;
allocator_traits;
allocator-aware containers;
propagation rules conceptually.

Interview

This ties directly back to Topic 1 lifetime.

Does allocating enough bytes create a T?

Why does vector separate storage from live elements?

Practice

Extend MiniVector to separate allocation/construction + timed graph traversal problem.

---

## 18. Custom allocation: arenas, pools, new/delete, alignment and std::pmr

We'll combine Ilia's allocator topics into one larger performance chapter instead of scattering them.

Scope

overriding operator new/delete;
aligned allocation;
fragmentation;
bump allocator;
pool allocator;
free list;
scoped allocators;
std::pmr::memory_resource;
monotonic_buffer_resource;
allocator lifetime.

Quant/HFT focus

Very high priority:

Why might a low-latency trading system forbid general-purpose allocation on the hot path?

Discuss:

unpredictable latency;
synchronization;
fragmentation;
cache effects;
page faults;
TLB;
deterministic reclamation.

CSAPP's memory chapters give useful systems context for allocation and the memory hierarchy.

Practice

Implement a proper arena/pool allocator and benchmark it against repeated new/delete.
Implement arena/pool allocator + task scheduling/topological-sort problem.

---

## 19. Value categories and move semantics

Scope

glvalue;
lvalue;
xvalue;
prvalue;
rvalue;
temporary materialization where useful;
rvalue references;
std::move;
move constructors;
moved-from state;
ref-qualified members.

Literature

Effective Modern C++, Items 23–29.

Interview

The central question:

Does std::move move anything?

Then progressively nastier examples involving const.

Practice

Instrument a type and predict every construction/copy/move/destruction before running it.
Instrumented class exercise + greedy/interval problem.

---

## 20. Perfect forwarding, copy elision and move_if_noexcept

I want these together because they're consequences of the same value-category machinery.

Scope

forwarding references;
reference collapsing;
std::forward;
universal/forwarding-reference deduction;
RVO;
NRVO;
guaranteed copy elision;
returning std::move(local);
move_if_noexcept.

Interview

Implement conceptually:

std::move
std::forward

and explain why:

return std::move(x);

can be pessimizing.

Practice

Build make_object<T>(args...) with perfect forwarding.

Then make our MiniVector choose copy vs move correctly.
Factory + vector relocation code + DP medium.

---

## 21. Type deduction: auto, decltype, CTAD and structured bindings

Ilia has a separate deduction section containing exactly these mechanisms.

Scope

template deduction rules;
auto;
auto&;
const auto&;
auto&&;
decltype;
decltype((x));
decltype(auto);
CTAD;
deduction guides;
structured bindings.

Practice

A large interview-style deduction round where you must determine the exact types without compiling.
Deduction interview round + binary search / prefix-sum problem.

---

22. unique_ptr and ownership-oriented API design

Scope

exclusive ownership;
move-only types;
custom deleters;
arrays;
incomplete types;
make_unique;
ownership transfer;
non-owning raw/reference views.

Quant angle

Smart pointers are not automatically desirable.

We'll distinguish:

ownership semantics
allocation policy
runtime overhead
API semantics

Practice

Implement UniquePtr<T, Deleter>.
Implement UniquePtr<T,D> + timed tree problem.

---

## 23. shared_ptr, weak_ptr and control blocks

This deserves its own chapter.

Scope

strong/weak counts;
control block;
object vs control-block lifetime;
make_shared;
custom deleter;
aliasing constructor;
cycles;
weak_ptr;
enable_shared_from_this;
double-control-block disaster;
CRTP connection.

Quant angle

Important:

atomic reference-count operations;
cache-line contention;
ownership ambiguity;
destruction happening on an unexpected thread;
latency spikes.

"Use shared_ptr everywhere" is absolutely not acceptable interview reasoning.

Practice

Implement a simplified SharedPtr / WeakPtr control block.

We already introduced these earlier; this is where we do them properly.
blocks
Simplified shared/weak pointer + graph with cycles problem. The conceptual connection is actually useful here.

---

## 24. Lambdas, closure objects and callable machinery

Scope

captures;
reference/value capture;
init capture;
mutable;
generic lambdas;
closure type;
closure object layout;
conversion to function pointer;
lifetime problems.

Quant angle

closure size;
accidental copies;
capturing large state;
reference lifetime;
potential allocation when stored elsewhere.

Practice

Recreate several lambdas manually as function objects.
Sorting + greedy challenge using custom predicates/comparators, then lambda/lifetime follow-ups.

---

## 25. std::function, type erasure and std::any

Scope

why heterogeneous callables require abstraction;
type erasure;
erased interface;
manager/vtable-like mechanism;
std::function;
std::any;
any_cast;
ownership of erased objects.

Lecture #35 specifically covers std::any implementation and type erasure.

Quant angle

The important question:

What can std::function cost compared with a template parameter or direct lambda?

Possibilities:

indirect call;
inability to inline;
erased type information;
possible allocation;
larger object.

Practice

Implement a simplified:

Function<R(Args...)>

without SBO first.

Jane-Street-style multi-stage API problem: start simple, then add caching/state/new requirements without rewriting everything.
---

## 26. Unions, variant, lifetime management, SBO/EBO and std::launder

This is one deep manual object representation chapter rather than five mini-lessons.

Scope

unions;
active member;
placement construction;
explicit destruction;
std::variant;
tagged unions;
std::visit;
Small Buffer Optimization;
Empty Base Optimization;
[[no_unique_address]] as modern context;
std::launder;
object lifetime corner cases.

Ilia groups these ideas under type erasure/unions and explicitly includes SBO/EBO, std::function, variant and launder.

Quant angle

SBO is extremely relevant to low-allocation designs.

Practice

Upgrade our Function from Topic 25 with a small-object buffer.

Optionally implement a two-type miniature Variant<T,U>.
Board-game/state-machine simulation. Very HRT/Optiver-like implementation exercise.

---

## 27. Template metaprogramming, traits, SFINAE and detection

Scope

metafunction;
type/value members;
integral_constant;
type transformations;
SFINAE;
enable_if;
detection idiom;
checking whether expressions/methods exist;
implementation ideas behind:
is_constructible,
is_nothrow_move_constructible,
is_base_of,
common_type.

These exact examples appear in Ilia's later curriculum and mirrored lectures.

Interview angle

This isn't just syntax torture.

We ask:

How can the compiler choose an implementation based on properties of a type without runtime branching?

Practice

Implement a mini traits library.
Traits implementation + harder DP timed problem.

---

## 28. Concepts, requires and modern constrained generic programming

Ilia explicitly ends the TMP sequence with requires and concepts.

Scope

constraints;
concepts;
requires-clause;
requires-expression;
compound requirements;
subsumption at a practical level;
constrained overload resolution;
SFINAE vs concepts.

Quant/library-design angle

Design compile-time interfaces with zero runtime dispatch.

Excellent fit for the matrix library.

Practice

Define concepts such as:

Scalar
MatrixLike
ContiguousMatrix
Multipliable

and constrain your algorithms appropriately.
Constrain a generic graph/container API + shortest-path problem.
---

## 29. constexpr, consteval, constinit and compile-time evaluation

Ilia explicitly covers all three and the expanded modern constexpr model.

Scope

constant expressions;
constexpr function;
runtime invocation of constexpr;
immediate functions;
consteval;
constinit;
static initialization;
compile-time vs runtime work;
C++20 constexpr capabilities.

Interview

Is a constexpr function always evaluated at compile time?

Difference between const, constexpr, consteval, constinit.

Practice

Implement compile-time matrix/vector operations for small fixed dimensions.
Compile-time programming + bitmasking/combinatorics runtime problem.

---

## 30. Typelists and compile-time algorithms

Ilia's final compile-time section includes a typelist and a compile-time quicksort.

Scope

type sequences;
push/pop;
transform;
filter;
recursion;
compile-time algorithms;
template-instantiation cost;
compiler resource consumption.

Quant angle

This is where we explicitly discuss the price of static polymorphism:

runtime cost ↓
compile time ↑
binary size ↑
instruction-cache pressure potentially ↑

"Compile time" does not automatically mean "free."

Practice

Implement:

TypeList<Ts...>

plus transform, filter and sorting by a compile-time trait.
Ilia-style compile-time algorithms + hard graph/tree problem.

---

## 31. C++ performance synthesis — quant-dev mock interview + implementation

This one is our addition, not an Ilia lecture.

The point is to connect the entire course to the interview we actually care about.

I give you a hypothetical hot-path component:

market data
    ↓
decode
    ↓
instrument lookup
    ↓
strategy callback
    ↓
order decision

and you defend decisions about:

virtual dispatch vs templates;
variant vs inheritance;
std::function vs direct callable;
unordered_map vs flat table vs sorted vector;
vector vs node structures;
ownership model;
shared_ptr;
allocations;
arenas;
exceptions;
move/copy behavior;
alignment;
cache locality;
compile-time specialization;
code size.

Practical task

Implement a miniature zero/low-allocation event-dispatch pipeline and benchmark alternative designs.

For example:

Version A — virtual interface
Version B — std::function
Version C — std::variant + visit
Version D — compile-time templated dispatcher

Then measure rather than blindly declare a winner.

This becomes our first genuinely quant-development-flavored C++ engineering task.

Full quant-SWE live coding mock: stateful trading-flavored component with requirement changes, complexity and performance follow-ups.


---


## 28. IMC-style Mock OA #1

**Źródła**
Brak nowej teorii — powtórka 19–27.

**Zadanie**

* 120 minut,
* 2 problemy,
* C++,
* zero podpowiedzi,
* testy niewidoczne,
* pełna analiza complexity.

Potem robimy postmortem:

* czas na pomysł,
* czas implementacji,
* edge cases,
* bugi,
* optymalność,
* jakość kodu.

---

# BLOK III — OPERATING SYSTEMS & MEMORY

OSTEP jest tutaj źródłem podstawowym. Aktualna wersja udostępnia dokładnie potrzebne rozdziały o procesach, VM, paging, TLB i concurrency. ([UW Computer Sciences][3])

## 29. Processes i address space

**Źródła**

* OSTEP Ch. 4 — *Processes*
* OSTEP Ch. 13 — *Address Spaces*
* CSAPP Ch. 8 — Exceptional Control Flow

**Zakres**

* czym jest process,
* CPU state,
* virtual address space,
* code/data/heap/stack,
* PCB,
* process states,
* isolation.

**Interview**
„What exactly is a process?”

---

## 30. `fork`, `exec`, `wait` i process API

**Źródła**

* OSTEP Ch. 5 — *Process API*
* TLPI/APUE jako rozszerzenie

**Coding**
Napisać:

```text
parent
 └── fork
      └── child → exec
parent → wait
```

**Zakres**

* return values,
* copy-on-write,
* zombie,
* orphan,
* `exec` semantics.

---

## 31. User mode, kernel mode, syscalls i context switches

**Źródła**

* OSTEP Ch. 6 — *Limited Direct Execution*
* CSAPP Ch. 8

**Zakres**

* privilege levels,
* syscall,
* trap,
* interrupt,
* exception,
* scheduler,
* context switch,
* dlaczego syscall kosztuje więcej niż normalny call.

**Interview**
„What happens when userspace calls `read()`?”

---

## 32. Virtual memory i address translation

**Źródła**

* OSTEP Ch. 13–15
* CSAPP Ch. 9 — Virtual Memory

**Zakres**

* virtual vs physical addresses,
* MMU,
* page mapping,
* protection,
* isolation,
* address translation.

**Praktyka**
Ręczne tłumaczenie adresów na małym modelu.

---

## 33. Paging, page tables i TLB

**Źródła**

* OSTEP Ch. 18 — Paging
* Ch. 19 — Translation Lookaside Buffers
* Ch. 20 — Advanced Page Tables

**Zakres**

* VPN/offset,
* PTE,
* multi-level tables,
* TLB hit/miss,
* page-table walk,
* huge pages.

**Praktyka**
Policzyć rozmiar page tables i koszt kilku translation scenarios.

---

## 34. Page faults, demand paging, swapping, COW i `mmap`

**Źródła**

* OSTEP Ch. 21–23
* CSAPP Ch. 9

**Zakres**

* minor/major conceptual page fault,
* demand paging,
* replacement,
* dirty page,
* copy-on-write,
* memory-mapped files.

**Interview**
Opisać krok po kroku page fault od CPU do ponownego wykonania instrukcji.

---

## 35. `malloc`, `free` i memory allocators

**Źródła**

* CSAPP Ch. 9 — Dynamic Memory Allocation
* OSTEP Ch. 17 — Free Space Management

**Zakres**

* free lists,
* splitting/coalescing,
* fragmentation,
* alignment,
* allocator metadata,
* `brk`/`mmap` conceptually.

**Coding**
Bump allocator albo prosty free-list allocator.

---

## 36. File descriptors i Unix I/O

**Źródła**

* CSAPP Ch. 10 — System-Level I/O
* OSTEP — Files and Directories
* TLPI jako rozszerzenie

**Zakres**

* FD table,
* open file description,
* `open/read/write/close`,
* `dup`,
* redirection,
* blocking I/O.

**Coding**
Mini `cat`/redirection program.

---

## 37. Memory bugs, UB i debugging tools

**Źródła**

* cppreference: Undefined behavior
* sanitizers documentation
* Valgrind jako uzupełnienie

**Zakres**

* use-after-free,
* double-free,
* buffer overflow,
* uninitialized memory,
* dangling pointer,
* strict aliasing,
* integer UB.

**Praktyka**
Naprawić zestaw buggy programs przez:

* ASan,
* UBSan,
* GDB.

---

BLOCK III — OPERATING SYSTEMS & MEMORY

The goal of this block is not to turn you into a kernel engineer. The target is to understand the OS deeply enough that when an interviewer asks why a syscall, page fault, context switch, allocation, TLB miss, or blocking I/O operation can hurt latency, you can follow the mechanism from your C++ code → CPU → kernel → hardware → back to userspace.

OSTEP is the primary conceptual source. CSAPP complements it from the application programmer's perspective: its Chapters 8–10 cover processes, context switches, VM, page tables/TLBs, mmap, allocators, memory bugs, and Unix I/O in exactly the areas we need.

29. Processes and Address Spaces
Literature

Primary

OSTEP Ch. 4 — The Abstraction: The Process
4.1 The Abstraction: A Process
4.2 Process API
4.3 Process Creation
4.4 Process States
4.5 Data Structures
OSTEP Ch. 13 — The Abstraction: Address Spaces
especially 13.3 Address Space
13.4 Goals: transparency, efficiency, protection

OSTEP explicitly introduces a process as the OS abstraction of a running program and explains CPU virtualization through time sharing; its address-space chapter then develops isolation and the illusion of private memory.

Secondary

CSAPP Ch. 8.2 — Processes
8.2.1 Logical Control Flow
8.2.2 Concurrent Flows
8.2.3 Private Address Space
8.2.4 User and Kernel Modes
8.2.5 Context Switches
TLPI Ch. 6 — Processes
xv6 Ch. 2.5 — Process Overview, optional implementation view.

xv6 is useful here because it explicitly connects process isolation with address spaces, CPU state, user/supervisor mode, and time sharing.

Scope

You should understand:

program vs process;
process execution context;
CPU registers, PC, stack pointer;
virtual address space;
code/text, static data, heap and stack;
kernel-maintained process state / PCB concept;
running, ready/runnable, blocked states;
process isolation;
private virtual addresses vs shared physical resources;
process vs thread at a high level.
Interview target

What exactly is a process?

And follow-ups such as:

Is a process just an address space?

What has to be saved when a process stops running?

Can two processes have the same virtual address?

Can they map that address to different physical pages?

Practical work

Task A — inspect your own address space.

Write a C/C++ program that prints the addresses of:

function
global variable
static variable
heap allocation
local stack variable

Then inspect the process with /proc/<pid>/maps or pmap and explain where every address belongs.

Task B — process observation.

Run a sleeping process and inspect:

/proc/<pid>/status
/proc/<pid>/maps
/proc/<pid>/fd

You should be able to connect the observable Linux information with the theoretical process abstraction.

30. fork, exec, wait and the Process API
Literature

Primary

OSTEP Ch. 5 — Interlude: Process API
5.1 fork()
5.2 wait()
5.3 exec()
5.4 Why this API exists

The uploaded OSTEP edition puts these three mechanisms together deliberately.

Deeper Linux treatment

TLPI:

Ch. 24 — Process Creation
Ch. 25 — Process Termination
Ch. 26 — Monitoring Child Processes
Ch. 27 — Program Execution
Ch. 28 — Process Creation and Program Execution in More Detail

Those chapter mappings come directly from the TLPI edition we have.

Also useful

CSAPP Ch. 8.4 — Process Control
process IDs;
creating and terminating processes;
reaping children;
loading/running programs;
fork() + execve().

Scope
exact fork() return semantics;
parent/child execution;
inherited process state;
virtual address spaces after fork;
copy-on-write;
exec replaces rather than creates a process;
executable loading;
wait / waitpid;
exit status;
zombie;
orphan;
why zombies exist;
file descriptor inheritance.
Interview targets

What happens when fork() returns?

Does fork() copy the entire physical memory of the parent?

What survives an exec()?

Why does the child keep the same PID across exec()?

What exactly is a zombie?

Practical task — mini process launcher

Build:

parent
  |
  +-- fork()
       |
       +-- child --> execvp(...)
  |
  +-- waitpid()

Then progressively add:

return-code propagation;
arguments;
child error handling;
timing;
multiple children.

A stronger follow-up later will turn this into part of a miniature shell.

31. User Mode, Kernel Mode, System Calls, Traps and Context Switches

This is one place where I would expand the old plan slightly.

Literature

Primary

OSTEP Ch. 6 — Mechanism: Limited Direct Execution
restricted operations;
switching between processes.
Selected OSTEP Ch. 7 — Scheduling: Introduction
Selected OSTEP Ch. 10 — Multiprocessor Scheduling, especially:
10.3 cache affinity.

OSTEP's table of contents explicitly includes process switching, scheduling, and later cache affinity in multiprocessor scheduling.

CSAPP

Ch. 8.1 — exceptions;
Ch. 8.2.4 — user/kernel modes;
Ch. 8.2.5 — context switches.

Linux implementation/API

TLPI Ch. 3 — System Programming Concepts
TLPI Ch. 35 — Process Priorities and Scheduling
xv6 Ch. 2.2 — user/supervisor mode and system calls
xv6 Ch. 4 — Traps and System Calls

The xv6 book separates page-table machinery from traps/system calls, which makes it a particularly clean implementation reference here.

Scope
privilege levels;
user mode vs kernel mode;
syscall;
trap;
hardware interrupt;
fault/exception;
transition into kernel;
kernel stack concept;
context switch;
mode switch vs context switch;
scheduler;
blocking;
timer interrupt;
scheduling latency;
CPU/cache affinity at a conceptual level.
Quant/HFT connection

This is where we start asking:

Why might a low-latency system care which CPU a thread runs on?

Why can involuntary preemption hurt tail latency?

Why is crossing into the kernel more expensive than an ordinary function call?

Importantly:

function call
!=
system call
!=
context switch

Interviewers love mixing these up.

Practical work

Task A — syscall measurement.

Benchmark:

ordinary function call
vs
actual syscall

over millions of iterations.

We'll be careful not to accidentally benchmark a libc/vDSO optimization instead of an actual kernel transition.

Task B — context-switch ping-pong.

Create two processes communicating through pipes:

A --> B --> A --> B ...

and estimate round-trip/context-switch cost.

Task C — tracing.

Use strace on a tiny program calling read() and explain every relevant system call.

Interview target

What happens when userspace calls read()?

Eventually your answer should trace:

userspace
→ syscall instruction
→ privilege transition
→ kernel
→ FD lookup
→ file/device/socket-specific path
→ possibly block
→ scheduler
→ wakeup
→ return to userspace

without pretending every read() necessarily follows exactly the same kernel path.

32. Virtual Memory and Address Translation
Literature

OSTEP

Ch. 13 — Address Spaces
Ch. 14 — Memory API
Ch. 15 — Mechanism: Address Translation

OSTEP's Ch. 15 explicitly develops hardware-assisted translation and the responsibilities of hardware and OS.

CSAPP

Ch. 9:

9.1 Physical and Virtual Addressing
9.2 Address Spaces
9.3 VM as a caching mechanism
9.4 VM as a memory-management mechanism
9.5 VM as a protection mechanism
9.6 Address Translation

Optional implementation

xv6 Ch. 3 — Page Tables
3.1 Paging Hardware
3.2 Kernel Address Space
3.3 Creating an Address Space.

xv6 gives a concrete example of separate user virtual address spaces and kernel mappings.

Scope
virtual vs physical address;
address space;
MMU;
translation;
permissions/protection;
mapping;
isolation;
page frame concept;
why VM is useful even on a machine with "enough RAM";
distinction between virtual-memory abstraction and swapping.
Practical work

First we do manual translation exercises on tiny artificial machines.

For example:

virtual address: 16 bits
page size:       256 bytes
VPN:             ?
offset:          ?
PTE:             ?
physical addr:   ?

Then write a small address translator simulator:

translate(virtual_address, page_table)

that reports:

VPN
offset
PTE
PFN
physical address
protection fault?
page fault?

That forces you to understand the mechanism rather than memorize diagrams.

33. Paging, Page Tables and the TLB

This should be one of the deepest OS topics.

Literature

OSTEP

Ch. 18 — Paging: Introduction
Ch. 19 — Paging: Faster Translations (TLBs)
Ch. 20 — Paging: Smaller Tables

The book also provides simulators for linear and multilevel page tables, including exercises calculating table size and translation costs.

CSAPP Ch. 9

Particularly:

9.3.2 Page Tables
9.6 Address Translation
9.6.2 TLB
9.6.3 Multi-Level Page Tables
9.6.4 End-to-End Address Translation
9.7 Intel/Linux case study.

CSAPP explicitly walks through both TLB hits and TLB misses inside the MMU.

Scope
page;
page frame;
VPN;
VPO/page offset;
PTE;
present/valid/permission/dirty/accessed bits conceptually;
linear page tables;
multilevel page tables;
page-table walk;
TLB;
TLB hit/miss;
TLB reach;
why larger pages affect TLB reach;
huge pages/superpages;
page-table memory overhead.
Quant/HFT questions

Why can random memory access hurt even if everything fits in RAM?

Because "RAM access" might actually involve:

TLB miss
→ page-table walk
→ several dependent memory reads
→ eventual data access

before we've even discussed normal cache misses.

Practical work

Task A — manual problems.

Calculate page-table sizes for:

32-bit VA
4 KiB pages


48-bit VA
4 KiB pages


different numbers of page-table levels

Task B — OSTEP simulators.

Use the supplied paging/TLB exercises before letting the simulator show the answer.

Task C — TLB benchmark.

Write a benchmark that touches one byte from every page:

for (each page) {
    sum += memory[page * page_size];
}

Change the number of pages and access order.

We then try to explain performance discontinuities using TLB capacity and locality.

This is very relevant to performance-oriented interviews.

34. Page Faults, Demand Paging, Replacement, COW and mmap
Literature

OSTEP

Ch. 21 — Beyond Physical Memory: Mechanisms
swap space;
present bit;
page faults;
page-fault control flow.
Ch. 22 — Beyond Physical Memory: Policies
Ch. 23 as continuation of the VM block.

The uploaded OSTEP edition explicitly walks through the page-fault mechanism and replacement decisions here.

CSAPP

9.3.4 Page Faults
9.8 Memory Mapping
shared objects;
fork() revisited;
execve() revisited;
userspace mmap().

TLPI

Ch. 49 — Memory Mappings
Ch. 50 — Virtual Memory Operations

TLPI Ch. 50 covers mechanisms such as mprotect(), mlock()/mlockall(), mincore(), and madvise(), which become extremely interesting later for low-latency Linux.

Optional xv6

Ch. 5 — Page Faults
lazy allocation;
COW fork;
demand paging;
memory-mapped files.
Scope
present vs absent page;
page fault;
faulting instruction;
page-fault handler;
demand paging;
minor vs major faults as a Linux-oriented distinction;
page replacement;
dirty pages;
swap;
copy-on-write;
anonymous mappings;
file-backed mappings;
private/shared mappings;
mmap;
mprotect;
mlock conceptually.
Interview target

Walk me through a page fault from the faulting CPU instruction until that instruction can execute successfully.

That answer should eventually distinguish several cases:

invalid access       → process gets an error/signal
valid, nonresident   → bring/create page and retry
COW write            → duplicate page, remap and retry
protection violation → reject access
Practical work

Task A — mmap a file.

Memory-map a file, modify bytes through the mapping, and explain when/why the file changes.

TLPI itself contains an exercise to implement a cp-like program using mmap() and memcpy().

Task B — first-touch benchmark.

Allocate/map a large anonymous region.

Measure:

first pass
second pass

and investigate why first-touch behavior differs.

Task C — COW experiment.

Allocate a large buffer, fork(), and compare:

child reads
child writes

while observing memory/page-fault behavior.

That is much stronger than merely defining copy-on-write.

35. malloc, free and Memory Allocators

We already touch allocators in C++, but here we study the systems side.

Literature

OSTEP

Ch. 14 — Memory API, especially underlying OS support
Ch. 17 — Free-Space Management

OSTEP's allocator material includes placement strategies, fragmentation, coalescing, and a simulator for comparing first/best/worst fit.

CSAPP

Ch. 9.9 — Dynamic Memory Allocation

Important subsections:

9.9.1 malloc and free
9.9.3 Requirements and Goals
9.9.4 Fragmentation
9.9.5 Implementation Issues
9.9.6 Implicit Free Lists
9.9.7 Placement
9.9.8 Splitting
9.9.9 Obtaining More Heap Memory
9.9.10 Coalescing
9.9.11 Boundary Tags
9.9.12 Simple Allocator
9.9.13 Explicit Free Lists
9.9.14 Segregated Free Lists

CSAPP explicitly frames allocator design as a trade-off between throughput and memory utilization.

TLPI

Ch. 7 — Memory Allocation
Scope
userspace allocator vs OS memory manager;
malloc/free;
backing memory from the OS;
brk/sbrk conceptually;
mmap;
block headers;
alignment;
internal fragmentation;
external fragmentation;
free lists;
first/best/worst fit;
splitting;
coalescing;
boundary tags;
segregated lists;
allocator complexity and latency.
Quant angle

We explicitly ask:

Why might malloc() be unacceptable on a hot path even when average allocation time looks small?

Potential concerns:

unpredictable search time;
synchronization;
page faults;
metadata/cache misses;
fragmentation;
interaction with the OS;
tail latency.
Practical tasks

We do two stages.

Stage 1 — bump allocator

void* allocate(size_t bytes, size_t alignment);
void reset();

Simple, fast, no individual frees.

Stage 2 — free-list allocator

Implement:

headers
alignment
free list
block splitting
free
coalescing

Then benchmark:

bump allocator
free-list allocator
system malloc/new

under different allocation patterns.

This is an excellent quant-development task.

36. File Descriptors and Unix I/O

I would significantly strengthen the old practical part. A mini cat is too small by itself.

Literature

CSAPP Ch. 10 — System-Level I/O

Read essentially the whole chapter:

10.1 Unix I/O
10.2 Files
10.3 Opening and Closing
10.4 Reading and Writing
10.5 Robust I/O
10.6 Metadata
10.7 Directories
10.8 Sharing Files
10.9 I/O Redirection
10.10 Standard I/O.

TLPI

Primary:

Ch. 4 — File I/O: The Universal I/O Model
Ch. 5 — File I/O: Further Details
Ch. 13 — File I/O Buffering

Optional:

Ch. 44 — Pipes and FIFOs

TLPI's structure makes the progression from basic descriptors to buffering and then IPC explicit.

Scope
file descriptor;
per-process FD table;
open file description;
inode/file object conceptually;
file offset;
open;
read;
write;
close;
lseek;
dup / dup2;
standard input/output/error;
shared descriptor state after fork;
redirection;
buffering;
partial reads/writes;
blocking I/O;
pipes.
Interview questions

What is a file descriptor actually identifying?

If I call dup(fd), are these two independent files?

What happens to the file offset?

What happens to descriptors after fork()?

Why can read(fd, buf, 4096) return fewer than 4096 bytes without an error?

Practical task — miniature shell pipeline

Rather than only cat, build:

command args...

then:

command > file

then finally:

command1 | command2

using:

fork
exec
pipe
dup2
close
waitpid

That one project ties Topics 30, 31, and 36 together beautifully.

37. Memory Bugs, Undefined Behavior and Debugging Tools

I would keep this topic, although technically it is not "OS theory". It belongs here because after understanding virtual memory, allocators, mappings, and process memory, debugging memory corruption becomes much more concrete.

Literature

CSAPP

3.10.2 — GDB
3.10.3 — Out-of-Bounds Memory References / Buffer Overflow
Ch. 9.11 — Common Memory-Related Bugs
bad pointers;
uninitialized memory;
stack buffer overflow;
off-by-one;
bad pointer arithmetic;
references to freed blocks;
leaks.

C++-specific reference

cppreference / compiler documentation for:

undefined behavior;
object lifetime;
signed overflow;
strict aliasing;
alignment.

CSAPP is excellent for concrete memory errors, but it is not sufficient by itself for the full C++ object-model/UB rules.

Scope
use-after-free;
double free;
heap buffer overflow;
stack buffer overflow;
uninitialized memory;
dangling pointers/references;
invalid pointer arithmetic;
alignment errors;
strict-aliasing issues;
signed integer overflow;
memory leaks;
difference between:
crash,
sanitizer error,
undefined behavior.
Practical work

I give you several intentionally broken programs.

You must diagnose them using:

GDB
ASan
UBSan
Valgrind

without me telling you the bug.

A later version will contain multiple interacting bugs, e.g.:

vector reallocation
→ dangling pointer
→ use-after-free
→ seemingly unrelated crash later

which is much closer to real systems debugging.

---

# BLOCK III — CONCURRENCY & THE C++ MEMORY MODEL

The goal is not merely to know how to launch `std::thread`.

By the end, you should be able to reason through:

```text
source code
   ↓
C++ abstract machine / memory model
   ↓
compiler transformations
   ↓
atomic operations / synchronization
   ↓
cache coherence / CPU interaction
   ↓
OS scheduling
   ↓
latency and scalability
```

And for an HFT/C++ interview you should be able to defend questions such as:

> Why is this program a data race?

> What makes an atomic operation atomic?

> Does a mutex only provide mutual exclusion, or also memory ordering?

> Why does acquire/release make another thread's writes visible?

> When might a lock-free structure actually be slower?

> Why does false sharing occur if two threads never touch the same variable?

> Why might an HFT thread spin rather than block?

---

# 1. Threads, Thread Lifetime, Scheduling and Task-Based Concurrency

This is broader than the old "Threads and scheduling" chapter.

## Literature

### Primary — *C++ Concurrency in Action*

**Ch. 1 — Hello, world of concurrency in C++**

Read:

* 1.1 What is concurrency?
* 1.2 Why use concurrency?
* 1.3 Concurrency and multithreading in C++

**Ch. 2 — Managing threads**

Essentially the whole chapter:

* basic thread management;
* launching;
* `join`;
* `detach`;
* passing arguments;
* transferring thread ownership;
* choosing number of threads;
* thread IDs.

The book explicitly treats `std::thread` ownership using move semantics and discusses oversubscription/thread count. 

### OSTEP

* Ch. 26 — **Concurrency: An Introduction**
* Ch. 27 — **Interlude: Thread API**

OSTEP's introductory examples are useful because they show that after thread creation, execution order is fundamentally nondeterministic from the program's perspective. 

### CSAPP

Ch. 12:

* 12.1 Concurrent Programming with Processes
* 12.2 Concurrent Programming with I/O Multiplexing
* 12.3 Concurrent Programming with Threads
* 12.4 Shared Variables in Threaded Programs

CSAPP is especially good for connecting the thread abstraction back to Linux processes and memory.

### Linux extension

TLPI:

* Ch. 29 — **Threads: Introduction**
* selected Ch. 33 — **Threads: Further Details**

TLPI's structure explicitly has a five-chapter thread block from Ch. 29 to 33. 

---

## Scope

* concurrency vs parallelism;
* process vs thread;
* address-space sharing;
* per-thread:

  * registers,
  * program counter,
  * stack,
  * thread-local state;
* shared:

  * globals,
  * heap,
  * address space;
* creation and termination;
* `join` vs `detach`;
* ownership of `std::thread`;
* `std::jthread`;
* cooperative cancellation / `stop_token` conceptually;
* `hardware_concurrency()`;
* scheduling;
* oversubscription;
* context switching;
* CPU-bound vs I/O-bound workloads;
* concurrency vs actual parallel execution.

We also introduce task-based programming:

* `std::async`;
* `std::future`;
* `std::promise`;
* `std::packaged_task`;

because Ch. 4 of *Concurrency in Action* treats futures and promises as a core synchronization mechanism rather than an unrelated library curiosity. 

---

## Interview questions

> What does a thread share with another thread in the same process?

> Why does every thread need its own stack?

> `join()` vs `detach()`?

> What happens if a joinable `std::thread` is destroyed?

> What is oversubscription?

> If I have 16 CPU cores, are 16 threads always optimal?

> What's the difference between a thread and a task?

> When might `std::async` be preferable to explicitly creating a thread?

---

## Practical work

### Task A — thread lifecycle experiment

Create workers and deliberately experiment with:

```cpp
join()
detach()
std::jthread
std::this_thread::get_id()
```

Observe nondeterministic ordering.

### Task B — parallel reduction

Implement:

```cpp
template<class It>
long long parallel_sum(It first, It last, std::size_t threads);
```

Requirements:

1. single-threaded baseline;
2. N threads;
3. correct partitioning;
4. handle remainder;
5. exception-safe thread joining;
6. benchmark different thread counts.

Then answer:

> Why does `2 × threads` eventually stop helping?

### Task C — task-based rewrite

Rewrite part of it using futures/`std::async`.

Compare the programming model, not just runtime.

---

# 2. Race Conditions, C++ Data Races and Shared Invariants

This chapter should come **before mutex APIs**.

First understand the bug. Then learn the tool.

## Literature

### OSTEP

Ch. 26 — especially the shared-counter example.

OSTEP's canonical program has two threads repeatedly executing:

```text
counter = counter + 1
```

to demonstrate why apparently simple source operations aren't atomic. 

### C++ Concurrency in Action

Ch. 3:

* 3.1 Problems with sharing data between threads
* race conditions;
* avoiding problematic race conditions;
* invariant protection.

The book explicitly distinguishes a general race condition from the C++ concept of a **data race**. 

### CSAPP

Ch. 12:

* shared variables;
* synchronization;
* progress-graph reasoning.

CSAPP even contains exercises distinguishing safe and unsafe execution trajectories through critical regions. 

---

## Scope

This distinction must become automatic:

### Race condition

Program correctness depends on relative timing/order.

### Data race

At least two potentially concurrent conflicting accesses to the same memory location, at least one write, without the synchronization required by the C++ memory model.

And in C++:

> **A data race causes undefined behavior.**

Not merely "sometimes returns the wrong number."

Also:

* read-modify-write decomposition;
* atomicity;
* shared invariant;
* critical region;
* synchronization;
* interleavings;
* TOCTOU-style races;
* why `volatile` does **not** fix thread synchronization;
* thread-safe implementation vs thread-safe **interface**.

That last one is very important.

For example, even if:

```cpp
stack.empty();
stack.top();
```

are individually synchronized, the sequence may still be logically racy.

---

## Interview questions

> Race condition vs data race?

> Is every race condition a data race?

> Is every data race a race condition?

> Why doesn't `volatile int counter` fix the counter?

> Can two threads read the same non-atomic integer concurrently?

> What if one reads and one writes?

> If every public member of a class locks a mutex, is the class automatically thread-safe?

---

## Practical work

### Task A — deliberately broken counter

Implement:

```cpp
int counter = 0;
```

with multiple workers doing `++counter`.

Run enough times to observe non-determinism.

Then test with:

```text
ThreadSanitizer
```

Compile roughly with:

```text
-fsanitize=thread
```

We don't just fix it—we first explain the interleavings.

### Task B — broken interface

I'll give you a nominally thread-safe stack whose individual methods lock correctly.

You need to discover why:

```cpp
if (!stack.empty()) {
    auto x = stack.top();
    stack.pop();
}
```

still has a race at the API level.

This style of question is excellent interview material.

---

# 3. Mutexes, RAII Locking and Lock Granularity

Now we introduce mutual exclusion properly.

## Literature

### OSTEP

Ch. 28 — **Locks**

Important ideas:

* lock correctness;
* spin waiting;
* hardware primitives conceptually;
* evaluating locks.

### C++ Concurrency in Action

Ch. 3, especially:

* protecting shared data with mutexes;
* `std::mutex`;
* `std::lock_guard`;
* structuring protected data;
* flexible locking with `std::unique_lock`;
* transferring ownership;
* lock granularity.

The book devotes significant attention to the fact that simply "putting a mutex somewhere" doesn't guarantee a good interface or good performance. 

### TLPI

Ch. 30 — **Threads: Thread Synchronization**. 

---

## Scope

* mutual exclusion;
* mutex ownership;
* RAII locking;
* `lock_guard`;
* `unique_lock`;
* `scoped_lock`;
* `try_lock`;
* recursive mutex — and why it is often suspicious;
* `shared_mutex`;
* reader/writer locking;
* coarse-grained vs fine-grained locks;
* contention;
* lock convoy conceptually;
* critical-section duration;
* exception safety;
* mutex synchronization also establishing ordering/visibility.

That final point matters:

> A mutex is **not merely a gate around code**.

Unlocking and subsequently locking the same mutex participate in synchronization relationships, which is why protected writes become visible.

---

## Quant/HFT angle

Suppose:

```text
market-data thread
        |
       mutex
        |
strategy thread
```

Questions become:

* how frequently is the mutex contended?
* how long is it held?
* can the thread block?
* could the scheduler intervene?
* are unrelated objects protected by the same lock?
* do we create cache-line bouncing?

"Mutexes are slow" is not a sufficient answer.

---

## Practical work

### Task A — synchronized account/order book state

Implement a state object with multiple operations and explicitly define its invariants.

Then make it safe using **one coarse mutex**.

### Task B — finer granularity

Redesign with multiple locks.

Benchmark.

Then reason about why finer locking might:

* improve parallelism;
* increase complexity;
* increase lock overhead;
* create deadlock possibilities.

---

# 4. Condition Variables and Blocking Synchronization Primitives

This deserves a proper topic rather than being squeezed into mutexes.

## Literature

### OSTEP

Ch. 30 — **Condition Variables**

### C++ Concurrency in Action

Ch. 4:

* waiting for an event or condition;
* condition variables;
* building a thread-safe queue.

Also use the library-reference discussion of `wait()`.

The book explicitly notes that `wait()` atomically unlocks the mutex and blocks, relocks it before return, and can wake spuriously; hence predicates/loops are required. 

### OSTEP semaphores

Ch. 31 — **Semaphores**

### C++20 reference

We also include:

* `std::counting_semaphore`;
* `std::binary_semaphore`;
* `std::latch`;
* `std::barrier`.

---

## Scope

### Condition variables

* condition vs mutex;
* `wait`;
* `notify_one`;
* `notify_all`;
* atomic unlock-and-sleep;
* predicate;
* spurious wakeup;
* lost wakeup reasoning;
* Mesa semantics;
* `while`, not naive `if`.

OSTEP explicitly explains that modern condition-variable semantics require rechecking the condition after waking. 

### Semaphores

* counter semantics;
* binary vs counting semaphore;
* producer/consumer;
* resource permits.

### C++20

* latch;
* barrier;
* when each abstraction is appropriate.

---

## Interview questions

> Why does `condition_variable::wait()` need a mutex?

> Why is the mutex released while sleeping?

> Why do we check the predicate again after waking?

> `notify_one` vs `notify_all`?

> What is the thundering herd problem?

> Mutex vs binary semaphore?

> Semaphore vs condition variable?

---

## Main implementation — bounded blocking queue

Implement:

```cpp
template<class T>
class BlockingQueue {
public:
    explicit BlockingQueue(std::size_t capacity);

    void push(T value);
    T pop();
};
```

Requirements:

* bounded capacity;
* multiple producers;
* multiple consumers;
* no busy-wait;
* correct shutdown behavior later;
* exception safety;
* no lost wakeups.

Then extend:

```cpp
close();
```

such that blocked consumers/producers exit cleanly.

This is a **very good quant/C++ live-coding problem** because the simple-looking API hides many concurrency edge cases.

---

# 5. Deadlock, Livelock, Starvation and Locking Strategy

I would keep these separate from basic locking because this is about **system-level progress**, not mutex syntax.

## Literature

### OSTEP

Ch. 32 — **Common Concurrency Problems**

OSTEP explicitly splits real-world concurrency bugs into deadlock and non-deadlock patterns. 

### C++ Concurrency in Action

Ch. 3:

* deadlock;
* deadlock avoidance;
* locking multiple mutexes;
* `std::lock`;
* guidelines.

### Optional classic problems

OSTEP recommends semaphore/deadlock problems including Dining Philosophers. 

---

## Scope

* deadlock;
* Coffman conditions:

  * mutual exclusion,
  * hold and wait,
  * no preemption,
  * circular wait;
* lock ordering;
* simultaneous lock acquisition;
* `std::scoped_lock`;
* nested locks;
* callback-under-lock hazards;
* livelock;
* starvation;
* fairness;
* priority inversion conceptually.

That last one is worth knowing for low-latency work even if we don't go deeply into RTOS scheduling.

---

## Interview problems

> Two functions acquire A then B / B then A. What's wrong?

> Can you get deadlock with only one mutex?

> Difference between deadlock and starvation?

> What's livelock?

> Does `try_lock()` automatically solve deadlocks?

> Why is calling user code while holding a mutex dangerous?

---

## Practical work

### Task A — deliberately produce deadlock

Two threads:

```text
T1: lock A → lock B
T2: lock B → lock A
```

Make it reproducible.

Then fix it at least two ways:

1. global ordering;
2. `std::scoped_lock`.

### Task B — Dining Philosophers

Implement a naive deadlocking solution.

Then redesign it.

The task is less about dining philosophers and more about learning to reason about **resource graphs and global invariants**.

---

# 6. Atomic Operations and Compare-and-Exchange

Now mutex-based synchronization is understood, so atomics will have context.

## Literature

### C++ Concurrency in Action

Ch. 5:

* 5.1 Memory model basics
* 5.2 Atomic operations and types

The chapter covers:

* `atomic_flag`;
* `atomic<bool>`;
* atomic pointers;
* integral atomics;
* generic `atomic<T>`.



### Reference

cppreference:

* `std::atomic`;
* `atomic_ref`;
* `is_lock_free`;
* `compare_exchange_weak`;
* `compare_exchange_strong`.

### Optional hardware bridge

xv6 Ch. 7 locking, particularly atomic operations and instruction/memory ordering.

The xv6 text makes the important connection that lock acquire/release needs memory ordering so another CPU observes earlier writes. 

---

## Scope

* atomic load/store;
* read-modify-write;
* `fetch_add`;
* exchange;
* compare-and-exchange;
* CAS;
* weak vs strong CAS;
* spurious CAS failure;
* CAS loops;
* atomic integral types;
* atomic pointers;
* atomic object requirements;
* lock-free vs implemented internally using locks;
* `is_lock_free`;
* `is_always_lock_free`;
* `atomic_ref`;
* atomicity vs ordering.

Critical idea:

> **Atomic does not mean "everything around this variable is synchronized."**

Atomicity and ordering are separate concepts.

---

## Interview questions

> How would you implement atomic increment using CAS?

> Why does CAS take an expected value by reference?

> Weak vs strong CAS?

> Does `std::atomic<T>` guarantee lock-free implementation?

> Does an atomic variable prevent races on nearby ordinary variables?

> Atomic vs mutex?

---

## Practical work

### Task A — CAS counter

No `fetch_add`.

Implement increment using a CAS loop.

### Task B — state machine

For example:

```text
Idle → Starting → Running → Stopping → Stopped
```

Multiple threads compete to perform transitions.

Use CAS to ensure only legal transitions happen.

### Task C — spinlock

Implement a tiny spinlock with `atomic_flag`.

Then benchmark against `std::mutex`.

Crucially, we will **not conclude that the spinlock is "faster" from one microbenchmark**.

We'll examine when spinning is catastrophic.

---

# 7. The C++ Memory Model and Happens-Before

This remains one of the deepest topics in the entire plan.

We should not rush it.

## Literature

### C++ Concurrency in Action

Ch. 5:

* 5.1 Memory model basics
* 5.3 Synchronizing operations and enforcing ordering

Essential concepts:

* objects and memory locations;
* modification order;
* synchronizes-with;
* happens-before;
* ordering non-atomic operations through atomic synchronization.



### cppreference

* Memory model
* memory order

cppreference is reference material here, **not the first explanation**.

### Hardware context

From our preceding OS/CPU knowledge:

* cache;
* store buffers conceptually;
* compiler reorderings;
* CPU reorderings;
* coherence.

But we must keep two models separate:

> C++ guarantees are defined by the **C++ abstract machine**, not by "whatever x86 happens to do."

---

## Scope

* memory location;
* conflicting evaluations;
* sequenced-before;
* inter-thread synchronization;
* synchronizes-with;
* happens-before;
* modification order;
* visibility;
* data races;
* atomic vs non-atomic accesses;
* publication;
* compiler reordering;
* CPU reordering;
* why source-code order alone isn't enough;
* mutexes expressed in memory-model terms.

---

## Central exercise

Given programs such as:

```cpp
int data = 0;
std::atomic<bool> ready = false;

// T1
data = 42;
ready.store(true, ...);

// T2
if (ready.load(...)) {
    std::cout << data;
}
```

you must tell me:

1. Is there a data race?
2. Which ordering is required?
3. What relationship is established?
4. Why does `data == 42` become visible?
5. What changes if we use relaxed ordering?

We will do many litmus-test-style examples.

---

## Interview target

A concise answer to:

> **What is happens-before?**

And then survive progressively harder follow-ups.

This should not become:

> "Acquire happens before release."

That statement itself is usually backwards/incomplete.

You will need to explain the actual chain.

---

# 8. Memory Ordering: `seq_cst`, Acquire/Release, Relaxed and Fences

Only after Topic 7.

## Literature

### C++ Concurrency in Action Ch. 5

The book goes deeply through:

* sequential consistency;
* non-SC orderings;
* acquire/release;
* release sequences;
* relaxed ordering;
* fences.

Its sequential-consistency example demonstrates the global total order guaranteed for SC atomics, before deliberately moving to weaker models. 

### cppreference

`std::memory_order` reference.

---

## Scope

* `memory_order_seq_cst`;
* `memory_order_release`;
* `memory_order_acquire`;
* `memory_order_acq_rel`;
* `memory_order_relaxed`;
* fences;
* release sequence;
* valid memory orders for loads/stores/RMW;
* why relaxed still provides atomicity;
* synchronization through acquire/release;
* global ordering under seq_cst;
* cost differences are architecture-dependent;
* x86 vs ARM at a conceptual level.

---

## Quant interview emphasis

I don't want you to memorize:

```text
acquire = reads
release = writes
```

and stop there.

Instead:

> **Which actual program invariant requires which ordering?**

We begin with `seq_cst`.

Only weaken ordering after proving why it remains correct.

This is also exactly the approach recommended in *Concurrency in Action* for lock-free prototyping: start with `seq_cst` before weakening order. 

---

## Practical work

### Task A — message passing

Implement the standard publication pattern.

Try:

```text
seq_cst
acquire/release
relaxed
```

Reason about correctness before running anything.

### Task B — litmus tests

I show code such as:

```text
Store Buffering
Message Passing
Independent Reads of Independent Writes
```

You enumerate allowed outcomes under specified C++ orderings.

### Task C — assembly

Use Compiler Explorer for x86 and ARM targets.

Look at how:

```cpp
load(relaxed)
load(acquire)
store(relaxed)
store(release)
```

can map differently depending on architecture.

This prevents us from treating memory ordering as mystical syntax.

---

# 9. Lock-Based Concurrent Data Structures and Synchronization Design

The old plan missed this almost completely.

It's too important to jump directly from atomics to lock-free programming.

## Literature

### C++ Concurrency in Action

**Ch. 6 — Designing lock-based concurrent data structures**

* thread-safe stack;
* queue;
* fine-grained queue;
* lookup table;
* thread-safe list.

The chapter explicitly moves from one-lock structures toward finer-grained locking. 

### OSTEP

Ch. 29 — **Lock-based Concurrent Data Structures**

### Design support

Ch. 8 of *Concurrency in Action*:

* partitioning work;
* data contention;
* cache ping-pong;
* false sharing;
* data proximity;
* oversubscription.



---

## Scope

* making an existing sequential structure thread-safe;
* choosing invariants;
* API-level races;
* coarse-grained locking;
* fine-grained locking;
* per-bucket/per-node locking;
* contention;
* scalability;
* linearization point conceptually;
* safe lifetime management;
* blocking structures.

---

## Quant angle

Suppose you have:

```text
InstrumentId → MarketState
```

and 32 threads.

Which is better?

```text
one mutex for map
one mutex per bucket
one mutex per instrument
lock-free map
single-owner thread + message passing
```

There is no universal answer.

We'll learn to ask:

* read/write ratio;
* contention distribution;
* object lifetime;
* update frequency;
* cache locality;
* complexity;
* required latency distribution.

---

## Main practical task — thread-safe hash table

Implement a simplified:

```cpp
ConcurrentHashMap<Key, Value>
```

Version 1:

```text
one global lock
```

Version 2:

```text
striped / bucket-level locking
```

Then benchmark workloads such as:

```text
100% reads
90% reads / 10% writes
50/50
high-key contention
low-key contention
```

This is much more valuable than saying "fine-grained locks scale better."

---

# 10. Cache Coherence, False Sharing and Concurrent Performance

Before lock-free structures, understand the hardware cost they're trying to manipulate.

## Literature

### C++ Concurrency in Action

Ch. 8.2:

* data contention and cache ping-pong;
* **false sharing**;
* data proximity;
* oversubscription;
* excessive task switching.

These are explicitly covered in the book's concurrent-performance chapter. 

### CSAPP

Ch. 6 — **Memory Hierarchy**

Use our prior cache/locality knowledge.

Ch. 12 — concurrent programming for scaling/parallel-performance context.

### Optional

Use Linux `perf` where hardware counters are available.

---

## Scope

* cache line;
* coherence conceptually;
* write ownership;
* cache-line invalidation;
* true sharing;
* false sharing;
* cache-line ping-pong;
* contention;
* padding/alignment;
* `std::hardware_destructive_interference_size`;
* per-thread counters;
* reduction;
* NUMA only as an introduction;
* affinity connection to the OS block.

---

## Interview question

Given:

```cpp
struct Counters {
    std::atomic<long> a;
    std::atomic<long> b;
};
```

Thread A only modifies `a`.

Thread B only modifies `b`.

> Why might performance still collapse?

This is one of the highest-value HFT-style concurrency questions.

---

## Practical task — false-sharing benchmark

Version A:

```cpp
struct Counter {
    std::atomic<uint64_t> value;
};
```

Adjacent array elements.

Version B:

separate hot counters onto different cache lines.

Benchmark scaling from:

```text
1
2
4
8
...
threads
```

Then interpret—not just report—the results.

---

# 11. Lock-Free Programming, ABA and Memory Reclamation

Now we're finally ready.

## Literature

### C++ Concurrency in Action

**Ch. 7 — Designing lock-free concurrent data structures**

Read carefully:

* definitions and consequences;
* lock-free stack;
* memory reclamation;
* hazard pointers;
* reference counting;
* lock-free queue;
* guidelines;
* ABA.

The book explicitly warns that memory reclamation is one of the central difficulties in lock-free structures, and includes hazard pointers as one strategy. 

The chapter's guidelines include:

* prototype with `seq_cst`;
* use a lock-free reclamation scheme;
* watch for ABA;
* identify busy-wait loops.



---

## Scope

Progress guarantees:

* blocking;
* obstruction-free conceptually;
* lock-free;
* wait-free.

Then:

* CAS loop;
* retry behavior;
* contention;
* ABA;
* tagged/versioned pointers conceptually;
* lifetime/reclamation problem;
* hazard pointers;
* epoch-based reclamation conceptually;
* why garbage-collected languages make some lock-free algorithms much easier.

And crucially:

> Lock-free does **not** mean wait-free.

> Lock-free does **not** mean faster.

> Lock-free does **not** mean contention-free.

---

## Interview questions

> Define lock-free.

> Can one particular thread starve in a lock-free algorithm?

> What is ABA?

> Why can't we simply `delete` a node after successfully popping it from a lock-free stack?

> Why is memory reclamation often harder than the actual CAS algorithm?

---

## Practical work

I would **not** start with a Treiber stack with hazard pointers. That's too much at once.

We progress:

### Task A — lock-free counter/state machine

Already familiar.

### Task B — bounded SPSC ring buffer

This is the big HFT-relevant task.

Implement:

```cpp
template<class T, std::size_t Capacity>
class SpscQueue {
public:
    bool try_push(const T&);
    bool try_pop(T&);
};
```

Requirements:

* fixed capacity;
* no allocation after construction;
* exactly one producer;
* exactly one consumer;
* non-blocking;
* correct wraparound;
* acquire/release only where justified;
* padding to reduce false sharing;
* benchmark against mutex queue.

This task is **far more relevant for low-latency development** than randomly implementing an MPMC queue.

### Task C — optional advanced

Treiber stack and then discuss why reclamation immediately becomes difficult.

---

# 12. Thread Pools, Work Stealing, Scalability and Low-Latency Design

The old block completely missed this, and I think that's a significant omission.

## Literature

### C++ Concurrency in Action

**Ch. 9 — Advanced Thread Management**

Particularly:

* 9.1 Thread pools;
* waiting for submitted tasks;
* tasks waiting for tasks;
* avoiding contention on work queue;
* **work stealing**.



### Ch. 8 — Designing Concurrent Code

Read:

* 8.1 dividing work;
* 8.2 performance factors;
* 8.4 scalability and Amdahl's law.

The book explicitly covers Amdahl's law, contention, false sharing, oversubscription, and hiding latency. 

### OS connection

From Block II:

* scheduler;
* CPU affinity;
* context switching.

---

## Scope

* why creating a thread per task is expensive;
* worker pool;
* task queue;
* producer/worker model;
* futures for results;
* shutdown;
* exception propagation;
* oversubscription;
* work stealing;
* queue contention;
* central vs per-worker queues;
* scalability;
* Amdahl's law;
* latency vs throughput;
* CPU affinity;
* busy polling vs blocking;
* spin/yield/sleep;
* dedicated threads;
* thread-per-core architecture conceptually.

---

## Quant/HFT angle

This deserves explicit treatment:

A traditional server might prioritize:

```text
throughput
resource efficiency
fairness
```

while an HFT component may prioritize:

```text
predictable latency
minimal jitter
cache warmth
no scheduler intervention
core locality
```

So something like:

```cpp
cv.wait(...)
```

may be exactly correct for a general application but undesirable for a dedicated hot-path consumer where a few cores are intentionally reserved.

Conversely, spinning everywhere is also terrible design.

We need to understand the workload.

---

## Main practical task — thread pool

Build progressively:

### V1

```text
global synchronized work queue
N workers
submit(task)
shutdown()
```

### V2

Return:

```cpp
std::future<R>
```

from `submit`.

### V3

Measure:

* 1 / 2 / 4 / 8 / ... workers;
* short vs long tasks;
* queue contention;
* oversubscription.

### Optional V4

Per-worker queues + basic work stealing.

This is substantial enough to become a portfolio-quality subsystem if implemented carefully.

---

# 13. Concurrency Testing, Debugging and Quant-Style Final Exercise

Concurrency code that "ran correctly 1000 times" is not evidence of correctness.

## Literature

### C++ Concurrency in Action

**Ch. 10 — Testing and debugging multithreaded applications**

It explicitly covers:

* unwanted blocking;
* race conditions;
* code review;
* testing;
* testability;
* multithreaded testing;
* performance testing.



### OSTEP

Ch. 32 — concurrency bug patterns. 

### Tools

* ThreadSanitizer;
* GDB;
* `perf`;
* assertions/invariants;
* stress tests.

---

## Scope

* why concurrency bugs are nondeterministic;
* heisenbugs;
* stress testing;
* randomized scheduling/yields;
* deterministic components;
* invariants;
* TSan;
* deadlock diagnosis;
* performance vs correctness testing;
* benchmark methodology;
* race detector limitations;
* logging changing timing.

---

## Practical task A — concurrency bug hunt

I provide a program containing several issues such as:

* data race;
* lifetime race;
* lock-order inversion;
* missing predicate around CV;
* accidental false sharing;
* overly broad critical section.

You get only:

```text
source
tests
symptoms
```

No hints.

Use:

```text
TSan
GDB
logging
reasoning
```

to fix it.

---

# Final Block III Project — Mini Market-Data Pipeline

I think this should be the culminating exercise.

Instead of an artificial final exercise, build a simplified low-latency pipeline:

```text
Producer
   |
   v
SPSC Ring Buffer
   |
   v
Consumer / Processor
   |
   v
Statistics / Output
```

We develop several versions.

### Version 1 — `std::mutex` + `condition_variable`

Correct blocking implementation.

### Version 2 — bounded SPSC ring

Fixed-capacity, preallocated structure.

### Version 3 — optimized layout

Investigate:

* false sharing;
* producer/consumer indices;
* alignment;
* batching;
* memory order;
* padding.

### Version 4 — benchmark

Measure:

* throughput;
* mean latency;
* p50;
* p95;
* p99;
* optionally p99.9;
* effect of queue capacity;
* batching;
* producer/consumer CPU placement if environment allows.

Then answer the real interview question:

> **Why is Version 2 faster/slower than Version 1 on this workload?**

Not:

> "Because lock-free is faster."

---

# So the final Block III structure

I would use **13 large mastery topics**, not eight fragmented lessons:

| #      | Topic                                              | Main implementation                |
| ------ | -------------------------------------------------- | ---------------------------------- |
| **1**  | Threads, scheduling and task-based concurrency     | Parallel reduction + futures       |
| **2**  | Race conditions, data races and invariants         | Broken concurrent programs + TSan  |
| **3**  | Mutexes, RAII locking and lock granularity         | Coarse vs fine locking             |
| **4**  | Condition variables, semaphores, latches, barriers | Bounded blocking queue             |
| **5**  | Deadlock, livelock, starvation                     | Deadlock lab + Dining Philosophers |
| **6**  | Atomics and CAS                                    | CAS state machine + spinlock       |
| **7**  | C++ memory model and happens-before                | Publication/litmus reasoning       |
| **8**  | Memory ordering and fences                         | Message passing + assembly         |
| **9**  | Lock-based concurrent structures                   | Concurrent hash table              |
| **10** | Coherence, contention and false sharing            | False-sharing benchmark            |
| **11** | Lock-free structures, ABA, reclamation             | **SPSC ring buffer**               |
| **12** | Thread pools, work stealing and scalability        | Thread pool                        |
| **13** | Testing/debugging + low-latency synthesis          | **Market-data pipeline**           |

I like this much more than the original Block V. The original was fundamentally correct,  but it jumped from mutexes to atomics/memory ordering and then compressed **false sharing + all lock-free structures** into one final topic. That skips the actual engineering progression.

The new progression is:

```text
threads
  ↓
races
  ↓
mutex synchronization
  ↓
waiting / coordination
  ↓
progress failures
  ↓
atomics
  ↓
formal memory model
  ↓
weak ordering
  ↓
concurrent structure design
  ↓
cache/coherence costs
  ↓
lock-free structures
  ↓
thread pools / scalability
  ↓
real low-latency system
```

That's the sequence I'd want before a demanding IMC/Optiver/HRT-style C++ concurrency interview.

---

Yes. I would make this **Block IV — CPU Architecture & Performance**, but I would strengthen it in the same way we strengthened OS and concurrency.

Your original six topics are good, but they compress two things too aggressively:

* **SIMD** is large enough to deserve its own mastery topic rather than sharing one with profiling.
* **Performance measurement** deserves its own final topic, because benchmarking mistakes can invalidate everything we learned before it.

So I would make this a **7-topic block**.

The central idea should be:

```text
C++ source
    ↓
compiler
    ↓
instructions
    ↓
frontend / branch prediction
    ↓
OoO execution / dependencies
    ↓
loads and stores
    ↓
cache hierarchy / DRAM
    ↓
measured latency and throughput
```

For quant/HFT interviews, the goal is not to become a CPU designer. The goal is to be able to explain why code with identical Big-O and even identical instruction count can have wildly different latency.

CSAPP is excellent here. Its performance sequence is particularly useful: Ch. 5 progresses from compiler limitations through modern processor execution, loop unrolling, instruction-level parallelism, branch prediction, memory performance, and profiling; Ch. 6 then develops locality and caches. 

---

# BLOCK IV — CPU ARCHITECTURE & PERFORMANCE

## 1. Data Representation, Alignment, Padding and Object Layout

This is the bridge from the C++ object model to hardware.

### Literature

### CSAPP Ch. 2 — Representing and Manipulating Information

Read selectively:

* **2.1 Information Storage**

  * data sizes;
  * byte ordering;
  * hexadecimal representation.
* **2.2 Integer Representations**

  * unsigned;
  * two's complement;
  * signed/unsigned conversions.
* **2.3 Integer Arithmetic**

  * overflow implications.
* **2.4 Floating Point**

  * IEEE representation;
  * rounding;
  * floating-point operations.

CSAPP's Ch. 2 explicitly moves from byte representation through signed/unsigned encodings and then IEEE floating point. 

### CSAPP Ch. 3

Most important:

* **3.8 Array Allocation and Access**
* **3.9 Heterogeneous Data Structures**

  * 3.9.1 Structures
  * 3.9.2 Unions
  * **3.9.3 Data Alignment**



### C++ reference

cppreference:

* object representation;
* `sizeof`;
* `alignof`;
* `alignas`;
* standard-layout types;
* `std::byte`;
* `std::bit_cast`.

---

### Scope

* bits, bytes, words;
* endianness;
* signed/unsigned representation;
* two's complement;
* floating-point representation at interview depth;
* alignment;
* padding;
* tail padding;
* arrays and alignment;
* object size;
* struct/class member ordering;
* nested structures;
* inheritance effects from our C++ block;
* `alignas`;
* packed structures and their drawbacks;
* Array of Structures vs Structure of Arrays;
* object representation vs value representation;
* padding bytes;
* serialization warning:

  * object memory layout is not automatically a portable wire format.

---

### Quant/HFT emphasis

We'll use something realistic:

```cpp
struct Order {
    uint64_t id;
    double price;
    uint32_t quantity;
    Side side;
    bool active;
};
```

Questions:

> Predict `sizeof(Order)` before compiling.

> Where is padding inserted?

> Can rearranging members reduce size?

> Is the smallest structure automatically the fastest structure?

> What if we process only `price` and `quantity` for 10 million orders?

This leads naturally into:

```text
AoS
Order Order Order Order

vs

SoA
prices:     ...
quantities: ...
sides:      ...
```

For trading systems this is much more interesting than merely memorizing alignment rules.

---

### Practical work

#### Task A — layout puzzles

I'll give you increasingly difficult structures involving:

```cpp
char
int
double
arrays
nested structs
inheritance
alignas(...)
```

You predict:

* member offsets;
* padding;
* alignment;
* `sizeof`.

Then verify.

#### Task B — `Order` layout optimization

Design three layouts:

1. obvious;
2. compact;
3. optimized for a particular access pattern.

Measure both:

* memory consumption;
* traversal performance.

#### Task C — AoS vs SoA

Generate several million synthetic orders.

Benchmark an operation such as:

```cpp
notional += price[i] * quantity[i];
```

with AoS and SoA.

Then explain the result in terms of **what bytes the CPU actually needs to fetch**.

---

# 2. Cache Hierarchy, Locality and Memory Bandwidth

One of the most important topics in the entire performance plan.

### Literature

### CSAPP Ch. 6 — The Memory Hierarchy

Read:

* **6.1 Storage Technologies** — selectively;
* **6.2 Locality**

  * temporal locality;
  * spatial locality;
* **6.3 Memory Hierarchy**
* **6.4 Cache Memories**

  * organization;
  * direct mapped;
  * set associative;
  * fully associative conceptually;
  * write policies;
  * cache parameters;
* **6.5 Writing Cache-Friendly Code**
* **6.6 Impact of Caches on Program Performance**

CSAPP explicitly ends this sequence with loop rearrangement and exploiting locality in real programs. 

### Prior dependencies

From Block II:

* pages;
* TLB;
* huge pages.

From Block III:

* cache-line sharing;
* false sharing.

We won't repeat them, but we'll connect them.

---

### Scope

* memory hierarchy;
* L1/L2/L3;
* cache line;
* cache capacity;
* sets and associativity;
* cache tag/index/offset conceptually;
* hit;
* miss;
* compulsory/capacity/conflict miss conceptually;
* temporal locality;
* spatial locality;
* working set;
* streaming access;
* random access;
* memory bandwidth vs latency;
* hardware prefetching;
* pointer chasing;
* cache-friendly layouts.

---

### Quant/HFT interview questions

> Why is sequential access to a `vector` so much faster than following a linked list?

Not merely:

> "cache locality."

We should be able to explain:

```text
contiguous vector
→ one cache line supplies several elements
→ prefetcher recognizes stream
→ multiple accesses amortize line fill

linked list
→ next address depends on previous load
→ poor spatial locality
→ difficult prefetching
→ dependent cache misses
```

Other questions:

> Why can a smaller data structure be dramatically faster?

> Why might a sorted vector beat a tree for lookup?

> What's a working set?

> Why can performance suddenly deteriorate when the data set crosses a certain size?

---

### Practical work

#### Task A — matrix traversal

Perfect use of your matrix library.

Compare:

```cpp
for (row)
    for (col)
        matrix[row][col];
```

against column-major traversal of row-major data.

Test multiple matrix sizes.

The important part isn't getting a pretty graph. It is identifying when the working set transitions through different levels of the hierarchy.

#### Task B — pointer chasing

Compare:

```text
sequential array traversal
random-index array
linked structure
```

for the same number of elements.

#### Task C — cache blocking

Implement naive matrix multiplication:

```text
i-j-k
```

and other loop orders.

Then implement tiled/block multiplication.

This is probably the single best practical exercise for this chapter because it combines:

* locality;
* cache reuse;
* matrix layout;
* loop ordering.

---

# 3. Pipelines, Superscalar Execution, Out-of-Order Execution and Dependencies

This is where performance becomes much more interesting than "CPU executes one instruction at a time."

### Literature

### CSAPP Ch. 5 — Optimizing Program Performance

Important parts:

* **5.1 Capabilities and Limitations of Optimizing Compilers**
* 5.2 Expressing Program Performance
* 5.4 Eliminating Loop Inefficiencies
* 5.6 Eliminating Unneeded Memory References
* **5.7 Understanding Modern Processors**

  * 5.7.1 Overall Operation
  * 5.7.2 Functional Unit Performance
  * 5.7.3 Abstract Model
* **5.8 Loop Unrolling**
* **5.9 Enhancing Parallelism**

  * multiple accumulators;
  * reassociation.
* 5.11.1 Register Spilling.

These sections are explicitly organized around exposing more instruction-level parallelism to the processor. 

### Optional deeper source

**Agner Fog — Optimizing software in C++ / Instruction Tables**

Use later as a hardware-specific reference for:

* latency;
* reciprocal throughput;
* execution-unit behavior.

Not something to memorize cover to cover.

---

### Scope

* pipeline idea;
* superscalar processor;
* IPC;
* instructions per cycle;
* functional units;
* instruction latency;
* throughput;
* dependency chains;
* independent instructions;
* instruction-level parallelism;
* out-of-order execution;
* reorder buffer conceptually;
* execution ports conceptually;
* register renaming conceptually;
* load/store dependencies;
* loop-carried dependency;
* loop unrolling;
* multiple accumulators;
* register pressure;
* register spilling.

---

### Very important distinction

Consider:

```cpp
for (...) {
    sum += a[i];
}
```

There is a dependency:

```text
sum₁ → sum₂ → sum₃ → sum₄
```

Now:

```cpp
sum1 += a[i];
sum2 += a[i + 1];
sum3 += a[i + 2];
sum4 += a[i + 3];
```

produces several independent chains.

Same algorithm.

Still O(n).

Possibly very different utilization of the CPU.

---

### Interview questions

> Latency vs throughput?

> Why does out-of-order execution help?

> What stops OoO from parallelizing everything?

> What's a dependency chain?

> Why can four accumulators outperform one?

> Why can unrolling eventually make things worse?

> What's register spilling?

---

### Practical work

#### Task A — dependency-chain benchmark

Compare:

```cpp
x = f(x);
x = f(x);
x = f(x);
```

with several independent values.

Measure.

#### Task B — reduction variants

Implement:

1. scalar accumulator;
2. 2 accumulators;
3. 4;
4. 8.

Inspect generated assembly.

Compare throughput.

#### Task C — matrix inner kernel

Use a dot-product from the matrix project and progressively:

* remove redundant loads;
* unroll;
* use independent accumulators.

Only optimize after measuring.

---

# 4. Branch Prediction and Speculative Execution

This deserves its own chapter.

### Literature

### CSAPP Ch. 5

Especially:

* **5.11.2 Branch Prediction and Misprediction Penalties**
* earlier control-flow material from Ch. 3 as supporting context.

CSAPP explicitly identifies branch prediction as one of the important limiting factors in optimized programs. 

### Optional

Agner Fog's optimization manuals for processor-specific details.

---

### Scope

* conditional branches;
* frontend control flow;
* branch predictor conceptually;
* historical behavior;
* predictable branches;
* unpredictable branches;
* misprediction;
* pipeline flush conceptually;
* speculative execution;
* conditional moves;
* branchless code;
* lookup tables;
* data dependency introduced by branchless transformations;
* why `branchless != faster`.

We need enough understanding of speculative execution to explain performance, not a full Spectre course.

---

### Classic interview question

Why can something like:

```cpp
if (x < threshold) {
    sum += x;
}
```

run differently when data is:

```text
sorted/predictable
```

versus:

```text
random
```

even though the number of iterations is identical?

---

### Quant/HFT angle

A trading hot path often contains decisions such as:

```cpp
if (message.type == Trade) ...
if (order.side == Buy) ...
if (price > limit) ...
```

The relevant questions are:

* how predictable are these distributions?
* can we restructure processing?
* does a branchless transformation actually remove the expensive branch?
* does it introduce extra loads/operations?
* what happens to code size?

---

### Practical work

#### Task A — predictable vs unpredictable branch

Benchmark the same loop against:

1. all true;
2. alternating;
3. highly skewed;
4. random conditions.

#### Task B — branchy vs branchless

Implement both.

Measure across data distributions.

You must predict **before benchmarking** which version should win.

Then explain surprises.

#### Task C — inspect assembly

Check whether the compiler used:

```text
branch
conditional move
vector mask
```

because source code containing `if` doesn't necessarily imply a branch in machine code.

---

# 5. Compiler Optimization, Inlining, Devirtualization and Generated Code

This connects our C++ block directly to hardware.

### Literature

### CSAPP Ch. 5

* **5.1 Capabilities and Limitations of Optimizing Compilers**
* 5.4 loop inefficiencies;
* 5.5 procedure calls;
* 5.6 memory references;
* 5.13 real-world optimization techniques.



### C++ references

cppreference:

* as-if rule;
* observable behavior;
* `inline`;
* `constexpr` context from Block I.

### Tool

**Compiler Explorer** becomes mandatory.

---

### Scope

Optimization levels:

```text
-O0
-O1
-O2
-O3
```

and conceptually:

* constant folding;
* constant propagation;
* dead-code elimination;
* common-subexpression elimination;
* loop-invariant code motion;
* inlining;
* devirtualization;
* escape-style reasoning conceptually;
* vectorization;
* unrolling;
* alias analysis;
* effect of `const` — without myths;
* LTO conceptually;
* PGO conceptually.

---

### C++-specific questions

> Does `inline` force inlining?

> Can a compiler inline a virtual call?

> How can `final` help optimization?

> Why may templates generate faster code than type erasure?

> Why might templates also hurt performance?

Because:

```text
specialization/inlining opportunities ↑

but potentially

binary size ↑
instruction-cache pressure ↑
```

---

### Practical work

#### Task A — optimization-level comparison

Take a moderately simple function.

Compile with:

```text
-O0
-O2
-O3
```

Explain major differences.

#### Task B — virtual dispatch

Return to our earlier C++ hierarchy.

Compare:

```cpp
Base& b
Derived final
template<T>
std::variant
```

and investigate whether the compiler can devirtualize.

#### Task C — benchmark trap

Write a benchmark whose entire computation gets optimized away.

Then repair it.

This sounds trivial, but it's essential. Many developers benchmark nothing.

---

# 6. SIMD and Auto-Vectorization

I would separate SIMD from profiling.

It is too valuable for performance C++, especially because your matrix project gives us excellent practical work.

### Literature

### CSAPP Ch. 5

Use:

* 5.7 modern processors;
* 5.8 loop unrolling;
* 5.9 parallelism;
* 5.12 memory performance.

CSAPP doesn't need to be our complete SIMD manual; it provides the architectural foundation. 

### Reference sources

Later:

* Intel Intrinsics Guide;
* compiler vectorization reports;
* Compiler Explorer.

Optional Agner Fog for instruction details.

---

### Scope

* scalar vs SIMD;
* data-level parallelism;
* vector registers;
* lanes;
* SIMD width conceptually;
* SSE/AVX/AVX2/AVX-512 as x86 families, not memorized instruction catalogs;
* SIMD-friendly data layout;
* alignment considerations;
* auto-vectorization;
* loop dependencies;
* aliasing preventing vectorization;
* reductions;
* masked operations;
* tail handling;
* explicit intrinsics;
* SIMD vs compiler-generated vector code.

---

### Interview questions

> What is SIMD?

> Why can't every loop be vectorized?

> What prevents vectorization?

> Why does SoA often help SIMD?

> Why might AVX-512 not simply make everything twice as fast as AVX2?

We can mention factors such as workload, memory bandwidth, CPU implementation and frequency behavior without going unnecessarily deep.

---

### Practical work

This chapter should heavily use the **matrix library**.

#### Task A — auto-vectorization

Start with:

```cpp
for (std::size_t i = 0; i < n; ++i) {
    c[i] = a[i] + b[i];
}
```

Inspect compiler output.

Then deliberately make vectorization harder and diagnose why.

#### Task B — dot product

Versions:

1. simple scalar;
2. compiler auto-vectorized;
3. manually unrolled;
4. optional explicit SIMD intrinsic version.

Measure.

#### Task C — matrix kernel

Optimize one real operation from your project.

Not the entire library—one hot loop.

This gives us a serious performance story for interviews:

> "I profiled my matrix library, identified X, changed the memory access/data-level parallelism, and measured Y."

Much stronger than:

> "I used AVX because AVX is fast."

---

# 7. Profiling, Benchmarking and Performance Methodology

This should finish Block IV.

Optimization without measurement is guessing.

### Literature

### CSAPP Ch. 5

Read:

* **5.2 Expressing Program Performance**
* **5.14 Identifying and Eliminating Performance Bottlenecks**

  * 5.14.1 Program Profiling
  * 5.14.2 Using a Profiler to Guide Optimization.

CSAPP explicitly finishes the performance chapter with profiler-guided optimization rather than treating profiling as an optional afterthought. 

### CSAPP Ch. 6

Use the cache sections to interpret profiling results.

### Tools

Linux:

```text
perf stat
perf record
perf report
```

Compiler Explorer.

Potentially later:

```text
Google Benchmark
perf annotate
flamegraphs
```

depending on environment.

---

### Scope

* wall-clock time;
* CPU time;
* throughput;
* latency;
* distribution;
* mean vs median;
* percentiles;
* p95 / p99 / p99.9;
* warm-up;
* cold vs warm cache;
* repeated measurements;
* noise;
* CPU frequency scaling;
* OS scheduling noise;
* affinity;
* dead-code elimination;
* constant folding;
* benchmark inputs;
* optimization levels;
* release vs debug builds;
* hardware counters;
* cycles;
* instructions;
* IPC;
* branches;
* branch misses;
* cache events;
* context switches;
* page faults;
* profiler sampling.

---

## Quant/HFT emphasis: averages are dangerous

Suppose two implementations have:

```text
A:
mean = 200 ns
p99  = 450 ns

B:
mean = 180 ns
p99  = 4 µs
```

Which is better?

For many ordinary throughput workloads, B may look great.

For latency-sensitive trading, that tail could completely change the answer.

We will develop the habit of asking:

> What metric are we actually optimizing?

---

## Main practical project — profile the matrix library

This should be the culmination of Block IV.

### Stage 1 — baseline

Choose a real operation such as matrix multiplication.

Establish:

* compiler flags;
* workload;
* matrix sizes;
* benchmark methodology.

### Stage 2 — profile

Use:

```text
perf stat
perf record
```

Find the actual hot code.

### Stage 3 — hypotheses

Possible bottleneck classes:

```text
compute
memory latency
memory bandwidth
cache misses
branching
dependency chain
vectorization
allocation
```

We do **not** optimize until we form a hypothesis.

### Stage 4 — transformations

Try things like:

* loop reordering;
* blocking;
* removing allocations;
* improved layout;
* unrolling;
* better alias information where appropriate;
* auto-vectorization;
* optional SIMD.

### Stage 5 — validate

Measure again.

Then determine **why** the result changed.

---

# Final Block IV interview exercise

I would end with a performance-oriented mock rather than another coding algorithm.

I'll give you something such as:

```cpp
double calculate(const std::vector<Order>& orders) {
    double result = 0;

    for (const auto& order : orders) {
        if (order.active && order.quantity > 0) {
            result += order.price * order.quantity;
        }
    }

    return result;
}
```

And start asking:

> It's too slow. What do you do?

The bad answer:

> Use SIMD and threads.

The answer I want you to develop:

1. What is "slow"?
2. What workload?
3. What's the baseline?
4. Profile it.
5. Understand the data layout.
6. Determine whether we're compute-bound or memory-bound.
7. Look at locality.
8. Look at branching.
9. Look at generated code.
10. Check vectorization.
11. Make one justified change.
12. Measure again.

Then I change the conditions:

> There are 100 million orders.

> Only 1% are active.

> Now 95% are active.

> The entire hot set fits in L2.

> Now it doesn't.

> `Order` grew from 24 to 96 bytes.

> We need p99 latency, not maximum throughput.

That is much closer to what a performance/C++ interviewer can do than asking you to recite what an L1 cache is.

---

# Final Block IV structure

| Topic                                                     | Main practical task                             |
| --------------------------------------------------------- | ----------------------------------------------- |
| **1. Data representation, alignment & object layout**     | `Order` layout + AoS vs SoA                     |
| **2. Cache hierarchy, locality & bandwidth**              | Matrix traversal + cache-blocked multiplication |
| **3. Pipelines, OoO & instruction dependencies**          | Multiple-accumulator/dependency benchmarks      |
| **4. Branch prediction & speculation**                    | Predictable/random/branchless benchmarks        |
| **5. Compiler optimization, inlining & devirtualization** | C++ → assembly investigation                    |
| **6. SIMD & auto-vectorization**                          | Matrix/dot-product SIMD optimization            |
| **7. Profiling & performance methodology**                | Full `perf` investigation of matrix library     |

This also fits extremely well with the preceding blocks:

```text
BLOCK I
C++ language and object model
        ↓
BLOCK II
OS / VM / syscalls / allocation
        ↓
BLOCK III
threads / memory model / synchronization
        ↓
BLOCK IV
what the CPU actually does with our code
```

And later, when we get to networking/low-latency systems, we can combine all four: socket syscalls, core affinity, queues, memory allocation, cache lines, packet layouts, batching, branch prediction and tail latency.

One thing I would **not** add here is another giant assembly-language chapter. We need to become comfortable **reading enough x86 assembly to understand compiler output**, but writing large programs in assembly would be poor ROI for your current interview target. CSAPP Ch. 3 gives us enough machine-level background to support that skill. 

---

# BLOK VI — NETWORKING

Beej's Guide jest nastawiony bezpośrednio na Internet sockets i zakłada znajomość C lub C++, więc będzie tutaj bardzo praktycznym źródłem. ([Beej][4])

## 52. „What happens when you type a URL?”

**Źródła**

* Beej's Guide to Network Concepts
* CSAPP Ch. 11
* Beej's Guide to Network Programming

**Musisz umieć przejść:**

```text
URL
↓
DNS
↓
routing / ARP
↓
TCP
↓
TLS
↓
HTTP
↓
server
↓
response
```

Każdy poziom może stać się osobnym follow-upem.

---

## 53. TCP głęboko

**Źródła**

* Beej
* TCP/IP Illustrated jako opcjonalne głębokie źródło

**Zakres**

* handshake,
* sequence numbers,
* ACK,
* retransmissions,
* sliding window,
* flow control,
* congestion control,
* connection teardown,
* TCP vs UDP.

**Interview**
Dlaczego TCP jest reliable i skąd właściwie ta reliability się bierze?

---

## 54. Socket programming i multiplexing

**Źródła**

* Beej's Guide to Network Programming
* TLPI networking chapters

**Coding**
TCP client/server.

Potem:

* blocking vs nonblocking,
* `select/poll/epoll`,
* partial reads/writes,
* connection handling.

---

## 55. Networking w low-latency systems

**Źródła**

* powtórka TCP/UDP
* Linux networking docs jako rozszerzenie

**Zakres**

* UDP,
* multicast,
* packet loss,
* Nagle,
* buffering,
* syscalls,
* copies,
* interrupts,
* kernel/user boundary,
* busy polling,
* kernel bypass — tylko architektura na początek.

**Design**
Prześledzić market-data packet od NIC do trading strategy.

---

Yes. Since we now have:

1. **Block I — C++**
2. **Block II — OS & Memory**
3. **Block III — Concurrency & C++ Memory Model**
4. **Block IV — CPU Architecture & Performance**

then networking should become **Block V**.

I would keep this block relatively compact. We do **not** need a university networking course with weeks of BGP, Ethernet switching, routing protocols, Wi-Fi, etc. For C++/quant SWE the high-value path is:

```text
application
→ socket
→ TCP / UDP
→ IP
→ Ethernet / NIC
→ network
```

plus the reverse path into userspace, with strong emphasis on **TCP semantics, UDP/multicast, Linux sockets, framing, nonblocking I/O, `epoll`, and low-latency packet handling**.

Your original four topics are good, but `Socket programming + multiplexing` is too large and UDP/multicast deserves more attention for trading. I would make it **6 topics**.

# BLOCK V — NETWORKING

## 1. Network Stack and “What Happens When You Type a URL?”

This becomes our conceptual foundation.

### Literature

**CSAPP Ch. 11 — Network Programming**

This is the best concise overview from the programmer's perspective. CSAPP treats networking as a core systems topic immediately after Unix I/O and before concurrency. 

**Beej's Guide to Network Programming**

Read initially:

* Ch. 2 — **What is a socket?**
* Ch. 3 — **IP Addresses, structs, and Data Munging**

  * IPv4/IPv6;
  * subnet basics;
  * ports;
  * byte order.
* selectively Ch. 5.1 — `getaddrinfo()`.

Beej then moves directly from these concepts into the actual socket API. 

**TCP/IP Illustrated**

Selected:

* Ch. 1 — TCP/IP layering, DNS, encapsulation, demultiplexing;
* Ch. 2 — link layer / Ethernet / MTU;
* Ch. 3 — IP;
* Ch. 4 — ARP;
* Ch. 9 — IP routing;
* Ch. 14 — DNS.

The uploaded Stevens book explicitly separates these mechanisms before beginning TCP. 

### Scope

You should be able to take:

```text
https://example.com/path
```

and walk through:

```text
URL parsing
    ↓
DNS resolution
    ↓
destination IP
    ↓
routing decision
    ↓
ARP / neighbor resolution when needed
    ↓
Ethernet frame
    ↓
IP packet
    ↓
TCP connection
    ↓
TLS handshake
    ↓
HTTP request
    ↓
server
    ↓
response
```

With follow-ups on:

* IP vs MAC address;
* subnet;
* default gateway;
* routing table;
* ARP;
* DNS caching;
* port numbers;
* ephemeral ports;
* network byte order;
* MTU;
* fragmentation conceptually;
* localhost/loopback;
* NAT conceptually;
* TCP 4-tuple.

### Interview questions

> Why do we need both an IP address and a MAC address?

> If the server is on another network, whose MAC address goes into my Ethernet frame?

> What does DNS actually return?

> What identifies a TCP connection?

> Can two clients connect to the same server IP and port simultaneously?

That last point leads to the TCP **4-tuple**:

```text
source IP
source port
destination IP
destination port
```

Stevens explicitly demonstrates that multiple established connections can share the server's port because TCP demultiplexes using the complete endpoint information. 

### Practical lab

No large implementation yet.

Use:

```text
dig / getent
ip addr
ip route
ip neigh
ping
traceroute
ss
tcpdump
curl
```

Pick one HTTP request and reconstruct what happened.

The target is to look at a packet capture and identify:

```text
ARP
DNS
TCP handshake
TLS
application data
TCP teardown
```

where observable.

---

# 2. TCP Deep Dive

This should be the deepest theoretical networking chapter.

### Literature

**TCP/IP Illustrated**

The core sequence is excellent:

* Ch. 17 — **TCP: Transmission Control Protocol**
* Ch. 18 — **Connection Establishment and Termination**
* Ch. 19 — **Interactive Data Flow**

  * delayed ACK;
  * **Nagle algorithm**.
* Ch. 20 — **Bulk Data Flow**

  * sliding windows;
  * slow start.
* Ch. 21 — **Timeout and Retransmission**

  * RTT;
  * retransmission;
  * congestion avoidance;
  * fast retransmit/recovery.
* Ch. 22 — Persist timer
* Ch. 23 — Keepalive
* selected Ch. 24 — TCP performance.

These chapters are explicitly laid out in that order in your copy. 

One caveat: your Stevens edition is the **1993 first edition**, so it is superb for fundamental TCP mechanisms and packet-level reasoning, but we should not treat its specific congestion-control details as a description of every modern Linux TCP implementation.

**TLPI**

* Ch. 58 — TCP/IP fundamentals;
* Ch. 61 — advanced sockets, including a deeper look at TCP and socket options. 

### Scope

* byte-stream semantics;
* connection orientation;
* three-way handshake;
* SYN / ACK / FIN / RST;
* TCP state machine;
* sequence numbers;
* acknowledgement numbers;
* cumulative ACK concept;
* retransmissions;
* duplicate data handling;
* ordering;
* checksum;
* RTT/RTO concept;
* sliding window;
* receiver window;
* **flow control vs congestion control**;
* slow start conceptually;
* congestion window;
* connection teardown;
* half-close;
* `TIME_WAIT`;
* MSS vs MTU;
* Nagle;
* delayed ACK;
* keepalive;
* TCP vs UDP.

### Central interview question

> **Why is TCP reliable?**

A strong answer isn't:

> Because TCP retransmits packets.

It should connect several mechanisms:

```text
sequence numbers
+ acknowledgements
+ retransmission
+ duplicate detection
+ ordered reconstruction
+ checksum/error detection
+ sender/receiver state
```

Then:

> Does reliable mean delivery is guaranteed no matter what?

No. A connection can fail. TCP provides reliable ordered delivery **while the connection remains viable**, and eventually reports failure rather than magically delivering forever.

### Very important questions

> TCP is a stream. What does that mean?

If the sender does:

```cpp
send("ABC");
send("DEF");
```

the receiver is **not** entitled to observe:

```text
recv() -> "ABC"
recv() -> "DEF"
```

It might observe:

```text
"ABCDEF"
```

or:

```text
"A"
"BCDE"
"F"
```

This connects directly to our later framing task.

### Practical work

**Task A — packet trace**

Capture:

```text
connect
data
shutdown
close
```

and identify:

* SYN;
* SYN/ACK;
* ACK;
* data sequence numbers;
* FINs.

**Task B — TCP state experiment**

Observe sockets through:

```text
ss -tan
```

and find states such as:

```text
LISTEN
ESTABLISHED
TIME-WAIT
```

**Optional experiment**

Introduce artificial delay/loss in a local test environment and watch TCP adapt/retransmit.

---

# 3. UDP, Datagram Semantics and Multicast

For trading, I would definitely give this an independent topic.

TCP is essential for software interviews, but **UDP and multicast are particularly relevant to market-data systems**.

### Literature

**TCP/IP Illustrated**

* Ch. 11 — **UDP**
* Ch. 12 — **Broadcasting and Multicasting**
* Ch. 13 — IGMP, conceptually.

Stevens' UDP chapter also connects UDP with fragmentation, path MTU and ARP. 

**Beej**

* Ch. 5.8 — `sendto()` / `recvfrom()`;
* Ch. 6.3 — Datagram sockets.

Beej explicitly distinguishes stream and datagram programming in its basic client/server material. 

**TLPI**

* Ch. 58–61;
* UDP server design examples.

TLPI's server-design chapter contains both UDP and TCP server examples. 

### Scope

* datagram semantics;
* preservation of message boundaries;
* connectionless communication;
* UDP checksum;
* no built-in:

  * ordering,
  * retransmission,
  * reliability,
  * flow control;
* packet duplication;
* packet reordering;
* packet loss;
* MTU;
* fragmentation risks;
* unicast;
* broadcast;
* multicast;
* multicast groups;
* IGMP at conceptual depth;
* why market feeds can prefer multicast.

### Quant/HFT angle

Imagine the exchange sends:

```text
1001
1002
1003
1005
1006
```

The interesting application question isn't:

> Why did UDP fail?

It is:

> **How does the application detect that message 1004 disappeared, and what does it do next?**

This gets us into:

* sequence numbers;
* gap detection;
* recovery channel;
* snapshots;
* retransmission requests conceptually;
* duplicate handling.

These mechanisms will become extremely useful in the later trading-systems block.

### Practical project — mini market-data feed

Build:

```text
Feed Publisher
      |
     UDP
      |
      v
Feed Receiver
```

Messages contain at minimum:

```cpp
struct Message {
    uint64_t sequence;
    uint32_t instrument;
    int64_t price;
    uint32_t quantity;
};
```

Then add:

* sequence checking;
* dropped-message detection;
* duplicates;
* artificial reorder;
* artificial loss.

Later optionally convert publisher/receiver to multicast.

This is a much more useful exercise than a generic UDP echo server.

---

# 4. Socket Programming, TCP Framing and Serialization

Now we actually write serious socket code.

### Literature

**Beej**

Core reading:

* Ch. 5 — **System Calls or Bust**

  * `getaddrinfo`
  * `socket`
  * `bind`
  * `connect`
  * `listen`
  * `accept`
  * `send/recv`
  * `sendto/recvfrom`
  * `shutdown/close`
* Ch. 6 — **Client-Server Background**
* Ch. 7.4 — **Handling Partial Sends**
* Ch. 7.5 — **Serialization**
* Ch. 7.6 — Data encapsulation/framing.

These topics are all explicitly grouped in Beej's guide. 

**CSAPP Ch. 11**

Especially socket-interface material. CSAPP's client example follows the portable pattern:

```text
getaddrinfo
→ socket
→ connect
```

before returning a file descriptor usable through Unix I/O. 

**TLPI**

* Ch. 56 — socket abstraction;
* Ch. 59 — Internet-domain sockets;
* Ch. 61 — advanced socket I/O.

TLPI explicitly emphasizes that a socket is represented to userspace as a **file descriptor**, tying networking back to our OS block. 

### Scope

Client:

```text
socket
→ connect
→ send/recv
→ close
```

Server:

```text
socket
→ bind
→ listen
→ accept
→ recv/send
→ close
```

Plus:

* `sockaddr`;
* IPv4 vs IPv6;
* `getaddrinfo`;
* network byte order;
* `htons` / `ntohs`;
* blocking sockets;
* EOF;
* `shutdown`;
* partial reads;
* partial writes;
* EINTR/EAGAIN later;
* TCP message framing;
* binary protocol design;
* serialization;
* endian conversion.

### Interview question

> `recv(fd, buf, 4096)` returned 317 bytes. Is something wrong?

No.

TLPI likewise explains that even socket-specific receive APIs can return less data than requested; stream code must be designed around partial I/O. 

### Main practical project — framed TCP protocol

Implement a client/server protocol such as:

```text
[length][type][payload]
```

Do **not** assume one `send()` maps to one `recv()`.

Implement:

```cpp
send_all(...)
read_exact(...)
read_message(...)
```

Then add several message types:

```text
Login
Order
Cancel
Heartbeat
```

This becomes a useful bridge toward the trading-system block.

---

# 5. Nonblocking I/O, `select`, `poll`, `epoll` and Server Architecture

I would absolutely separate this from basic socket programming.

### Literature

**Beej Ch. 7**

* 7.1 Blocking
* 7.2 `poll()`
* 7.3 `select()`

Beej even includes a multi-client `poll()` chat server as a worked example. 

**TLPI**

* Ch. 60 — **Sockets: Server Design**
* Ch. 61 — advanced sockets
* **Ch. 63 — Alternative I/O Models**

TLPI discusses the problem directly: ordinary blocking I/O doesn't work well when one thread must monitor many descriptors; busy polling wastes CPU, while `select()` and `poll()` provide I/O multiplexing. 

TLPI's overall networking sequence explicitly runs from socket fundamentals through server design and advanced sockets to alternative I/O models. 

### Scope

* blocking vs nonblocking;
* `O_NONBLOCK`;
* `EAGAIN` / `EWOULDBLOCK`;
* readiness;
* I/O multiplexing;
* `select`;
* `poll`;
* `epoll`;
* level-triggered vs edge-triggered conceptually;
* accept loop;
* connection state;
* partial read/write state;
* backpressure;
* send buffers;
* receive buffers;
* one-thread-per-connection;
* thread pool;
* event loop;
* reactor pattern conceptually.

### Interview questions

> Why not just make one thread per connection?

> Why not loop over 10,000 nonblocking sockets and call `recv()` on each?

> `select` vs `poll` vs `epoll`?

> What does "socket readable" actually guarantee?

> If a nonblocking `send()` sends only half your message, what happens to the rest?

### Main practical task — `epoll` server

Take the framed TCP server from Topic 4.

Version 1:

```text
one blocking connection
```

Version 2:

```text
thread per client
```

Version 3:

```text
nonblocking sockets + epoll
```

Maintain explicit per-connection state for:

```text
partially received header
partially received payload
pending output
connection close
```

This is excellent systems interview preparation because it combines OS, networking and state-machine design.

---

# 6. Low-Latency Networking for Trading Systems

This should be the synthesis chapter rather than a giant Linux-network-stack course.

### Literature

Reuse:

**TCP/IP Illustrated**

* UDP/multicast chapters;
* TCP interactive flow / Nagle;
* TCP performance material. 

**TLPI Ch. 61**

For:

* socket-specific I/O;
* socket options;
* nonblocking behavior.

For example, TLPI distinguishes descriptor-wide `O_NONBLOCK` from per-call nonblocking behavior through socket flags. 

For current Linux-specific mechanisms such as kernel busy polling/kernel bypass, we'd use current Linux/vendor documentation when we actually reach this topic rather than relying on the older books.

### Scope

First, the normal Linux path:

```text
NIC
 ↓
DMA
 ↓
kernel/network driver
 ↓
packet processing
 ↓
IP
 ↓
UDP/TCP
 ↓
socket receive buffer
 ↓
syscall
 ↓
userspace buffer
 ↓
parser
 ↓
strategy
```

Then performance questions around:

* interrupts;
* polling;
* scheduler interaction;
* syscalls;
* context switches;
* copies;
* socket buffers;
* batching;
* allocation;
* cache locality;
* CPU affinity;
* receive queues conceptually;
* packet loss;
* UDP multicast;
* TCP Nagle;
* `TCP_NODELAY`;
* delayed ACK;
* busy polling;
* zero-copy ideas conceptually;
* kernel bypass:

  * DPDK;
  * AF_XDP;
  * Solarflare/OpenOnload-style approaches conceptually.

I would **not** make you learn DPDK APIs now. The goal is to understand why bypassing parts of the normal kernel path can reduce latency/jitter and what complexity it introduces.

### Important quant interview questions

> Why might TCP be undesirable for some market-data feeds?

> Why might UDP be preferable even though it can lose packets?

> Why can multicast be useful for an exchange?

> Why can `TCP_NODELAY` reduce latency?

> Why can disabling Nagle also reduce efficiency?

> Why might an application busy-poll a NIC/socket?

> Why isn't kernel bypass universally better?

> What happens between a packet hitting the NIC and your C++ strategy seeing it?

---

## Final practical project — market-data network path

This ties together Blocks II–V.

Use:

```text
UDP publisher
        ↓
network
        ↓
receiver thread
        ↓
SPSC queue
        ↓
parser/consumer
```

Reuse our **SPSC ring buffer from the concurrency block**.

Measure at least:

* packets/messages per second;
* end-to-end latency;
* p50;
* p99;
* packet gaps;
* effect of batching.

Then experiment with architectural changes:

```text
socket receive → process immediately
```

versus:

```text
socket receive → SPSC → dedicated processing thread
```

and reason about:

* scheduler;
* cache movement;
* queueing;
* batching;
* syscalls;
* producer/consumer separation.

This becomes a very nice precursor to the later matching-engine/trading-system block.

---

## Final Block V structure

| Topic                                   | Main practical work                            |
| --------------------------------------- | ---------------------------------------------- |
| **1. Network stack + URL journey**      | DNS/routing/ARP/packet-capture lab             |
| **2. TCP internals**                    | TCP packet/state-machine investigation         |
| **3. UDP & multicast**                  | **Market-data feed + sequence-gap detection**  |
| **4. Sockets, framing & serialization** | **Binary framed TCP client/server**            |
| **5. Nonblocking I/O & `epoll`**        | **Multi-client event-driven server**           |
| **6. Low-latency networking**           | **UDP → SPSC → strategy pipeline + benchmark** |

I think **six topics is the right size**. We get substantially deeper than generic backend interview networking, but we don't waste weeks on areas with weak ROI for C++/HFT interviews.

The source stack is also unusually good here: Beej gives us a very direct sockets tutorial whose contents run from socket basics through `bind/connect/listen/accept`, client/server programs, `poll`, `select`, partial sends and serialization;  TLPI gives us the Linux API/server/`epoll` perspective;  and Stevens gives us the packet/protocol mechanics of IP, ARP, UDP, multicast, DNS and TCP. 

That should be enough networking depth before moving into the **trading-systems block**, where these concepts stop being isolated theory and become part of the market-data → order-book → strategy → order-gateway architecture.

---

Yes. With the renumbering we established, this becomes **Block VI — Trading Systems & Practical HFT/IMC Interview**.

I would keep this block deliberately smaller than C++, OS, or concurrency. We are preparing for **software engineering**, not trying to turn the curriculum into a finance degree. That matches IMC's current position: its EU technology internship explicitly says no prior financial-market knowledge is required, and its recruitment guidance says interns/graduates are evaluated primarily on analytical/technical background plus interest in markets. ([IMC Trading][1])

What we *do* want is enough market structure to understand the systems we're designing:

```text
market rules
    ↓
market-data events
    ↓
order book
    ↓
strategy
    ↓
risk
    ↓
order entry
    ↓
exchange
```

And this block should be **heavily practical**. By this point you've already studied C++, OS, concurrency, CPU performance, and networking. Now we force all of them to interact.

---

# BLOCK VI — TRADING SYSTEMS & PRACTICAL HFT INTERVIEW

## 1. Market Microstructure for a Software Engineer

This is the only real "finance theory" chapter we need before building things.

### Literature

### Primary: Larry Harris — *Trading and Exchanges*

Read selectively:

* **Ch. 4 — Orders and Order Properties**
* **Ch. 5 — Market Structures**
* **Ch. 6 — Order-driven Markets**
* selected **Ch. 14 — Bid/Ask Spreads**
* selected **Ch. 19 — Liquidity**

Those chapters form exactly the progression we need: what orders are, how markets are organized, how order-driven markets operate, and then what spread/liquidity mean. ([OUP Academic][2])

We are **not** reading the entire 600-page market-microstructure book.

### Real exchange rules

Use selected sections of the **Euronext Optiq Trading Manual**.

In continuous trading, Euronext describes incoming orders being checked against the opposite side and residual orders being stored according to price/time priority; an incoming order may produce zero, one, or several trades and therefore partial execution is a normal case. ([Euronext][3])

That's particularly useful because we're targeting Amsterdam rather than teaching some fictional exchange.

### Scope

You should understand:

* instrument;
* venue/exchange;
* bid;
* ask/offer;
* spread;
* mid-price;
* quantity;
* market order;
* limit order;
* resting order;
* aggressive/passive order;
* liquidity;
* maker/taker at conceptual level;
* price priority;
* time priority;
* FIFO at a price level;
* trade/execution;
* full fill;
* partial fill;
* cancellation;
* modification;
* crossing prices;
* best bid / best ask;
* top of book;
* depth of book.

Then just enough language to understand:

```text
BUY 100 @ 99
BUY 200 @ 98
----------------
SELL 150 @ 101
SELL 300 @ 102
```

### Interview questions

> What is the spread?

> Why is the highest buy order called the best bid?

> What happens if I submit a buy limit at 102?

> At which price does it trade?

> If two orders have the same price, which one executes first?

> Can one incoming order produce multiple trades?

> What's the difference between an order and a trade?

### Practical work — manual exchange simulation

I give you an empty book and approximately 20 events:

```text
ADD BUY  10 @ 100
ADD BUY   5 @ 101
ADD SELL  4 @ 103
ADD SELL  8 @ 101
CANCEL ...
...
```

After every event you must give me:

* best bid;
* best ask;
* book state;
* generated trades;
* remaining quantities;
* which order has time priority.

No code yet.

By the end you should be able to simulate price-time matching almost mechanically.

---

# 2. Limit Order Book — Data Structure Design Interview

This should be a **design interview before implementation**.

You're given something like:

```cpp
OrderId addOrder(Side side, Price price, Quantity qty);
bool cancelOrder(OrderId id);

std::optional<Price> bestBid() const;
std::optional<Price> bestAsk() const;
```

And I don't tell you which containers to use.

### Literature

No major new book.

Use:

* Harris Ch. 6 — order-driven markets; ([OUP Academic][2])
* Euronext price/time rules as the behavioral specification; ([Euronext][3])
* our earlier C++ container chapters;
* our CPU/cache and allocator chapters.

This is exactly where those earlier topics become useful.

---

## Design requirements

We gradually reveal requirements:

### V1

```text
add
bestBid
bestAsk
```

### V2

```text
cancel(id)
```

### V3

```text
FIFO inside each price level
```

### V4

```text
10 million live orders
```

### V5

```text
price range known / unknown
```

### V6

```text
very frequent cancel/replace
```

Now compare possible representations.

### `std::map<Price, PriceLevel>`

Advantages:

* ordered;
* easy best-price discovery;
* O(log P) level insertion/removal.

Costs:

* node allocation;
* pointer chasing;
* locality.

### `std::unordered_map`

Useful for:

```text
OrderId → Order*
```

but terrible by itself for finding best price.

### Heap

Potentially attractive for best bid/ask.

Then:

> How do you efficiently cancel an arbitrary order?

Suddenly the design becomes more complicated.

### Dense array indexed by price

Could be excellent if tick range is bounded/suitable.

Potentially:

```text
O(1)
excellent locality
no tree traversal
```

but huge/sparse price domains change the answer.

### FIFO price levels

We may consider:

```text
PriceLevel
  ├── Order
  ├── Order
  └── Order
```

using list-like/intrusive links.

---

## Interview targets

You must answer:

> Complexity of add?

> Complexity of cancel?

> Complexity of best bid?

> How do you find an order by ID?

> How do you preserve FIFO?

> Where do allocations happen?

> Which structures are pointer-heavy?

> Which hot data should be contiguous?

> Would your answer change if the price universe were small?

> What if 95% of messages are cancel/replace?

---

## Practical task

At the end of the design interview you produce the interfaces and internal structures, but **you don't implement the entire engine yet**.

You should be able to draw something like:

```text
              OrderId index
                  |
             unordered_map
                  |
                  v

Bid levels                        Ask levels
101 ─────> [A][B][C]         102 ─────> [D][E]
100 ─────> [F][G]            103 ─────> [H]
 99 ─────> [I]               104 ─────> [J][K]

       price priority
             +
       FIFO per level
```

and defend every component.

---

# 3. Matching Engine — Core Implementation

Now we turn the design into interview-quality C++.

### Literature

Again, no finance textbook is needed.

Behavior comes from the real exchange model: Euronext's continuous order-driven model checks incoming orders for execution against the opposite side and applies price/time priority to resting orders. ([Euronext][3])

Use previous blocks for implementation:

* C++ object/lifetime/container knowledge;
* allocators;
* CPU locality;
* exception safety;
* testing.

---

## Initial scope

Support:

```cpp
addOrder(...)
cancelOrder(...)
```

with:

```text
BUY
SELL

LIMIT orders

price-time priority

partial fills
```

For example:

```text
ASKS

101: S1 5
102: S2 7

Incoming:
BUY 10 @ 102
```

should produce something conceptually like:

```text
trade S1: 5 @ 101
trade S2: 5 @ 102

S2 remains 2 @ 102
```

---

## Required invariants

Examples:

* no zero-quantity live orders;
* no duplicate live `OrderId`;
* FIFO within a price level;
* bid side ordered highest → lowest;
* ask side ordered lowest → highest;
* matching stops when prices no longer cross;
* executed quantity never exceeds remaining quantity.

### Interview questions while coding

I will interrupt with questions such as:

> Why did you choose this representation?

> What's the complexity of this line?

> Can this iterator be invalidated?

> Can `cancel` leave a dangling pointer?

> Who owns an `Order`?

> What if matching deletes the price level you're currently iterating over?

> What if the incoming order is fully filled?

> What if it crosses three price levels?

---

## Practical target

Something in the shape of:

```cpp
class MatchingEngine {
public:
    OrderId add(Side side, Price price, Quantity qty);
    bool cancel(OrderId id);

private:
    // your design
};
```

plus executions:

```cpp
struct Trade {
    OrderId restingOrder;
    OrderId aggressingOrder;
    Price price;
    Quantity quantity;
};
```

But **you design the actual representation**.

I don't give you a finished architecture beforehand.

---

# 4. Matching Engine — Correctness, Replay and Performance

This is where the little coding exercise starts behaving like a systems component.

### Literature

### Market-data protocol example: Nasdaq TotalView-ITCH

Use selected parts of the current **TotalView-ITCH 5.0 specification**.

We don't learn the whole protocol. We study the event-oriented model:

```text
Add Order
Order Executed
Order Cancel
Order Delete
Order Replace
Trade
```

The specification, for example, defines order-replace messages carrying the old and new order reference numbers, quantity, and price; trade/execution messages can represent partial execution. ([NASDAQ Trader][4])

This is extremely useful because it demonstrates how a real order book can be represented as an **ordered event stream**, not just as a mutable C++ class.

### Optional European equivalent

Euronext currently publishes separate Optiq specifications for:

* Market Data Gateway;
* Order Entry Gateway;
* SBE message formats;
* FIX order-entry messages.

We won't learn the protocol exhaustively, but because Amsterdam is the target, I'll use selected real Optiq messages/examples where useful. ([Connect Euronext][5])

---

## Extend the engine

Add:

```text
unique OrderId
cancel
replace / modify
invalid input
event sequence
trade events
```

Then introduce:

```text
Event Log
```

For example:

```text
1 ADD    ...
2 ADD    ...
3 TRADE  ...
4 CANCEL ...
5 ADD    ...
```

### Deterministic replay

Start with:

```text
empty state
+
event log
=
same book
```

This gives us a powerful correctness property.

Then discuss:

* snapshot;
* event log;
* replay;
* recovery;
* sequence number;
* missing event;
* duplicate event;
* deterministic state reconstruction.

Notice how this connects directly to our UDP market-data work.

---

## Testing

We build much stronger tests than:

```cpp
EXPECT_EQ(book.bestBid(), 100);
```

We test invariants such as:

```text
bestBid < bestAsk
```

for a non-crossed resting book,

and:

```text
sum(original qty)
=
sum(remaining qty)
+
sum(executed qty)
+
sum(cancelled qty)
```

where appropriate.

Test:

* empty book;
* one order;
* full fill;
* partial fill;
* multiple price levels;
* multiple orders at same level;
* cancel first/middle/last;
* cancel nonexistent;
* replace;
* duplicate ID;
* replay.

---

## Performance interview

Now I begin asking:

> Where does the engine allocate?

> How many pointer dereferences does matching require?

> What's the hottest object?

> How large is an `Order`?

> What's the working set?

> Does the ID hash table point to stable objects?

> What does cancel do to cache locality?

> Which operations are O(log n)?

> Does Big-O even capture the biggest cost?

> What changes with ten million orders?

> Can we preallocate?

> Arena?

> Pool?

> Intrusive structure?

> Dense price array?

> Can we keep the top few price levels hotter?

This chapter ties **Block I + II + IV** together.

---

## Practical work — benchmark

Generate synthetic event streams.

For example:

```text
40% add
40% cancel
20% aggressive orders
```

then other mixes.

Measure:

```text
events/sec
mean latency
p50
p95
p99
```

Then profile with the methodology from Block IV.

No fake "we optimized it because unordered_map is O(1)" reasoning.

---

# 5. Low-Latency Trading System Design

Now we zoom out from the exchange simulator.

This is **systems design**, but not a standard "design Twitter" distributed-systems interview.

### Architecture

Start from:

```text
                  EXCHANGE
                     |
               Market Data
                     |
                     v
            Network Receiver
                     |
                     v
                  Parser
                     |
                     v
                Order Book
                     |
                     v
                 Strategy
                     |
                     v
                   Risk
                     |
                     v
              Order Gateway
                     |
                     v
                  EXCHANGE
```

Then make it more realistic:

```text
      Market Data Feed
             |
          NIC/RX
             |
        Feed Handler
             |
       Book Builder
             |
          Strategy
             |
         Risk Check
             |
      Order Entry GW
             |
          Exchange

       + logging
       + monitoring
       + replay
       + recovery
```

---

## Literature

This chapter intentionally reuses everything we've already studied rather than introducing another giant textbook.

### Protocol context

Selected real exchange interfaces:

* Nasdaq ITCH — market-data/event-stream example; ([NASDAQ Trader][4])
* Euronext Optiq MDG — market data;
* Euronext Optiq OEG — order entry. ([Connect Euronext][5])

### Reuse previous sources

Networking:

* Beej/TLPI;
* TCP/IP Illustrated.

Concurrency:

* *C++ Concurrency in Action*.

CPU:

* CSAPP Ch. 5–6.

OS:

* scheduling, page faults, syscalls, affinity.

---

## Design dimensions

For every box we ask:

### Network

* TCP or UDP?
* multicast?
* packet gaps?
* syscalls?
* batching?
* kernel bypass?

### Memory

* allocations?
* preallocation?
* page faults?
* huge pages?
* working set?

### CPU

* cache locality?
* branch prediction?
* data layout?
* SIMD relevant?
* instruction-cache footprint?

### Concurrency

* shared-memory structure?
* mutex?
* SPSC queue?
* single-thread ownership?
* thread per stage?
* busy polling?
* CPU affinity?

### Correctness

* packet loss;
* duplicate messages;
* recovery;
* sequence gaps;
* stale state;
* rejected orders;
* exchange disconnect.

### Latency

We distinguish:

```text
median
p99
p99.9
jitter
```

instead of simply:

> "This design is fast."

---

## Central interview scenario

I say:

> We're receiving 2 million market-data messages per second. The strategy cares primarily about the top 5 levels. p99 latency is too high. What do you investigate?

I expect you to ask questions before proposing random optimizations.

Then follow-ups:

> CPU usage is only 40%.

> There are page faults.

> One core is at 100%.

> `shared_ptr` is everywhere.

> market-data and strategy threads migrate between cores.

> p50 is excellent but p99.9 is terrible.

> allocations disappear, but performance barely changes.

> `perf` shows LLC misses.

This is where all five previous blocks finally converge.

---

# 6. Practical Existing-Codebase Interview

I strongly agree with keeping this.

There is some anecdotal evidence for an IMC interview style close to this: one reported process involved first analyzing a long problem/design, discussing appropriate data structures and complexity, then being given several files/classes and implementing the discussed functionality; that report mentions heaps, priority queues, BSTs, C++, and OOP as part of the exercise. This is a candidate report, not a guarantee that Amsterdam uses the identical format. ([Glassdoor][6])

IMC's official description is broader: the process includes an assessment and technical interviews/stations built around solving problems with engineers. ([IMC Trading][7])

So this exercise is a very reasonable simulation without pretending we know the exact question you'll receive.

---

## Format

You receive a repository such as:

```text
src/
    order.hpp
    order_book.hpp
    order_book.cpp
    matching_engine.hpp
    matching_engine.cpp
tests/
    ...
```

You **do not write code immediately**.

### Stage 1 — Read

Explain:

* classes;
* ownership;
* data flow;
* invariants;
* complexity;
* suspicious areas.

### Stage 2 — Requirements interview

I give you something like:

> Add cancel/replace support.

or:

> Add a query for top N levels.

or:

> We now need multiple instruments.

or:

> Replay must be deterministic.

or:

> This method now appears on the hot path.

### Stage 3 — Clarify

You ask questions.

I specifically grade whether you blindly make assumptions.

### Stage 4 — Design

Explain:

* proposed change;
* alternative;
* complexity;
* lifetime effects;
* invalidation;
* concurrency implications.

### Stage 5 — Implement

You modify unfamiliar code rather than starting from a blank LeetCode editor.

### Stage 6 — Debug

Run tests.

Some requirements may intentionally break existing assumptions.

### Stage 7 — Performance follow-up

I ask:

> What became more expensive?

> Any new allocation?

> Iterator invalidation?

> Thread safety?

> Cache effects?

> Exception safety?

> How would you test this at production scale?

---

# 7. FINAL IMC / HFT SWE MOCK

No theory beforehand.

This is an actual assessment.

Given current IMC reports, keeping serious DSA is essential: recent Amsterdam internship reports still describe difficult OA algorithmic problems, including optimization/DP, and broader reports mention medium/hard DSA. ([Glassdoor][8])

IMC's technology internship also currently states that candidates take a coding assessment in C++ or Java. ([IMC Trading][1])

So the final mock should **not** become only a matching-engine interview.

---

## Stage A — C++ / Systems Fundamentals

Approximately 45–60 minutes.

Random questions across:

```text
C++
object model
templates
exceptions
STL
allocators
OS
virtual memory
TLB
syscalls
CPU caches
branch prediction
compiler optimization
concurrency
memory ordering
networking
```

Example progression:

> What does `std::move` do?

then:

> What happens with `const T&&`?

then:

> How can `vector` use `noexcept` move?

then:

> What happens during reallocation?

then:

> Where does the new allocation come from?

then:

> What if touching the new pages causes page faults?

That's the style I want: **follow the mechanism across layers**.

---

## Stage B — DSA Coding

Timed problem.

Could involve:

* graph;
* DP;
* heap;
* hash table;
* scheduling;
* binary search;
* custom data structure.

Requirements:

1. clarify;
2. brute force;
3. derive better solution;
4. complexity;
5. implement in C++;
6. test;
7. edge cases;
8. follow-up requirement.

This keeps the algorithmic part we deliberately restored earlier.

---

## Stage C — Practical Coding

Unfamiliar mini-codebase.

Something related to:

```text
orders
event processing
scheduler
cache
data feed
stateful API
```

but it does not have to literally be a matching engine.

You receive a new requirement and modify the code.

---

## Stage D — Performance/System Design

Approximately:

> Design the path from exchange market data to strategy and order gateway.

Then progressively:

> Where is state?

> How many threads?

> Why?

> What is shared?

> Which queue?

> What if messages are dropped?

> What happens during overload?

> Where could p99 spikes come from?

> What would you measure first?

---

## Stage E — Project Deep Dive

Especially your strongest C++ project.

Questions:

> What did you personally implement?

> Hardest technical decision?

> What alternatives did you reject?

> What's the complexity?

> What would break at 100× scale?

> What was your performance bottleneck?

> How did you prove it was the bottleneck?

> What did the generated assembly look like?

> What did you learn?

The matrix library is very useful here because by then we'll have used it for cache blocking, layout experiments, compiler optimization, SIMD and profiling.

---

## Stage F — Behavioral / IMC

Current IMC recruiting says initial/final rounds can contain both technical and behavioural components, so this should stay in the final simulation. ([IMC Trading][7])

Questions such as:

```text
Why IMC?
Why trading technology?
Why C++?
Why not ordinary backend SWE?
Tell me about a disagreement.
Tell me about a failure.
Feedback you initially disagreed with.
Situation with incomplete information.
A time you changed your approach after data contradicted you.
```

No memorized corporate speeches.

---

# Final scoring

After the entire mock:

| Area                                           | Score |
| ---------------------------------------------- | ----: |
| **C++ language/object model**                  |   /10 |
| **STL/templates/resource management**          |   /10 |
| **DSA/problem solving**                        |   /10 |
| **OS & memory**                                |   /10 |
| **CPU/performance**                            |   /10 |
| **Concurrency/memory model**                   |   /10 |
| **Networking**                                 |   /10 |
| **Trading-system reasoning**                   |   /10 |
| **Practical codebase work**                    |   /10 |
| **Communication / English technical delivery** |   /10 |

And for each area:

```text
Strong
Interview-ready
Needs review
Major gap
```

plus exact mistakes and what to revise.

---

# Final Block VI structure

| Topic                                        | Main practical work                                            |
| -------------------------------------------- | -------------------------------------------------------------- |
| **1. Market microstructure for SWE**         | Manual price-time order-book simulation                        |
| **2. Limit-order-book data structures**      | **Pure design interview + complexity/performance defence**     |
| **3. Matching engine core**                  | **Add/cancel/match + FIFO + partial fills**                    |
| **4. Correctness, replay & performance**     | **Event log + replay + tests + benchmark**                     |
| **5. Low-latency trading-system design**     | **Market data → strategy → risk → order gateway interview**    |
| **6. Existing-codebase practical interview** | **Read → clarify → design → implement → debug → optimize**     |
| **7. Final IMC/HFT SWE mock**                | Fundamentals + DSA + codebase + systems + project + behavioral |

I think this is stronger than adding more finance. In particular, I would **not add options pricing, Greeks, stochastic calculus, portfolio theory, or trading strategies to the SWE curriculum right now**. IMC explicitly says the technology internship doesn't require prior market knowledge; the relevant advantage for us is being able to discuss a real trading system naturally and connect it to C++, memory, CPUs, concurrency and networking. ([IMC Trading][1])

The most useful addition over your original version is the **real event-stream/protocol perspective** in Topic 4. A matching engine alone teaches data structures. An event log + sequence numbers + replay + real ITCH/Optiq-style messages turns it into a much more realistic systems-engineering exercise.

[1]: https://www.imc.com/eu/careers/students-graduates/internships/technology-internship?utm_source=chatgpt.com "Technology internship - EU | IMC Trading"
[2]: https://academic.oup.com/book/52292?utm_source=chatgpt.com "Trading and Exchanges: Market Microstructure for Practitioners | Oxford Academic"
[3]: https://www.euronext.com/sites/default/files/2025-02/Trading%20Manual%20for%20the%20Optiq%20Trading%20Platform%20with%20new%20pre-trade%20controls%20and%20iceberg%20precisions%20-%20150724%20.pdf?utm_source=chatgpt.com "Insert Document Title"
[4]: https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf?utm_source=chatgpt.com "Nasdaq TotalView-ITCH 5.0"
[5]: https://connect.euronext.com/en/membership/resources/it-documentation/by-product?page=1&product_target_id=1611&utm_source=chatgpt.com "IT Documentation by Product / Service | Connect"
[6]: https://www.glassdoor.com/Interview/This-was-a-campus-interview-at-IITB-Before-the-interview-there-were-two-rounds-OA-They-gave-3-coding-questions-of-Leet-QTN_8745658.htm?utm_source=chatgpt.com "IMC Trading Interview Question: This was a campus interview at IITB. Before the interview, there were two rounds. OA: They gave 3 coding questions of Leetcode Medium/Hard difficulty, all of them on graphs. 1 question was there to test the debugging skills in OOPS. All of the coding questions were to be attempted in C++. Group Discussion: We were given a piece of paper with some C++ code on it and we had to answer some questions in a group. What is this code doing? What is this class's main function? What bugs do you see here? What possible optimizations can you suggest here? People needed to raise their hands to answer. Interviewers were helpful ad helped us arrive to the answers. Now the interview rounds. Round 1: DSA Pen+Paper Round. I was given a 10 page problem statement and the interviewers left me alone in the room and told me that I can read the problem statement on my own for 20 minutes. After 20 minutes, when they came back, they asked me my approach to solve the problem. What data structure would you use? How would you optimize this functionality if you don't care about the other one? In each case, I was asked the best case and worst case time complexity. Round 2: Coding Round I was asked to code the same thing, but they said more than me passing all the test cases they were interested in the way I approach the coding. So I was given few files, with different classes, I needed to analyze and implement whatever we discussed in the previous round. The topics that were asked were heap, priority queue, binary search tree, and in-depth C++ and OOPs. | Glassdoor"
[7]: https://www.imc.com/ap/articles/how-to-prepare-for-an-interview-at-imc?utm_source=chatgpt.com "How to prepare for an interview at IMC | IMC Trading"
[8]: https://www.glassdoor.com/Interview/IMC-Trading-Interview-Questions-E278100.htm?filter.jobTitleExact=Software+Engineer%28Internship%29&utm_source=chatgpt.com "IMC Trading Software Engineer(Internship) Interview Experience & Questions | Glassdoor"
