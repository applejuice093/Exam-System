// Unit tests for the pure helpers in exam_utils.h. No framework needed:
// build with `make unit` (or g++ -std=c++17 -I. tests/unit_tests.cpp).
#include <iostream>
#include <string>
#include <vector>

#include "../exam_utils.h"

static int failures = 0;
static int checks = 0;

#define CHECK(cond)                                                         \
    do                                                                      \
    {                                                                       \
        ++checks;                                                           \
        if (!(cond))                                                        \
        {                                                                   \
            ++failures;                                                     \
            std::cerr << __FILE__ << ":" << __LINE__ << ": FAILED " #cond "\n"; \
        }                                                                   \
    } while (0)

static void testTrim()
{
    CHECK(trim("  hello \t\n") == "hello");
    CHECK(trim("") == "");
    CHECK(trim("   ") == "");
    CHECK(trim("a b") == "a b");
}

static void testParseIntStrict()
{
    int v = 0;
    CHECK(parseIntStrict("42", v) && v == 42);
    CHECK(parseIntStrict(" -7 ", v) && v == -7);
    CHECK(!parseIntStrict("", v));
    CHECK(!parseIntStrict("12abc", v));
    CHECK(!parseIntStrict("abc", v));
    CHECK(!parseIntStrict("99999999999", v));
}

static void testParseIdList()
{
    CHECK((parseIdList("3, 1,2") == std::vector<int>{3, 1, 2}));
    CHECK((parseIdList("1,1,2,1") == std::vector<int>{1, 2}));
    CHECK((parseIdList("x,,5,7y") == std::vector<int>{5}));
    CHECK(parseIdList("").empty());
}

static void testMarkLines()
{
    MarkRecord r;
    CHECK(parseMarkLine("alice|Unit Test 1|4", r));
    CHECK(r.student == "alice" && r.exam == "Unit Test 1" && r.score == 4);

    CHECK(parseMarkLine(formatMarkLine({"bob", "Final Exam", 9}), r));
    CHECK(r.student == "bob" && r.exam == "Final Exam" && r.score == 9);

    // Legacy space-separated line from older builds.
    CHECK(parseMarkLine("carol Midterm 3", r));
    CHECK(r.student == "carol" && r.exam == "Midterm" && r.score == 3);

    CHECK(!parseMarkLine("", r));
    CHECK(!parseMarkLine("alice|exam", r));
    CHECK(!parseMarkLine("alice|exam|ten", r));
    CHECK(!parseMarkLine("|exam|1", r));
    CHECK(!parseMarkLine("dave Unit Test 2", r)); // ambiguous legacy line
}

static void testRecordFields()
{
    CHECK(isValidRecordField("Unit Test 1"));
    CHECK(!isValidRecordField(""));
    CHECK(!isValidRecordField("   "));
    CHECK(!isValidRecordField("a|b"));
    CHECK(!isValidRecordField("line\nbreak"));
}

static void testPasswords()
{
    CHECK(processPassword(processPassword("s3cret!")) == "s3cret!");
    CHECK(processPassword("admin123") != "admin123");
    CHECK(isStorablePassword("admin123"));
    CHECK(!isStorablePassword("it's"));   // ' ^ 7 == ' '
    CHECK(!isStorablePassword(""));
}

int main()
{
    testTrim();
    testParseIntStrict();
    testParseIdList();
    testMarkLines();
    testRecordFields();
    testPasswords();
    if (failures)
    {
        std::cerr << failures << " of " << checks << " checks failed\n";
        return 1;
    }
    std::cout << "unit tests passed (" << checks << " checks)\n";
    return 0;
}
