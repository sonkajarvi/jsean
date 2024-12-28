#include <test.h>

#include "../../src/string.h"

#define LARGE 100000

test_case(string_free)
{
    string s = {0};

    string_append_chars(&s, "hello, world");
    test_assert(s.data != NULL);
    test_assert(s.length == 12);

    string_free(&s);
    test_assert(s.data == NULL);
    test_assert(s.length == 0);

    test_success();
}

test_case(string_append_char)
{
    string s = {0};

    for (int i = 0; i < LARGE; i++)
        string_append_char(&s, 'a');

    test_assert(s.data != NULL);
    test_assert(s.length == LARGE);
    string_free(&s);

    test_success();
}

test_case(string_append_chars_n)
{
    string s = {0};

    for (int i = 0; i < LARGE; i++)
        string_append_chars_n(&s, "abc", 3);

    test_assert(s.data != NULL);
    test_assert(s.length == 3 * LARGE);
    string_free(&s);

    test_success();
}

test_case(string_reserve)
{
    string s = {0};

    string_reserve(&s, 1000);

    test_assert(s.data != NULL);
    test_assert(s.capacity == 1000);
    string_free(&s);

    test_success();
}
