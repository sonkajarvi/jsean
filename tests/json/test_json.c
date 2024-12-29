#include <errno.h>
#include <limits.h>
#include <string.h>

#include <jsean/json.h>
#include <test.h>

test_case(json_move)
{
    struct json json = {0}, tmp = {0};

    json_init_signed(&json, 1);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_signed(&json) == 1);

    json_move(&tmp, &json);
    test_assert(json_type(&tmp) == JSON_TYPE_NUMBER);
    test_assert(json_signed(&tmp) == 1);

    test_success();
}

test_case(json_type)
{
    struct json json = {0};

    json_init_null(&json);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    json_init_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);

    json_init_object(&json);
    test_assert(json_type(&json) == JSON_TYPE_OBJECT);

    json_init_array(&json);
    test_assert(json_type(&json) == JSON_TYPE_ARRAY);

    json_init_signed(&json, 42);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);

    json_init_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    json_free(&json);

    test_success();
}

test_case(json_set_null)
{
    struct json json = {0};

    test_assert(json_init_null(&json) == 0);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    test_assert(json_init_null(NULL) == EFAULT);

    test_success();
}

test_case(json_set_and_get_boolean)
{
    struct json json = {0};

    json_init_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);
    test_assert(json_boolean(&json) == true);

    json_init_boolean(&json, false);
    test_assert(json_boolean(&json) == false);

    test_success();
}

test_case(json_set_and_get_signed)
{
    struct json json = {0};

    json_init_signed(&json, -123);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_signed(&json) == -123);

    // From unsigned to signed
    json_init_unsigned(&json, -1);
    test_assert(json_signed(&json) == INT64_MAX);

    // From double to signed
    json_init_double(&json, -1.5);
    test_assert(json_signed(&json) == -1);

    test_success();
}

test_case(json_set_and_get_unsigned)
{
    struct json json = {0};

    json_init_unsigned(&json, 123);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_unsigned(&json) == 123);

    // From signed to unsigned
    json_init_signed(&json, -1);
    test_assert(json_unsigned(&json) == 0);

    // From double to unsigned
    json_init_double(&json, 1.5);
    test_assert(json_unsigned(&json) == 1);

    test_success();
}

test_case(json_set_and_get_double)
{
    struct json json = {0};

    json_init_double(&json, 1.5);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_double(&json) == 1.5);

    // From signed to double
    json_init_signed(&json, -1);
    test_assert(json_double(&json) == -1.0);

    // From unsigned to double
    json_init_unsigned(&json, 1);
    test_assert(json_double(&json) == 1.0);

    test_success();
}

test_case(json_set_and_get_string)
{
    struct json json = {0};

    json_init_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    test_assert(strcmp(json_string(&json), "Hello, World!") == 0);
    json_free(&json);

    test_success();
}
