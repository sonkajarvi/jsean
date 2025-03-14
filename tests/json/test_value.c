#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/jsean.h>
#include <test/test.h>

test_case(jsean_type)
{
    jsean_t a;

    jsean_set_null(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_boolean(&a, true);
    test_assert(jsean_type(&a) == JSEAN_TYPE_BOOLEAN);

    jsean_set_object(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_OBJECT);
    jsean_free(&a);

    jsean_set_array(&a, 0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_ARRAY);
    jsean_free(&a);

    jsean_set_number(&a, 42);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NUMBER);

    jsean_set_string(&a, "hello, world");
    test_assert(jsean_type(&a) == JSEAN_TYPE_STRING);
    jsean_free(&a);

    test_success();
}

test_case(jsean_move)
{
    jsean_t a, b;

    jsean_set_number(&a, 2.0);
    jsean_set_boolean(&b, true);

    test_assert(jsean_type(&a) == JSEAN_TYPE_NUMBER);
    test_assert(jsean_get_number(&a) == 2.0);
    test_assert(jsean_type(&b) == JSEAN_TYPE_BOOLEAN);
    test_assert(jsean_get_boolean(&a) == true);

    jsean_move(&b, &a);

    test_assert(jsean_type(&b) == JSEAN_TYPE_NUMBER);
    test_assert(jsean_get_number(&b) == 2.0);
    // test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    test_success();
}

test_case(jsean_free)
{
    jsean_t a;

    jsean_set_null(&a);
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_boolean(&a, true);
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_object(&a);
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_array(&a, 0);
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_number(&a, 42);
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_string(&a, "hello, world");
    jsean_free(&a);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    test_success();
}

test_case(jsean_set_null)
{
    jsean_t a;

    test_assert(jsean_set_null(NULL) == EINVAL);

    test_assert(jsean_set_null(&a) == 0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NULL);

    test_success();
}

test_case(jsean_set_and_get_boolean)
{
    jsean_t a;

    test_assert(jsean_set_boolean(NULL, true) == EINVAL);

    jsean_set_boolean(&a, true);
    test_assert(jsean_type(&a) == JSEAN_TYPE_BOOLEAN);
    test_assert(jsean_get_boolean(&a) == true);

    jsean_set_boolean(&a, false);
    test_assert(jsean_type(&a) == JSEAN_TYPE_BOOLEAN);
    test_assert(jsean_get_boolean(&a) == false);

    test_success();
}

test_case(jsean_set_and_get_double)
{
    jsean_t a;

    test_assert(jsean_set_number(NULL, 2.0) == EINVAL);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_NUMBER);
    test_assert(jsean_get_number(&a) == 2.0);

    test_success();
}

test_case(jsean_set_and_get_string)
{
    jsean_t a;

    char *s = malloc(13);
    strcpy(s, "hello, world");

    jsean_set_string(&a, s);
    test_assert(jsean_type(&a) == JSEAN_TYPE_STRING);
    test_assert(strcmp(jsean_get_string(&a), s) == 0);
    test_assert(jsean_get_string(&a) != s);
    jsean_free(&a);

    jsean_move_string(&a, s);
    test_assert(jsean_type(&a) == JSEAN_TYPE_STRING);
    test_assert(strcmp(jsean_get_string(&a), s) == 0);
    test_assert(jsean_get_string(&a) == s);
    jsean_free(&a);

    test_success();
}
