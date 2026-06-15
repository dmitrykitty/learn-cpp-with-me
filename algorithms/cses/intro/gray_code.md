In CSES **Gray Code**, you need to print all binary strings of length `n` such that **two neighboring strings differ in exactly one bit**.

Example for `n = 2`:

```text id="bgn6m6"
00
01
11
10
```

Check neighbors:

```text id="lywla3"
00 -> 01   differs in last bit
01 -> 11   differs in first bit
11 -> 10   differs in last bit
```

## Main idea

Normal binary counting is not good:

```text id="iql4p4"
00
01
10
11
```

Problem:

```text id="0xjthh"
01 -> 10
```

Two bits change.

Gray code fixes this.

## The easiest formula

For every number `i` from `0` to `2^n - 1`, Gray code is:

```cpp id="filwkc"
gray = i ^ (i >> 1);
```

Here:

```text id="p4ka7j"
^  = XOR
>> = right shift
```

Then print `gray` as an `n`-bit binary number.

## Example: n = 3

Numbers:

```text id="lqfzhq"
i = 0  -> 000
i = 1  -> 001
i = 2  -> 010
i = 3  -> 011
i = 4  -> 100
i = 5  -> 101
i = 6  -> 110
i = 7  -> 111
```

Now compute:

```text id="xti5ig"
gray = i ^ (i >> 1)
```

| i | binary i | i >> 1 | gray |
| - | -------- | ------ | ---- |
| 0 | 000      | 000    | 000  |
| 1 | 001      | 000    | 001  |
| 2 | 010      | 001    | 011  |
| 3 | 011      | 001    | 010  |
| 4 | 100      | 010    | 110  |
| 5 | 101      | 010    | 111  |
| 6 | 110      | 011    | 101  |
| 7 | 111      | 011    | 100  |

So output:

```text id="p8sumj"
000
001
011
010
110
111
101
100
```

Each neighboring pair differs by exactly one bit.

## Why does the formula work?

The formula is:

```cpp id="vfukkt"
gray = i ^ (i >> 1);
```

Each Gray code bit is created from two neighboring binary bits.

Suppose:

```text id="0qj5xp"
i = b2 b1 b0
```

Then:

```text id="w2hfhc"
i >> 1 = 0 b2 b1
```

XOR them:

```text id="28natf"
gray = b2       b1       b0
      XOR
       0        b2       b1

gray = b2   b2^b1   b1^b0
```

So every Gray bit says:

```text id="azufp3"
Did this binary bit change compared to the previous higher bit?
```

That encoding makes consecutive numbers differ by only one position in Gray representation.

## Intuitive construction: mirror method

There is another way to understand it.

For `n = 1`:

```text id="o2hdby"
0
1
```

For `n = 2`, take previous list:

```text id="8fyhjv"
0
1
```

Copy it normally and add `0` in front:

```text id="qo8dxg"
00
01
```

Then mirror it and add `1` in front:

```text id="touath"
11
10
```

Together:

```text id="elj3ca"
00
01
11
10
```

For `n = 3`, take the `n = 2` list:

```text id="s2nyah"
00
01
11
10
```

Add `0` in front:

```text id="dj8xo6"
000
001
011
010
```

Mirror old list and add `1` in front:

```text id="vw8wrc"
110
111
101
100
```

Together:

```text id="ar6smy"
000
001
011
010
110
111
101
100
```

Same result as formula.

## C++ solution using formula

```cpp id="r43gzi"
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    int total = 1 << n;

    for (int i = 0; i < total; ++i) {
        int gray = i ^ (i >> 1);

        for (int bit = n - 1; bit >= 0; --bit) {
            cout << ((gray >> bit) & 1);
        }

        cout << '\n';
    }
}
```

## Why this is efficient

You must print `2^n` strings, each of length `n`.

So minimum possible complexity is:

```text id="36u6mh"
O(n * 2^n)
```

Your algorithm cannot be asymptotically faster because the output itself has that size.

## Interview-style summary

Gray code is a binary ordering where consecutive values differ in one bit. The standard way to generate the `i`-th Gray code is:

```cpp id="3lghqo"
i ^ (i >> 1)
```

For CSES, iterate from `0` to `2^n - 1`, compute this value, and print it with exactly `n` bits. The total complexity is `O(n * 2^n)`, which is optimal because we print `2^n` strings of length `n`.
