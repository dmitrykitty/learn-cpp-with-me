# `explicit`, conversion operators and user-defined literals

## 1. `explicit` constructor

```cpp
struct Latitude {
    double value;

    explicit Latitude(double val) : value(val) {}
};
```

`explicit` blocks implicit conversion from `double` to `Latitude`.

Without `explicit`:

```cpp
Latitude lat = 12.0; // OK if constructor is not explicit
```

With `explicit`:

```cpp
Latitude lat = 12.0;        // CTE
Latitude lat(12.0);         // OK
Latitude lat{12.0};         // OK
auto lat = Latitude(12.0);  // OK
```

Main rule:

```text
explicit blocks implicit conversions,
but still allows direct construction.
```

Good style: one-argument constructors should usually be `explicit`, especially when the type has semantic meaning.

Example:

```cpp
struct Latitude { ... };
struct Longitude { ... };
```

We do not want to accidentally mix raw numbers with strongly typed coordinates.

---

## 2. Why `Point p(12, 15)` is dangerous

```cpp
struct Point {
private:
    Latitude lat;
    Longitude lng;

public:
    Point(Latitude lat, Longitude lng) : lat(lat), lng(lng) {}
};
```

If `Latitude(double)` and `Longitude(double)` are not explicit, then this works:

```cpp
Point p(12, 15);
```

because compiler silently does:

```cpp
Point p(Latitude(12), Longitude(15));
```

This is convenient, but weakens type safety.

With explicit constructors:

```cpp
Point p(12, 15); // CTE
```

Now user must write intent clearly:

```cpp
Point p(Latitude(12), Longitude(15));
```

or:

```cpp
Point p{Latitude{12}, Longitude{15}};
```

This is exactly why `explicit` is useful for domain types.

---

## 3. User-defined literals

```cpp
Latitude operator""_LTD(long double x) {
    return Latitude(static_cast<double>(x));
}

Longitude operator""_LNG(long double x) {
    return Longitude(static_cast<double>(x));
}
```

Now we can write:

```cpp
Point pt(15.4_LTD, 12.2_LNG);
```

This is readable and type-safe.

Important notes:

```text
User-defined literal operators must be declared at namespace scope.
Suffixes should start with underscore.
The literal 15.4_LTD calls operator""_LTD(15.4L).
```

This works even though constructor is `explicit`, because inside literal operator we explicitly construct the object:

```cpp
return Latitude(static_cast<double>(x));
```

So user code stays clean, but implicit conversions are still blocked.

---

## 4. Conversion operator

```cpp
struct Longitude {
    double value;

    explicit Longitude(double val) : value(val) {}

    explicit operator double() const {
        return value;
    }
};
```

This defines conversion from `Longitude` to `double`.

Return type is written before `operator` indirectly:

```cpp
operator double()
```

means:

```text
conversion operator to double
```

Because it is `explicit`, this does not work:

```cpp
Longitude lng(12.5);

double x = lng; // CTE
```

But this works:

```cpp
double x = static_cast<double>(lng);
```

or:

```cpp
double x = double(lng);
```

So in your `Point`:

```cpp
double get_lng() const {
    return static_cast<double>(lng);
}
```

is correct.

---

## 5. Why conversion operators should often be explicit

Implicit conversion operators can create surprising code.

If this were non-explicit:

```cpp
operator double() const {
    return value;
}
```

then `Longitude` could accidentally behave like a number:

```cpp
Longitude lng(10.0);

double x = lng;      // OK
auto y = lng + 5.0;  // OK
if (lng) {}          // maybe OK through double -> bool
```

This weakens the strong type.

For semantic wrappers like:

```text
Latitude
Longitude
Meters
Seconds
UserId
Price
```

implicit conversion back to primitive type often destroys type safety.

So better:

```cpp
explicit operator double() const;
```

and require:

```cpp
static_cast<double>(lng)
```

where conversion is intentional.

---

## 6. `operator double&()`

You wrote:

```cpp
explicit operator double&() {
    // ...
}
```

This is possible, but dangerous.

It would allow external code to mutate internal value:

```cpp
Longitude lng(12.0);

static_cast<double&>(lng) = 15.0;
```

This breaks encapsulation.

If you want read-only access, prefer:

```cpp
double value() const {
    return value_;
}
```

If you really need mutable access, make it explicit in API:

```cpp
double& raw_value() {
    return value;
}
```

Conversion to reference is usually not good style unless you are implementing low-level wrapper/proxy types.

---

## 7. Contextual conversion to `bool`

Special case: `explicit operator bool()`.

Example:

```cpp
struct File {
    bool opened = false;

    explicit operator bool() const {
        return opened;
    }
};
```

Even though the conversion is explicit, this works:

```cpp
File f;

if (f) {
    // file is opened
}
```

Why?

Because conditions use **contextual conversion to bool**.

C++ allows explicit `operator bool()` in boolean contexts, for example:

```cpp
if (obj) {}
while (obj) {}
!obj
obj && other
obj || other
```

But it does not allow general implicit conversion:

```cpp
bool b = f; // CTE if operator bool is explicit
int x = f;  // CTE
```

This is exactly what we want.

---

## 8. Why custom types should use `explicit operator bool()`

Bad old style:

```cpp
operator bool() const {
    return opened;
}
```

This allows too much:

```cpp
File f;

bool b = f; // OK
int x = f;  // could work through bool -> int
```

That is dangerous because your object becomes usable as an integer-like value in places you did not intend.

Good modern style:

```cpp
explicit operator bool() const {
    return opened;
}
```

Then:

```cpp
if (f) {}              // OK
bool b = static_cast<bool>(f); // OK
bool b2 = f;           // CTE
int x = f;             // CTE
```

So `explicit operator bool()` gives natural usage in conditions without allowing accidental numeric conversions.

---

## 9. Corrected compact code

```cpp
struct Latitude {
    double value;

    explicit Latitude(double val) : value(val) {}
};

Latitude operator""_LTD(long double x) {
    return Latitude(static_cast<double>(x));
}

struct Longitude {
    double value;

    explicit Longitude(double val) : value(val) {}

    explicit operator double() const {
        return value;
    }
};

Longitude operator""_LNG(long double x) {
    return Longitude(static_cast<double>(x));
}

struct Point {
private:
    Latitude lat;
    Longitude lng;

public:
    Point(Latitude lat, Longitude lng) : lat(lat), lng(lng) {}

    double get_lng() const {
        return static_cast<double>(lng);
    }
};

int main() {
    // Point p(12, 15); // CTE because constructors are explicit

    Point pt(Latitude(12), Longitude(13));
    Point pt2(15.4_LTD, 12.2_LNG);
}
```

---

## 10. Short summary

`explicit` on a constructor blocks implicit conversion into your type.

Use it for one-argument constructors unless implicit conversion is really intended.

User-defined literals can make strong types convenient without losing type safety.

Conversion operators convert your type back to another type.

`explicit operator double()` requires `static_cast<double>(obj)`.

`explicit operator bool()` is special: it works in `if`, `while`, `!`, `&&`, `||`, but blocks accidental conversion to `int` or general `bool` assignment.

For strong domain types like `Latitude` and `Longitude`, prefer explicit construction and explicit conversion.
