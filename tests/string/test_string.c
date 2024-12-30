#include <stdlib.h>
#include <string.h>

#include <test.h>

#include <jsean/string.h>

#define LARGE 100000

test_case(string_free)
{
    struct string s = {0};

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
    struct string s = {0};

    for (int i = 0; i < LARGE; i++)
        string_append_char(&s, 'a');

    test_assert(s.data != NULL);
    test_assert(s.length == LARGE);
    string_free(&s);

    test_success();
}

test_case(string_append_chars_n)
{
    struct string s = {0};

    for (int i = 0; i < LARGE; i++)
        string_append_chars_n(&s, "abc", 3);

    test_assert(s.data != NULL);
    test_assert(s.length == 3 * LARGE);
    string_free(&s);

    test_success();
}

test_case(string_reserve)
{
    struct string s = {0};

    string_reserve(&s, 1000);

    test_assert(s.data != NULL);
    test_assert(s.capacity == 1000);
    string_free(&s);

    test_success();
}

test_case(string_ref)
{
    struct string s = {0};

    test_assert(string_ref(&s) == NULL);
    test_assert(string_ref(NULL) == NULL);

    string_append_chars(&s, "hello, world");
    test_assert(strcmp(string_ref(&s), "hello, world") == 0);
    string_free(&s);

    test_success();
}

test_case(string_release)
{
    struct string s = {0};

    test_assert(string_release(&s) == NULL);
    test_assert(string_release(NULL) == NULL);

    string_append_chars(&s, "hello, world");
    char *released = string_release(&s);
    test_assert(s.data == NULL);
    test_assert(s.length == 0);
    test_assert(s.capacity == 0);
    test_assert(strcmp(released, "hello, world") == 0);
    free(released);

    test_success();
}

test_case(string_shrink)
{
    struct string s = {0};

    string_append_chars(&s, "hello, world");
    string_reserve(&s, 1000);

    test_assert(s.capacity == 1000);
    string_shrink(&s);
    test_assert(s.capacity == 13);
    string_free(&s);

    test_success();
}