#include <gtest/gtest.h>

#include "String.h"

#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>

namespace {

void expectStringEquals(const String& actual, const char* expected) {
    ASSERT_NE(actual.c_str(), nullptr);

    EXPECT_EQ(actual.size(), std::strlen(expected));
    EXPECT_STREQ(actual.c_str(), expected);
    EXPECT_EQ(actual.c_str()[actual.size()], '\0');
}

}

// Compile-time properties

static_assert(std::is_constructible_v<String, char>);
static_assert(!std::is_convertible_v<char, String>);


// Constructors

TEST(StringConstructorTests, DefaultConstructorCreatesEmptyString) {
    String str;

    EXPECT_EQ(str.size(), 0u);
    EXPECT_TRUE(str.empty());
    EXPECT_NE(str.c_str(), nullptr);
    EXPECT_EQ(str.c_str()[0], '\0');
}

TEST(StringConstructorTests, CStringConstructorCreatesCorrectString) {
    const char* source = "abcd";
    String str(source);
    expectStringEquals(str, "abcd");
    EXPECT_NE(str.c_str(), source);
}

TEST(StringConstructorTests, CStringConstructorCreatesEmptyStringFromNullptr) {
    String str(nullptr);
    expectStringEquals(str, "");
    EXPECT_TRUE(str.empty());
}

TEST(StringConstructorTests, RepeatedCharacterConstructorCreatesCorrectString) {
    String str(5, 'x');
    expectStringEquals(str, "xxxxx");
}

TEST(StringConstructorTests, RepeatedCharacterConstructorHandlesZeroSize) {
    String str(0, 'x');
    expectStringEquals(str, "");
}

TEST(StringConstructorTests, CharacterConstructorCreatesOneCharacterString) {
    String str('q');

    expectStringEquals(str, "q");
}

TEST(StringConstructorTests, InitializerListConstructorCopiesCharacters) {
    String str{'a', 'b', 'c', 'd'};
    expectStringEquals(str, "abcd");
}

// Copy constructor and assignment

TEST(StringCopyTests, CopyConstructorPerformsDeepCopy) {
    String original("hello");
    String copy(original);

    expectStringEquals(original, "hello");
    expectStringEquals(copy, "hello");

    EXPECT_NE(original.c_str(), copy.c_str());

    copy[0] = 'H';

    expectStringEquals(original, "hello");
    expectStringEquals(copy, "Hello");
}

TEST(StringCopyTests, AssignmentOperatorPerformsDeepCopy) {
    String source("hello");
    String destination("old");

    String& result = (destination = source);

    EXPECT_EQ(&result, &destination);
    EXPECT_NE(destination.c_str(), source.c_str());

    expectStringEquals(source, "hello");
    expectStringEquals(destination, "hello");

    destination[0] = 'H';

    expectStringEquals(source, "hello");
    expectStringEquals(destination, "Hello");
}

TEST(StringCopyTests, SelfAssignmentPreservesValue) {
    String str("hello");

    str = str;

    expectStringEquals(str, "hello");
}

TEST(StringCopyTests, AssignmentFromTemporaryWorksCorrectly) {
    String str("old");

    str = String("new");

    expectStringEquals(str, "new");
}


// Element access

TEST(StringElementAccessTests, SubscriptOperatorReadsCharacters) {
    const String str("abcd");

    EXPECT_EQ(str[0], 'a');
    EXPECT_EQ(str[1], 'b');
    EXPECT_EQ(str[2], 'c');
    EXPECT_EQ(str[3], 'd');
}

TEST(StringElementAccessTests, NonConstSubscriptOperatorModifiesCharacters) {
    String str("abcd");
    str[1] = 'X';

    expectStringEquals(str, "aXcd");
}

TEST(StringElementAccessTests, FrontAndBackReturnCorrectCharacters) {
    const String str("abcd");

    EXPECT_EQ(str.front(), 'a');
    EXPECT_EQ(str.back(), 'd');
}

TEST(StringElementAccessTests, NonConstFrontAndBackCanModifyCharacters) {
    String str("abcd");

    str.front() = 'X';
    str.back() = 'Y';

    expectStringEquals(str, "XbcY");
}


// Modifiers

TEST(StringModifierTests, PushBackAddsCharacterToEmptyString) {
    String str;
    str.push_back('a');
    expectStringEquals(str, "a");
}

TEST(StringModifierTests, PushBackPreservesExistingCharacters) {
    String str("abc");
    str.push_back('d');
    str.push_back('e');

    expectStringEquals(str, "abcde");
}

TEST(StringModifierTests, PushBackHandlesMultipleReallocations) {
    String str;
    std::string expected;

    for (int i = 0; i < 200; ++i) {
        const char character = static_cast<char>('a' + i % 26);

        str.push_back(character);
        expected.push_back(character);
    }

    EXPECT_EQ(str.size(), expected.size());
    EXPECT_STREQ(str.c_str(), expected.c_str());
    EXPECT_EQ(str.c_str()[str.size()], '\0');
}

TEST(StringModifierTests, ClearCreatesEmptyReusableString) {
    String str("hello");
    str.clear();

    expectStringEquals(str, "");
    EXPECT_TRUE(str.empty());

    str.push_back('x');
    expectStringEquals(str, "x");
}

TEST(StringModifierTests, PopBackRemovesLastCharacter) {
    String str("abcd");

    str.pop_back();

    expectStringEquals(str, "abc");
    EXPECT_EQ(str.back(), 'c');
}

TEST(StringModifierTests, PopBackCanMakeStringEmpty) {
    String str("a");

    str.pop_back();

    expectStringEquals(str, "");
    EXPECT_TRUE(str.empty());
}

TEST(StringModifierTests, ReservePreservesExistingValue) {
    String str("hello");

    str.reserve(1000);

    expectStringEquals(str, "hello");

    str.push_back('!');

    expectStringEquals(str, "hello!");
}

TEST(StringModifierTests, ReserveWithSmallerValuePreservesExistingValue) {
    String str("hello");
    str.reserve(2);
    expectStringEquals(str, "hello");
}

TEST(StringModifierTests, ShrinkToFitPreservesValue) {
    String str("hello");

    str.reserve(1000);
    str.shrink_to_fit();
    expectStringEquals(str, "hello");
    str.push_back('!');
    expectStringEquals(str, "hello!");
}



// Concatenation

TEST(StringConcatenationTests, PlusEqualsAppendsAnotherString) {
    String left("hello");
    const String right(" world");

    String& result = (left += right);

    EXPECT_EQ(&result, &left);
    expectStringEquals(left, "hello world");
    expectStringEquals(right, " world");
}

TEST(StringConcatenationTests, PlusEqualsHandlesEmptyString) {
    String str("hello");
    str += String();
    expectStringEquals(str, "hello");
}

TEST(StringConcatenationTests, PlusEqualsCanAppendToEmptyString) {
    String str;
    str += String("hello");
    expectStringEquals(str, "hello");
}

TEST(StringConcatenationTests, PlusEqualsHandlesSelfAppend) {
    String str("abc");
    str += str;
    expectStringEquals(str, "abcabc");
}

TEST(StringConcatenationTests, PlusCreatesNewStringWithoutModifyingOperands) {
    const String left("hello");
    const String right(" world");

    String result = left + right;

    expectStringEquals(result, "hello world");
    expectStringEquals(left, "hello");
    expectStringEquals(right, " world");
}

TEST(StringConcatenationTests, PlusHandlesEmptyOperands) {
    expectStringEquals(String() + String("abc"), "abc");
    expectStringEquals(String("abc") + String(), "abc");
}


// Comparison

TEST(StringComparisonTests, EqualStringsCompareEqual) {
    EXPECT_EQ(String("abc"), String("abc"));
}

TEST(StringComparisonTests, DifferentStringsCompareNotEqual) {
    EXPECT_NE(String("abc"), String("abd"));
}

TEST(StringComparisonTests, ComparisonIsLexicographical) {
    EXPECT_LT(String("abc"), String("abd"));
    EXPECT_GT(String("abd"), String("abc"));
}

TEST(StringComparisonTests, ShorterPrefixComparesAsSmaller) {
    EXPECT_LT(String("abc"), String("abcd"));
    EXPECT_GT(String("abcd"), String("abc"));
}

TEST(StringComparisonTests, EmptyStringComparesAsSmallerThanNonEmptyString) {
    EXPECT_LT(String(), String("a"));
}


// Find and rfind

TEST(StringFindTests, FindReturnsFirstOccurrence) {
    const String str("banana");
    EXPECT_EQ(str.find(String("ana")), 1);
}

TEST(StringFindTests, FindReturnsMinusOneWhenSubstringIsMissing) {
    const String str("banana");
    EXPECT_EQ(str.find(String("xyz")), -1);
}

TEST(StringFindTests, FindCanFindWholeString) {
    const String str("banana");
    EXPECT_EQ(str.find(String("banana")), 0);
}

TEST(StringFindTests, RfindReturnsLastOccurrence) {
    const String str("banana");
    EXPECT_EQ(str.rfind(String("ana")), 3);
}

TEST(StringFindTests, RfindReturnsMinusOneWhenSubstringIsMissing) {
    const String str("banana");

    EXPECT_EQ(str.rfind(String("xyz")), -1);
}


// Substring

TEST(StringSubstringTests, SubstrReturnsRequestedPart) {
    const String str("abcdef");
    const String result = str.substr(2, 3);

    expectStringEquals(result, "cde");
    expectStringEquals(str, "abcdef");
}

TEST(StringSubstringTests, SubstrWithZeroLengthReturnsEmptyString) {
    const String str("abcdef");
    const String result = str.substr(2, 0);

    expectStringEquals(result, "");
}

TEST(StringSubstringTests, SubstrCanReturnWholeString) {
    const String str("abcdef");
    const String result = str.substr(0, str.size());

    expectStringEquals(result, "abcdef");
    EXPECT_NE(result.c_str(), str.c_str());
}



// Stream operators

TEST(StringStreamTests, OutputOperatorWritesStringContent) {
    const String str("hello");
    std::ostringstream output;
    output << str;
    EXPECT_EQ(output.str(), "hello");
}

TEST(StringStreamTests, InputOperatorReadsString) {
    std::istringstream input("hello");
    String str("old");
    input >> str;
    expectStringEquals(str, "hello");
}