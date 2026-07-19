#include <cstdint>
#include <iostream>

// 3.8. Enums and enum classes

// Unscoped enum.
// E is a distinct enumeration type, not exactly an int wrapper,
// but its values can be implicitly converted to an integral type.
enum E {
    WHITE, // 0
    GRAY,  // 1
    BLACK  // 2
};

// Scoped enum.
// The underlying type is explicitly fixed to std::int8_t.
enum class EC : std::int8_t {
    WHITE,    // 0
    GRAY = 3, // 3
    BLACK     // 4
};

// Different enum classes remain completely separate types.
enum class Status {
    SUCCESS,
    FAILURE
};

enum class Permission : unsigned {
    READ    = 1u << 0, // 0001
    WRITE   = 1u << 1, // 0010
    EXECUTE = 1u << 2  // 0100
};

void print_color(EC color) {
    switch (color) {
        case EC::WHITE:
            std::cout << "white\n";
            break;
        case EC::GRAY:
            std::cout << "gray\n";
            break;
        case EC::BLACK:
            std::cout << "black\n";
            break;
    }
}

int main() {
    E e = WHITE;

    // Unscoped enums implicitly convert to integers.
    std::cout << e << '\n'; // 0

    int number = e; // OK: implicit conversion E -> int

    // Enumerator names are introduced directly into the surrounding scope.
    // We write WHITE instead of E::WHITE.
    E another = BLACK;

    EC ec = EC::WHITE;

    // enum class is safer and stronger because:
    //
    // 1. Enumerator names are scoped:
    //      EC::WHITE
    //    instead of:
    //      WHITE
    //
    // 2. It does not implicitly convert to int.
    //
    // 3. Different enum classes cannot be accidentally mixed.
    //
    // 4. Its underlying integral type can be selected explicitly.

    // int value = ec; // CTE: no implicit conversion EC -> int

    std::cout << static_cast<int>(ec) << '\n'; // 0

    // Casting directly to std::int8_t may print it as a character,
    // because int8_t is often an alias for signed char.
    std::cout << static_cast<int>(
        static_cast<std::int8_t>(ec)
    ) << '\n';

    EC gray = EC::GRAY;
    std::cout << static_cast<int>(gray) << '\n'; // 3

    EC black = EC::BLACK;
    std::cout << static_cast<int>(black) << '\n'; // 4

    // enum class values cannot be compared with integers directly.
    // if (ec == 0) {} // CTE

    if (ec == EC::WHITE) {
        std::cout << "The color is white\n";
    }

    // Different enum classes cannot be mixed.
    Status status = Status::SUCCESS;

    // if (ec == status) {} // CTE: EC and Status are different types

    print_color(EC::BLACK);

    // Explicit conversion from integer to enum is possible,
    // but the integer may not correspond to any declared enumerator.
    EC unknown = static_cast<EC>(100);

    std::cout << static_cast<int>(unknown) << '\n'; // usually 100

    // This is not automatically invalid.
    // Enum variables may contain values that are not listed as enumerators,
    // provided the value fits the enum's underlying representation.

    // Enum classes are also commonly used for bit flags.
    unsigned permissions =
        static_cast<unsigned>(Permission::READ) |
        static_cast<unsigned>(Permission::WRITE);

    if (permissions & static_cast<unsigned>(Permission::WRITE)) {
        std::cout << "Write permission enabled\n";
    }
}