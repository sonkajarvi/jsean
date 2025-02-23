#include <errno.h>
#include <limits.h>
#include <string.h>

#include <jsean/JSON.h>
#include <test.h>

test_case(json_move)
{
    JSON json = {0}, tmp = {0};

    json_set_number(&json, 1);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_number(&json) == 1);

    json_move(&tmp, &json);
    test_assert(json_type(&tmp) == JSON_TYPE_NUMBER);
    test_assert(json_number(&tmp) == 1);

    test_success();
}

test_case(json_type)
{
    JSON json = {0};

    json_init_null(&json);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    json_init_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);

    json_init_object(&json, 0);
    test_assert(json_type(&json) == JSON_TYPE_OBJECT);

    json_init_array(&json, 0);
    test_assert(json_type(&json) == JSON_TYPE_ARRAY);

    json_set_number(&json, 42);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);

    json_set_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    json_free(&json);

    test_success();
}

test_case(json_set_null)
{
    JSON json = {0};

    test_assert(json_init_null(&json) == 0);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    test_assert(json_init_null(NULL) == EFAULT);

    test_success();
}

test_case(json_set_and_get_boolean)
{
    JSON json = {0};

    json_init_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);
    test_assert(json_boolean(&json) == true);

    json_init_boolean(&json, false);
    test_assert(json_boolean(&json) == false);

    test_success();
}

test_case(json_set_and_get_double)
{
    JSON json = {0};

    json_set_number(&json, 1.5);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_number(&json) == 1.5);

    // From signed to double
    json_set_number(&json, -1);
    test_assert(json_number(&json) == -1.0);

    // From unsigned to double
    json_set_number(&json, 1);
    test_assert(json_number(&json) == 1.0);

    test_success();
}

test_case(json_set_and_get_string)
{
    JSON json = {0};

    json_set_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    test_assert(strcmp(json_string(&json), "Hello, World!") == 0);
    json_free(&json);

    test_success();
}
