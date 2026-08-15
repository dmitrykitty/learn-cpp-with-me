Tak. Ustaliłbym to jako **docelowy syllabus 60 tematów**, który potem już tylko realizujemy. Nie będziemy go rozszerzać za każdym razem, gdy przypomnimy sobie kolejne słowo z C++ — zakres jest już wystarczająco szeroki.

To nie ma być 60 „lekcji”. Każdy temat ma doprowadzić Cię do poziomu:

> **rozumiem → umiem wyjaśnić → potrafię odpowiedzieć na follow-up → potrafię zastosować → potrafię napisać kod bez pomocy.**

Plan jest celowo szerszy niż jedna rozmowa IMC, bo ma jednocześnie budować fundament pod C++/systems/low-latency/HFT, zgodnie z Twoim długoterminowym kierunkiem.  IMC samo zaznacza, że proces zależy od roli, może obejmować assessment oraz kilka interview rounds/stations, a u internów oczekuje przede wszystkim mocnego technicznego i analitycznego fundamentu, a nie wcześniejszego doświadczenia finansowego. ([IMC Trading][1])

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
* `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`,
* kiedy cast oznacza potencjalny design smell.

**Praktyka**
Operacje na surowym buforze oraz kilka zadań typu „what does this code do / is this UB?”.

---

## 3. RAII, ownership i smart pointers

**Źródła**

* *A Tour of C++* — Resource Management
* *Effective Modern C++* — Items 18–22
* cppreference: RAII, `unique_ptr`, `shared_ptr`, `weak_ptr`

**Zakres**

* ownership vs non-owning reference,
* RAII,
* deterministic destruction,
* `unique_ptr`,
* `shared_ptr` control block,
* reference counting,
* cycles,
* `weak_ptr`,
* kiedy smart pointer jest niepotrzebny.

**Praktyka**
Własny RAII wrapper dla zasobu + analiza ownership w kilku API.

---

## 4. Constructors, destructors i Rule of 0/3/5

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
* exception podczas konstrukcji.

**Praktyka**
Rozbieramy Twój `String` i odpowiadamy, które special members compiler wygenerowałby automatycznie i dlaczego.

---

## 5. Value categories, move semantics i copy elision

**Źródła**

* *Effective Modern C++* — Items 23–29
* cppreference: Value categories, Move constructor, Copy elision

**Zakres**

* lvalue,
* xvalue,
* prvalue,
* `std::move`,
* moved-from state,
* move vs copy,
* RVO/NRVO,
* guaranteed copy elision,
* kiedy move wcale nie jest tani.

**Praktyka**
Instrumentowana klasa logująca konstrukcje/copy/move/destruction.

---

## 6. Templates — fundamentals

**Źródła**

* *A Tour of C++* — Templates
* cppreference: Function templates, Class templates, Specialization

**Zakres**

* function/class templates,
* instantiation,
* explicit instantiation,
* specialization,
* partial specialization,
* compile-time polymorphism,
* dlaczego templates zazwyczaj muszą być widoczne w headerach.

**Praktyka**
Napisać prostą generyczną strukturę danych.

---

## 7. Type deduction: `auto`, `decltype`, `T&&`

**Źródła**

* *Effective Modern C++* — Items 1–4
* cppreference: template argument deduction, `auto`, `decltype`

**Zakres**
Dla kodu:

```cpp
template<class T>
void f(T);

template<class T>
void g(T&);

template<class T>
void h(T&&);
```

masz być w stanie określić dokładnie `T` dla różnych argumentów.

Do tego:

* `auto`,
* `const auto&`,
* `decltype`,
* `decltype(auto)`.

**Praktyka**
Duży zestaw deduction puzzles.

---

## 8. Perfect forwarding, reference collapsing i variadic templates

**Źródła**

* *Effective Modern C++* — Items 23–30
* cppreference: forwarding references, parameter packs, fold expressions

**Zakres**

* forwarding reference,
* `std::forward`,
* reference collapsing,
* parameter packs,
* dlaczego `std::move(arg)` jest błędem w generic forwarding function.

**Praktyka**
Mini `make_object()` / factory przekazujące argumenty do konstruktora.

---

## 9. Concepts, `requires` i constraints

**Źródła**

* *A Tour of C++* — Concepts and Generic Programming
* cppreference: Constraints and concepts, Requires expression
* podstawowe porównanie z SFINAE

**Zakres**

* concept,
* constraint,
* requires-clause,
* requires-expression,
* compile-time interface,
* concepts vs inheritance,
* dlaczego concepts poprawiają diagnostics.

**Praktyka**
Zaprojektować concepts dla części Twojej biblioteki macierzowej.

---

## 10. Inheritance, name lookup i slicing

**Źródła**

* *A Tour of C++* — Classes/Class Hierarchies
* cppreference: Derived classes, Name lookup, Overload resolution

**Zakres**

* public/protected/private inheritance,
* base subobject,
* construction/destruction order,
* object slicing,
* overload hiding,
* `using Base::f`,
* name lookup vs overload resolution.

**Praktyka**
Wracamy m.in. do:

```cpp
Base::f(int, int);
Derived::f(int);

Derived d;
d.f(1, 2);
```

i wyjaśniasz dokładnie **dlaczego C++ zachowuje się tak, a nie inaczej**.

---

## 11. Virtual functions, vtable i dynamic dispatch

**Źródła**

* *A Tour of C++* — Class Hierarchies
* cppreference: Virtual function, Abstract class, `override`, `final`
* opcjonalnie Compiler Explorer

**Zakres**

* dynamic dispatch,
* virtual destructor,
* pure virtual,
* vptr/vtable jako typowa implementacja ABI,
* devirtualization,
* indirect branch,
* możliwości i ograniczenia inliningu.

**Praktyka**
Hierarchia klas + oglądanie wygenerowanego assembly.

**Interview**
„What is the runtime cost of a virtual function call?”

---

## 12. Multiple inheritance i virtual inheritance

**Źródła**

* cppreference: Derived classes / virtual base classes
* *A Tour of C++* — class hierarchies

**Zakres**
Diamond:

```text
    A
   / \
  B   C
   \ /
    D
```

* dwa `A`,
* virtual inheritance,
* shared virtual base,
* construction order,
* object layout,
* pointer adjustment,
* runtime/space implications.

**Praktyka**
Sprawdzamy `sizeof`, adresy subobjects i casty w kilku hierarchiach.

---

## 13. Static polymorphism, CRTP i type erasure

**Źródła**

* cppreference: CRTP
* *A Tour of C++* — Templates
* opcjonalnie C++ Core Guidelines

**Zakres**
Porównujemy:

```cpp
virtual void process();
```

z compile-time polymorphism.

Do tego:

* CRTP,
* `std::variant`,
* type erasure / `std::function`,
* trade-off runtime vs compile time/binary size.

**Praktyka**
Dwie implementacje tego samego interfejsu: virtual i template-based.

---

## 14. STL containers, iterators i invalidation

**Źródła**

* *A Tour of C++* — Containers, Algorithms
* cppreference: container requirements
* *Effective STL* opcjonalnie

**Zakres**

* `vector`,
* `deque`,
* `list`,
* `map`,
* `unordered_map`,
* iterators,
* reallocation,
* iterator/reference invalidation,
* complexity guarantees,
* cache locality.

**Praktyka**
Dla 10 scenariuszy wybierasz container i bronisz wyboru.

---

## 15. Allocators, `std::pmr`, arenas i exception safety

**Źródła**

* cppreference: Allocator, `<memory_resource>`
* *A Tour of C++* — memory/resources
* CSAPP — Dynamic Memory Allocation

**Zakres**

* koszt `new/delete`,
* arena/pool/bump allocation,
* fragmentation,
* `std::pmr`,
* strong/basic/no-throw exception guarantee,
* dlaczego low-latency system może unikać allocation na hot path.

**Praktyka**
Prosty arena allocator + `pmr::vector`.

---

## 16. Compilation model, ODR, linking, ABI i zero-cost abstractions

**Źródła**

* cppreference: ODR, translation phases, `inline`, templates, zero-overhead principle
* *A Tour of C++* — Modularity
* Compiler Explorer

**Zakres**

* preprocessing → compilation → assembling → linking,
* translation unit,
* symbols,
* static/dynamic libraries,
* ODR,
* `inline`,
* template instantiation,
* ABI,
* zero-cost abstractions,
* as-if rule.

**Praktyka**
Celowo tworzymy linker errors, ODR violations i oglądamy symbols przez `nm`.

---

# BLOK II — DATA STRUCTURES & ALGORITHMS / OA

## 17. Complexity i amortized analysis

**Źródła**

* CLRS — Growth of Functions
* *Competitive Programmer's Handbook* — Time Complexity

**Zakres**

* O, Ω, Θ,
* time vs space,
* worst/average/amortized,
* geometric growth,
* amortized `vector::push_back`.

**Praktyka**
Analiza 15 fragmentów kodu + ręczne wyprowadzenie amortized O(1).

---

## 18. Stack, queue, deque i linked list

**Źródła**

* CLRS — Elementary Data Structures
* CPH — Data Structures

**Zakres**

* implementations,
* complexity,
* monotonic stack/queue,
* linked list locality.

**Coding**

* stack od zera,
* queue,
* **special-stack style problem podobny do raportowanych IMC OA**,
* jeden monotonic-stack problem.

---

## 19. Hash tables od środka

**Źródła**

* CLRS — Hash Tables
* cppreference: `unordered_map`

**Zakres**

* hashing,
* collisions,
* chaining,
* open addressing,
* linear probing,
* load factor,
* rehashing,
* tombstones,
* average O(1) vs worst O(n),
* cache locality.

**Coding**
Własny `HashMap<Key, Value>`.

**Interview**
„How would you implement a hash map?”

---

## 20. Trees, balanced trees, heaps i priority queues

**Źródła**

* CLRS — BST / Red-Black Trees / Heaps
* CPH

**Zakres**

* BST,
* degeneration,
* balanced trees conceptually,
* binary heap,
* priority queue,
* tree vs hash map.

**Coding**
Binary heap + kilka operacji BST.

---

## 21. Sorting i greedy algorithms

**Źródła**

* CLRS — Sorting / Greedy Algorithms
* CPH — Sorting and Searching / Greedy

**Zakres**

* comparison sorting,
* `std::sort`,
* stable sorting,
* greedy-choice property,
* proof/exchange argument.

**Coding**
2 medium greedy problems.

---

## 22. Binary search i prefix sums

**Źródła**

* CPH — Sorting and Searching
* prefix sums / range query section

**Zakres**

* lower/upper bound,
* binary search on answer,
* 1D/2D prefix sums,
* overflow i boundary bugs.

**Coding**
Jedno zwykłe BS, jedno BS-on-answer i problem macierzowy.

---

## 23. Two pointers i sliding window

**Źródła**

* CPH
* wybrane medium LeetCode/Codeforces

**Zakres**
Rozpoznawanie, kiedy można przesuwać granice monotonicznie.

**Coding**
3 różne problemy bez informacji, którego patternu użyć.

---

## 24. Graphs: BFS, DFS i shortest paths

**Źródła**

* CPH — Graph Traversal / Shortest Paths
* CLRS — Elementary Graph Algorithms

**Zakres**

* graph representation,
* BFS,
* DFS,
* connected components,
* cycles,
* topological ordering,
* Dijkstra,
* kiedy BFS wystarcza.

**Coding**
Obowiązkowo problem **knight-moves/BFS style**.

---

## 25. Dynamic programming

**Źródła**

* CPH — Dynamic Programming
* CLRS — Dynamic Programming

**Zakres**

* state,
* transitions,
* base cases,
* top-down vs bottom-up,
* space optimization,
* rozpoznawanie DP.

**Coding**
1 medium + 1 trudniejszy problem.

---

## 26. IMC-style Mock OA #1

**Źródła**
Brak nowej teorii — powtórka 17–25.

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

## 27. Processes i address space

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

## 28. `fork`, `exec`, `wait` i process API

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

## 29. User mode, kernel mode, syscalls i context switches

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

## 30. Virtual memory i address translation

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

## 31. Paging, page tables i TLB

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

## 32. Page faults, demand paging, swapping, COW i `mmap`

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

## 33. `malloc`, `free` i memory allocators

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

## 34. File descriptors i Unix I/O

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

## 35. Memory bugs, UB i debugging tools

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

# BLOK IV — CPU ARCHITECTURE & PERFORMANCE

## 36. Data representation, alignment, padding i object layout

**Źródła**

* CSAPP Ch. 2–3
* cppreference: `sizeof`, `alignof`, object representation

**Zakres**

* integers,
* endianness,
* alignment,
* padding,
* struct layout,
* data packing,
* AoS vs SoA.

**Praktyka**
Optymalizacja layoutu:

```cpp
struct Order {
    ...
};
```

i ręczne przewidywanie `sizeof`.

---

## 37. CPU cache hierarchy i locality

**Źródła**

* CSAPP Ch. 6 — The Memory Hierarchy

**Zakres**

* L1/L2/L3,
* cache line,
* temporal/spatial locality,
* cache hit/miss,
* working set,
* prefetching.

**Coding**
Row-major vs column-major traversal macierzy + benchmark.

Idealnie wykorzystamy też Twój matrix project.

---

## 38. Pipelines, superscalar execution, OoO i dependencies

**Źródła**

* CSAPP Ch. 5 — Optimizing Program Performance
* Agner Fog — opcjonalnie jako głębsze źródło

**Zakres**

* pipeline,
* IPC,
* instruction latency vs throughput,
* dependencies,
* out-of-order execution,
* execution ports — tylko koncepcyjnie na tym etapie.

**Interview**
Dlaczego dwie pętle o takim samym Big-O mogą mieć zupełnie inną wydajność?

---

## 39. Branch prediction i speculation

**Źródła**

* CSAPP Ch. 5
* opcjonalnie Agner Fog

**Zakres**

* conditional branches,
* prediction,
* misprediction,
* speculative execution,
* branchless programming,
* dlaczego branchless != always faster.

**Coding**
Benchmark predictable vs random branches.

---

## 40. Compiler optimizations, inlining i devirtualization

**Źródła**

* cppreference: as-if rule
* Compiler Explorer
* CSAPP Ch. 5

**Zakres**

* `-O0/-O2/-O3`,
* inlining,
* dead-code elimination,
* vectorization,
* constant propagation,
* devirtualization,
* benchmark traps.

**Praktyka**
Kod C++ → assembly → interpretacja optymalizacji.

---

## 41. SIMD, profiling i performance methodology

**Źródła**

* CSAPP Ch. 5–6
* Intel Intrinsics Guide później jako reference
* Linux `perf`

**Zakres**

* SIMD idea,
* auto-vectorization,
* SIMD vs scalar,
* measurement before optimization,
* CPU time vs wall time,
* hotspots,
* hardware counters.

**Praktyka**
Profilujemy fragment matrix library i próbujemy znaleźć realny bottleneck.

---

# BLOK V — CONCURRENCY & C++ MEMORY MODEL

OSTEP posiada osobne rozdziały 26–33 o threads, thread API, locks, locked structures, condition variables, semaphores i concurrency bugs. ([UW Computer Sciences][3])

## 42. Threads i scheduling

**Źródła**

* OSTEP Ch. 26–27
* *C++ Concurrency in Action*, Ch. 1–2
* CSAPP Ch. 12

**Zakres**

* thread vs process,
* shared/private state,
* stacks,
* scheduler,
* thread creation,
* context switching.

**Coding**
`std::thread` / `std::jthread`.

---

## 43. Race condition, data race i critical section

**Źródła**

* OSTEP Ch. 26
* *C++ Concurrency in Action* Ch. 3

**Zakres**

* race condition vs C++ data race,
* atomicity,
* critical section,
* synchronization,
* invariants.

**Coding**
Celowo buggy concurrent counter → naprawa.

---

## 44. Mutex, locks i condition variables

**Źródła**

* OSTEP Ch. 28 i 30
* *C++ Concurrency in Action* Ch. 3–4

**Zakres**

* mutex,
* `lock_guard`,
* `unique_lock`,
* contention,
* condition variable,
* predicate,
* spurious wakeups.

**Coding**
Blocking producer-consumer queue.

---

## 45. Deadlock, livelock, starvation i semaphores

**Źródła**

* OSTEP Ch. 31–32
* C++ Concurrency in Action

**Zakres**

* Coffman conditions,
* lock ordering,
* `std::scoped_lock`,
* deadlock avoidance,
* livelock,
* starvation,
* semaphore.

**Coding**
Stworzyć deadlock → znaleźć → naprawić.

---

## 46. Atomics i Compare-And-Swap

**Źródła**

* *C++ Concurrency in Action* Ch. 5
* cppreference: `std::atomic`

**Zakres**

* atomic operations,
* RMW,
* CAS,
* `compare_exchange_weak/strong`,
* lock-free property.

**Coding**
CAS counter i prosty concurrent state transition.

---

## 47. C++ memory model i happens-before

**Źródła**

* *C++ Concurrency in Action* Ch. 5
* cppreference: Memory model

**Zakres**

* sequenced-before,
* synchronizes-with,
* happens-before,
* visibility,
* modification order,
* compiler vs CPU reorderings.

To będzie jeden z najgłębszych dni.

---

## 48. Memory ordering

**Źródła**

* *C++ Concurrency in Action* Ch. 5
* cppreference: `memory_order`

**Zakres**

* `seq_cst`,
* `acquire`,
* `release`,
* `acq_rel`,
* `relaxed`.

**Coding**
Message passing + reasoning, jakie wyniki programu są legalne.

---

## 49. Cache coherence, false sharing i lock-free structures

**Źródła**

* C++ Concurrency in Action Ch. 7
* CSAPP Ch. 6 + 12

**Zakres**

* cache coherence conceptually,
* false sharing,
* CAS loops,
* ABA,
* lock-free/wait-free/blocking,
* SPSC queue.

**Coding**
Najpierw benchmark false sharing, potem prosty SPSC ring buffer.

---

# BLOK VI — NETWORKING

Beej's Guide jest nastawiony bezpośrednio na Internet sockets i zakłada znajomość C lub C++, więc będzie tutaj bardzo praktycznym źródłem. ([Beej][4])

## 50. „What happens when you type a URL?”

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

## 51. TCP głęboko

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

## 52. Socket programming i multiplexing

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

## 53. Networking w low-latency systems

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

# BLOK VII — TRADING SYSTEMS + IMC PRACTICAL INTERVIEW

Tutaj nie robimy kursu finansów. IMC oficjalnie zaznacza, że dla internów i graduate'ów nie wymaga wcześniejszego doświadczenia w financial markets; ważniejsze są background techniczny/analityczny i zainteresowanie rynkiem. ([IMC Trading][1])

## 54. Market microstructure dla software engineera

**Źródła**

* podstawowe materiały o limit order books/exchanges
* będziemy dobierać aktualne źródła podczas tego dnia

**Zakres**

* bid,
* ask,
* spread,
* market order,
* limit order,
* liquidity,
* maker/taker,
* price-time priority,
* partial fill.

**Praktyka**
Ręcznie wykonujesz sekwencję orders na order booku.

---

## 55. Order book — data structures

Dostajesz wymagania:

```cpp
addOrder()
cancelOrder()
bestBid()
bestAsk()
```

i sam projektujesz strukturę.

Rozważamy:

* `map`,
* `unordered_map`,
* heap,
* arrays,
* intrusive lists,
* price levels.

Analizujemy:

* complexity,
* allocation,
* locality,
* cancellation cost.

**Nie zaczynamy od kodu. Najpierw design interview.**

---

## 56. Matching engine — implementation

Rozbudowujesz poprzedni system o:

```cpp
match()
add()
cancel()
```

oraz:

* buy/sell,
* crossing prices,
* FIFO within price level,
* partial fills.

**Cel**
Kod interview-quality, a nie production exchange.

---

## 57. Matching engine — robustness i performance

Rozszerzamy engine o:

* unique order IDs,
* cancel,
* modify,
* invalid input,
* replay,
* snapshots albo event log,
* tests.

Następnie pytam:

> Where are your allocations?

> What happens with 10 million orders?

> What's the hottest data?

> Which operation is O(log n)?

---

## 58. Low-latency trading system design

Projektujesz:

```text
Exchange
   ↓
Market Data
   ↓
Parser
   ↓
Order Book
   ↓
Strategy
   ↓
Risk
   ↓
Order Gateway
   ↓
Exchange
```

Analizujemy każdą warstwę pod:

* latency,
* throughput,
* concurrency,
* allocations,
* cache misses,
* network I/O,
* failure handling.

To będzie **systems interview**, a nie klasyczny distributed-system design.

---

## 59. Practical codebase interview

Dostajesz istniejący mini-project.

Najpierw czytasz kod.

Potem dostajesz nowe requirements.

Musisz:

1. zrozumieć architecture,
2. zadawać pytania,
3. zaproponować zmianę,
4. wskazać trade-offs,
5. zaimplementować ją,
6. przejść testy,
7. wyjaśnić complexity/performance.

To ma symulować rodzaj zadania dużo bliższy realnej pracy niż LeetCode.

---

# 60. FINAL IMC / HFT SWE MOCK

Bez nowej teorii.

### Etap A — fundamentals

Losowo:

* C++,
* templates,
* virtual dispatch,
* lifetime,
* STL,
* OS,
* virtual memory,
* CPU/cache,
* concurrency,
* networking.

I robię follow-upy.

### Etap B — coding

Problem DSA na czas.

Musisz:

* wyjaśnić brute force,
* znaleźć lepszy algorytm,
* policzyć complexity,
* napisać poprawny C++,
* znaleźć edge cases.

### Etap C — practical coding

Nowy requirement do matching engine albo podobnego mini-systemu.

### Etap D — project deep dive

Pytania typu:

* najtrudniejszy problem,
* decyzja architektoniczna,
* znaleziony bottleneck,
* jak mierzyłeś performance,
* co zrobiłbyś inaczej.

### Etap E — behavioral / IMC

* Why IMC?
* Why trading?
* Why C++?
* strongest project,
* disagreement,
* failure,
* receiving feedback,
* working under uncertainty.

Na koniec dostajesz ocenę osobno:

```text
C++                 /10
DSA                  /10
OS                   /10
CPU/performance      /10
Concurrency          /10
Networking           /10
Practical coding     /10
Communication        /10
```

---

# Główne źródła na całe 60 tematów

Nie musisz czytać żadnej z tych książek od początku do końca. Będziemy wyciągać **konkretne rozdziały pod konkretny dzień**.

**C++**

* Bjarne Stroustrup — *A Tour of C++, 3rd Edition*
* Scott Meyers — *Effective Modern C++*
* cppreference
* C++ Core Guidelines
* Compiler Explorer

**Systems / performance**

* Randal Bryant, David O'Hallaron — *Computer Systems: A Programmer's Perspective, 3rd Edition*
* Remzi & Andrea Arpaci-Dusseau — *Operating Systems: Three Easy Pieces*
* Michael Kerrisk — *The Linux Programming Interface* jako materiał rozszerzający

OSTEP jest szczególnie wygodne, ponieważ wszystkie kluczowe dla nas części od Processes przez VM aż po Concurrency są dostępne rozdziałami online. ([UW Computer Sciences][3])

**Concurrency**

* Anthony Williams — *C++ Concurrency in Action, 2nd Edition*
* OSTEP Ch. 26–33
* cppreference atomics/memory model

**Algorithms**

* Antti Laaksonen — *Competitive Programmer's Handbook*
* Cormen et al. — *Introduction to Algorithms (CLRS)*
* konkretne LeetCode/Codeforces problems będę dobierał do tematów

**Networking**

* Brian „Beej” Hall — *Beej's Guide to Network Programming*
* *Computer Systems: A Programmer's Perspective*, Ch. 11
* Stevens — *TCP/IP Illustrated* tylko tam, gdzie będziemy chcieli wejść głębiej.

---

## Jak używać tego w osobnym czacie

Najlepiej **nie wklejaj całych 60 punktów**. Wklej tylko np. temat #1 i napisz:

> „To jest kolejny temat mojego planu przygotowań do IMC/HFT SWE. Przeprowadź mnie przez niego dokładnie według opisu. Najpierw ustal teorię i materiały, potem omówmy mechanizmy, następnie zrób mi interview z follow-upami, a dopiero później zadanie praktyczne. Nie podawaj rozwiązania zadania przed moją próbą.”

I jedziemy.

**Nie przejmowałbym się przy tym sztywnym „60 dni”.** Jeden temat dziennie jest dobrym targetem, ale np. memory ordering albo matching engine mogą uczciwie zająć dwa dni. Z kolei coś, co już bardzo dobrze znasz, możemy przejść szybciej. Przy Twoim projekcie równolegle ważniejsze jest, żeby te ~60 tematów przerobić **głęboko w ciągu kilku miesięcy**, niż sztucznie odhaczyć dokładnie jeden numer każdego dnia.

Po takim przejściu zakres będzie już bardzo poważny: nie tylko pod obecny **IMC SWE Intern Amsterdam**, ale również jako fundament późniejszego przygotowania do Optiver/Citadel/HRT i ogólnie C++/low-latency.

[1]: https://www.imc.com/eu/careers/recruitment-process "Recruitment Process EU | IMC Trading"
[2]: https://en.cppreference.com/cpp/language "C++ language - cppreference.com"
[3]: https://pages.cs.wisc.edu/~remzi/OSTEP/ "Operating Systems: Three Easy Pieces"
[4]: https://beej.us/guide/bgnet/ "Beej's Guide to Network Programming"
