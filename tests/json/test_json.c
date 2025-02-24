#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/JSON.h>
#include <test.h>

test_case(JSON_type)
{
    JSON a;

    JSON_set_null(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    JSON_set_boolean(&a, true);
    test_assert(JSON_type(&a) == JSON_TYPE_BOOLEAN);

    json_init_object(&a, 0);
    test_assert(JSON_type(&a) == JSON_TYPE_OBJECT);
    JSON_free(&a);

    JSON_set_array(&a, 0);
    test_assert(JSON_type(&a) == JSON_TYPE_ARRAY);
    JSON_free(&a);

    JSON_set_number(&a, 42);
    test_assert(JSON_type(&a) == JSON_TYPE_NUMBER);

    JSON_set_string(&a, "Hello, World!");
    test_assert(JSON_type(&a) == JSON_TYPE_STRING);
    JSON_free(&a);

    test_success();
}

test_case(JSON_move)
{
    JSON a, b;

    JSON_set_number(&a, 2.0);
    JSON_set_boolean(&b, true);

    test_assert(JSON_type(&a) == JSON_TYPE_NUMBER);
    test_assert(JSON_number(&a) == 2.0);
    test_assert(JSON_type(&b) == JSON_TYPE_BOOLEAN);
    test_assert(JSON_boolean(&a) == true);

    JSON_move(&b, &a);

    test_assert(JSON_type(&b) == JSON_TYPE_NUMBER);
    test_assert(JSON_number(&b) == 2.0);
    // test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    test_success();
}

test_case(JSON_free)
{
    JSON a;

    JSON_set_null(&a);
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    JSON_set_boolean(&a, true);
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    json_init_object(&a, 0);
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    JSON_set_array(&a, 0);
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    JSON_set_number(&a, 42);
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    JSON_set_string(&a, "Hello, World!");
    JSON_free(&a);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    test_success();
}

test_case(JSON_set_null)
{
    JSON a;

    test_assert(JSON_set_null(NULL) == EINVAL);

    test_assert(JSON_set_null(&a) == 0);
    test_assert(JSON_type(&a) == JSON_TYPE_NULL);

    test_success();
}

test_case(JSON_set_and_get_boolean)
{
    JSON a;

    test_assert(JSON_set_boolean(NULL, true) == EINVAL);

    JSON_set_boolean(&a, true);
    test_assert(JSON_type(&a) == JSON_TYPE_BOOLEAN);
    test_assert(JSON_boolean(&a) == true);

    JSON_set_boolean(&a, false);
    test_assert(JSON_type(&a) == JSON_TYPE_BOOLEAN);
    test_assert(JSON_boolean(&a) == false);

    test_success();
}

test_case(JSON_set_and_get_double)
{
    JSON a;

    test_assert(JSON_set_number(NULL, 2.0) == EINVAL);

    JSON_set_number(&a, 2.0);
    test_assert(JSON_type(&a) == JSON_TYPE_NUMBER);
    test_assert(JSON_number(&a) == 2.0);

    test_success();
}

test_case(JSON_set_and_get_string)
{
    JSON a;

    char *s = malloc(13);
    strcpy(s, "hello, world");

    JSON_set_string(&a, s);
    test_assert(JSON_type(&a) == JSON_TYPE_STRING);
    test_assert(strcmp(JSON_string(&a), s) == 0);
    test_assert(JSON_string(&a) != s);
    JSON_free(&a);

    JSON_move_string(&a, s);
    test_assert(JSON_type(&a) == JSON_TYPE_STRING);
    test_assert(strcmp(JSON_string(&a), s) == 0);
    test_assert(JSON_string(&a) == s);
    JSON_free(&a);

    test_success();
}
