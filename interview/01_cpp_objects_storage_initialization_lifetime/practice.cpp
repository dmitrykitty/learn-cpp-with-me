#include <iostream>
#include <string>

namespace EX1 {
    /*
    1) How many std::string objects are actually created here?
    2) What exactly is s inside identity — object or reference?
    3) What is the storage duration of the temporary std::string{"hello"}?
    4) When does that temporary's lifetime begin?
    5) Binding it to const std::string& s — does that extend its lifetime?
    6) Then identity returns s, and the result is bound to another const std::string&, r. Does that extend the lifetime further?
    7) At the std::cout << r line, is r valid, dangling, or something else?
    8) Is there UB? If yes, identify the exact operation that triggers it, not merely where the bug originates.
    */
    const std::string& identity(const std::string& s) {
        return s;
    }

    int main() {
        const std::string& r = identity(std::string{"hello"});
        std::cout << r << '\n';
    }

    /*
    1) How many std::string objects are actually created here?
        so, i think one from std::string{"hello"}, then it taken by const lref, so no copy. And later the same string is returned an reference, so again no copy created. 
        my guess - 1 s and r are references; neither creates another string.

    2) What exactly is s inside identity — object or reference?
        reference, we pass it as reference to temp object 

    3) What is the storage duration of the temporary std::string{"hello"}?
        this one is tricky. Maybe automatic because it created in function param
        -----------------------------------------------------------------------------------------------------------------------------
        The important correction is that the temporary is not created inside identity and is not the function parameter.

        It is created by the argument expression in main:
        identity(std::string{"hello"})
                ^^^^^^^^^^^^^^^^^^^^

        In this situation, think of it as a temporary with automatic storage duration. But s merely refers to it. Static, thread, and automatic storage durations can apply to temporary objects.
        This distinction matters a lot:
        temporary string object
                ↑
                |
                s   // reference, not another string
                |
                r   // another reference to same string

    4) When does that temporary's lifetime begin?
        after calling constraction from cstring
        ---------------------------------------------------------------------------------------------------------------------------
        The lifetime begins once storage with the required properties exists and initialization of the std::string has completed

    5) Binding it to const std::string& s — does that extend its lifetime?
        I think extends 
        -------------------------------------------------------------------------------------------------------------------------------
        The temporary bound to s survives only until the end of the full-expression containing the function call. The C++ standard states this explicitly.
        But binding a temporary to a reference parameter does not extend it to some longer lifetime. It dies at the end of the full-expression containing the call.

    6) Then identity returns s, and the result is bound to another const std::string&, r. Does that extend the lifetime further?
        I think no. returned reference does not extend lifetime of the object

    7) At the std::cout << r line, is r valid, dangling, or something else?
        Probably dandling. So its UB

    8) Is there UB? If yes, identify the exact operation that triggers it, not merely where the bug originates.
        std::cout as mention above
        ----------------------------------------------------------------------------------------------------------------------------------
        The destruction itself is valid. Having a dangling reference is not by itself the UB here. The UB occurs when the next statement attempts to use r as a live std::string.
    */
}


namespace EX2 {
    /*
    1) How many relevant objects/subobjects exist inside getName()?
    2) What is the relationship between user and user.name from the object-lifetime perspective?
    3) What storage duration does user have?
    4) When does the lifetime of user.name begin?
    5) When does its lifetime end relative to the lifetime of user?
    6) Does returning const std::string& change anything?
    7) Does binding the returned reference to name extend anything?
    8) At exactly what point does name become dangling?
    9) Bonus: suppose User::~User() explicitly prints name. Is accessing name inside the body of User::~User() valid, given that we said an object's lifetime ends when destruction begins?
    */
    struct User {
        std::string name;
    };

    const std::string& getName() {
        User user{"Alice"};
        return user.name;
    }

    int main() {
        const std::string& name = getName();

        std::cout << name << '\n';
    }

    /*
    1) How many relevant objects/subobjects exist inside getName()?
    It's quit tricky. First string is during initialization of User. Because in getName we don't have named varialbe - optimization will not work
    and one extra copy will be created and we will create referenceto this copy 
    --------------------------------------------------------------------------------------------------------------------------------
    You wrote that an extra std::string copy is created. It isn't.

    User user{"Alice"};

    user.name is initialized directly from "Alice".

    Conceptually, the important objects are:

    User user
    └── std::string user.name

    So there is:

    one User object,
    one std::string member subobject.

    If we are extremely pedantic, "Alice" itself is also a string-literal object of type roughly const char[6] with static storage duration. But there is no extra temporary std::string copy involved that the returned reference refers to.

    2) What is the relationship between user and user.name from the object-lifetime perspective?
    I think it realted because before destruction of the user all its fields should be destroyed. 
    -------------------------------------------------------------------------------------------------------------
    Your intuition is partly correct, but destruction order is the opposite of what you later assumed.
    name is a subobject of user.

    During construction:
    storage for User
        ↓
    initialize name
        ↓
    User becomes fully initialized

    During destruction:
    User::~User() begins
        ↓
    User::~User() body executes
        ↓
    name is destroyed
        ↓
    storage can be reused/released

    3) What storage duration does user have?
    Automatic because it's local inside function

    4) When does the lifetime of user.name begin?
    Before lifetime of user. Firstly all fields are initialized and only then user 
    user.name is a member subobject. Its lifetime begins when initialization of that member completes, during initialization of the enclosing User object.

    5) When does its lifetime end relative to the lifetime of user?
    Before end of user lifetime
    -------------------------------------------------------------------------------------------------------------
    Here your answer is incorrect.
    You said:"Before end of user lifetime"

    The tricky part is that formally the lifetime of a class object ends when its destructor starts.

    So:

    User::~User() {
        // formal User lifetime has already ended
        // BUT name is still alive here
    }

    Only after the destructor body do the member subobjects get destroyed.
    So the sequence is roughly:

    User lifetime ends
            ↓
    User::~User() body
            ↓
    std::string name lifetime ends
            ↓
    name destructor executes

    This looks strange at first, but C++ has explicit rules that allow you to work with the object and its still-alive subobjects during destruction.

    6) Does returning const std::string& change anything?
    I don't think so. It simply returns a reference to the existing member.

    7) Does binding the returned reference to name extend anything?
    No

    8) At exactly what point does name become dangling?
    After return call? I'm not sure

    9) Bonus: suppose User::~User() explicitly prints name. Is accessing name inside the body of User::~User() valid, given that we said an object's lifetime ends when destruction begins?
    No, because all fields are destroyed beofre destroying the user 
    -----------------------------------------------------------------------------------------------------
    You said: "No, because all fields are destroyed before destroying the user"

    It's the opposite.
    Suppose:
        struct User {
            std::string name;


            ~User() {
                std::cout << name;
            }
        };

    This is perfectly valid.
    Member subobjects are destroyed after the destructor body finishes.
    */
}

namespace EX3 {
    struct User {
        std::string name;

        ~User() {
            std::cout << "Destroying User\n";
        }
    };

    int main() {
        const std::string& name = User{"Alice"}.name;

        std::cout << name << '\n';
    }

    /*
    1) How many User objects are created?
    It looks like only one temp user object created

    2) Is User{"Alice"} a temporary?
    Yes

    3) Normally, when would that temporary be destroyed?
    I think after assingment of the name, so basicly after line const std::string& name = User{"Alice"}.name;

    4) We bind name not to the entire User, but to the member subobject User{"Alice"}.name. Does lifetime extension happen?
    I guess this object user will be created on stack probably (not like before on heap) so we will expand lifetime of user and name
    -----------------------------------------------------------------------------------------------------------------------------------
    The stack has nothing to do with lifetime extension.
    Also, the User user{"Alice"} from the previous exercise was not on the heap. It had automatic storage duration and would typically also be implemented using stack storage.

    Never use this mental model:

    stack -> lifetime extension
    heap  -> no lifetime extension

    Those concepts are unrelated.
    The reason this works is the expression itself:

    const std::string& name = User{"Alice"}.name;
    C++ explicitly says that if a reference binds to a subobject of a temporary through member access ., the lifetime of the complete temporary object containing that subobject is extended.

    5) If yes, what exactly gets its lifetime extended: only the std::string, or the entire temporary User? 
    both, The lifetime of the complete temporary User is extended to the lifetime of name. Because user.name is its member subobject, that member remains alive as part of the temporary.

    6) When does "Destroying User" print: before or after std::cout << name?
    After because of ref name

    7) Is name dangling at the cout line? 
    No

    8) Is there UB?
    No
    */
}

namespace EX4 {
    namespace A {
        const std::string& makeA() {
            return std::string{"hello"};
        }

        int main() {
            const std::string& a = makeA();
            std::cout << a << '\n';
        }
    }
    namespace B {
        std::string makeB() {
            return std::string{"hello"};
        }

        int main() {
            const std::string& b = makeB();
            std::cout << b << '\n';
        }
    }

    /*
    I want you to compare them, rather than analyze them independently.

    1) In makeA(), where is the std::string{"hello"} created, and when would it normally die?
    Probably on heap and string will be destroyed after leaving the function

    2) Does returning it as const std::string& extend its lifetime?
    Nope

    3) When a binds to the returned reference, can the temporary's lifetime be extended again?
    Nope
    4) Is a valid at cout?
    No, it is UB

    ------------------------------------------------------------------------------------------
    Your answer:

    Probably on heap

    Don't think of the std::string object itself as “on the heap.” std::string{"hello"} is a temporary std::string object created while evaluating the return expression. Whether that string internally allocates a character buffer dynamically is a completely separate implementation detail; for "hello", small-string optimization may mean no dynamic allocation at all.

    So always separate:

    std::string object
            vs
    memory potentially owned internally by std::string

    The important lifetime sequence is:
    construct temporary std::string
            ↓
    return reference binds to it
            ↓
    temporary dies at end of return-expression full-expression
            ↓
    makeA() returns a dangling reference

    Returning a reference does not extend the temporary's lifetime, and binding a to that returned reference does not extend it again. So by cout, a is dangling.
    One modern-C++ footnote: under the C++20 semantics of our exercise, this is a dangling-reference bug. The current C++ draft has gone further and makes a return statement that directly binds a returned reference to a temporary expression ill-formed, so newer language rules catch this case at compile time.

    ---------------
    Then Version B:

    5) What is fundamentally different about returning std::string by value?
    Copy will be returned. How many copies will be done. Is RVO works here? I think now so one object of string is created and then copy of it returned
    6) Does makeB() return a reference to its local/temporary object, or does the caller receive a result object?
    Result new object - copy of object created inside makeB()

    7) When that result object binds directly to:
    const std::string& b

    does lifetime extension happen?
    Yes, because makeB() the same as User("Ailce").name return temp object and const reference to it expand lifetime

    8) Is b valid at cout?
    Yes

    9) Therefore, which version has UB: A, B, both, or neither?
    A
    The key question I would ask you in an interview afterward is:

    Why can b extend the lifetime, while a cannot, even though both declarations in main() look like const std::string& x = ...?
    Return type matter - in b we return copy so it's lifetime not connected with makeB function call. 
    


   -----------------------------------------------------------------------------------------------------
   Version B — this is where your main mistake is:
    std::string makeB() {
        return std::string{"hello"};
    }

    You said: one object is created and then copy of it returned
    For C++17 and later, no copy is required here — and this is stronger than optional RVO.

    The operand: std::string{"hello"}

    is a prvalue of exactly the function's return type. The function's result object is initialized directly from it. The standard specifically notes that a return needs a copy/move constructor when the operand is not a prvalue or has a different type; that's not our case.

    So the useful mental model is:
    std::string makeB() {
        return std::string{"hello"};
    }

    Approximately:
    construct makeB()'s result std::string directly
            ↓
    no intermediate string that must be copied
            ↓
    caller receives that result object

    Think one std::string object, not:
    local temporary -> copy -> returned object

    This is part of the C++17 prvalue model / commonly called guaranteed copy elision.

    Then:
    const std::string& b = makeB();

    makeB() has a non-reference return type, so the call expression produces a std::string prvalue/result object. Binding b directly to that result materializes the temporary, and the normal reference lifetime-extension rule applies. The standard explicitly allows a temporary bound this way to persist for the lifetime of the reference.

    So:

    makeB result object
        ↓
    b binds directly to it
        ↓
    lifetime extended to b
        ↓
    cout is valid
        ↓
    temporary destroyed when b's lifetime ends


    “The return type matters. makeA() returns a reference, so the call expression only designates an existing object; it does not create a new result std::string, and binding another reference cannot re-extend the original object's lifetime. makeB() returns by value, so the call produces a std::string result object. b binds directly to that temporary result object, so its lifetime is extended.”

    That's a very strong interview answer.
    And notice the beautiful distinction:

    const std::string& a = makeA(); // makeA() is an lvalue expression
    const std::string& b = makeB(); // makeB() is a prvalue expression

    because a function call returning an lvalue reference is an lvalue, while a call returning a non-reference object type is a prvalue.

    You don't need to memorize the value-category terminology yet — that's a later chapter — but the underlying distinction is already useful.
   */
}

