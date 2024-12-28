#include <errno.h>
#include <limits.h>

#include <jsean/json.h>
#include <test.h>

test_case(json_get_type)
{
    struct json json = {0};

    json_set_null(&json);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    json_set_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);

    json_set_object(&json);
    test_assert(json_type(&json) == JSON_TYPE_OBJECT);

    json_set_array(&json);
    test_assert(json_type(&json) == JSON_TYPE_ARRAY);

    json_set_signed(&json, 42);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);

    json_set_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    json_free(&json);

    test_success();
}

test_case(json_set_null)
{
    struct json json = {0};

    test_assert(json_set_null(&json) == 0);
    test_assert(json_type(&json) == JSON_TYPE_NULL);

    test_assert(json_set_null(NULL) == EFAULT);

    test_success();
}

test_case(json_set_and_get_boolean)
{
    struct json json = {0};

    json_set_boolean(&json, true);
    test_assert(json_type(&json) == JSON_TYPE_BOOLEAN);
    test_assert(json_boolean(&json) == true);

    json_set_boolean(&json, false);
    test_assert(json_boolean(&json) == false);

    test_success();
}

test_case(json_set_and_get_signed)
{
    struct json json = {0};

    json_set_signed(&json, -123);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_signed(&json) == -123);

    // From unsigned to signed
    json_set_unsigned(&json, -1);
    test_assert(json_signed(&json) == INT64_MAX);

    // From double to signed
    json_set_double(&json, -1.5);
    test_assert(json_signed(&json) == -1);

    test_success();
}

test_case(json_set_and_get_unsigned)
{
    struct json json = {0};

    json_set_unsigned(&json, 123);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_unsigned(&json) == 123);

    // From signed to unsigned
    json_set_signed(&json, -1);
    test_assert(json_unsigned(&json) == 0);

    // From double to unsigned
    json_set_double(&json, 1.5);
    test_assert(json_unsigned(&json) == 1);

    test_success();
}

test_case(json_set_and_get_double)
{
    struct json json = {0};

    json_set_double(&json, 1.5);
    test_assert(json_type(&json) == JSON_TYPE_NUMBER);
    test_assert(json_double(&json) == 1.5);

    // From signed to double
    json_set_signed(&json, -1);
    test_assert(json_double(&json) == -1.0);

    // From unsigned to double
    json_set_unsigned(&json, 1);
    test_assert(json_double(&json) == 1.0);

    test_success();
}

test_case(json_set_and_get_string)
{
    struct json json = {0};

    json_set_string(&json, "Hello, World!");
    test_assert(json_type(&json) == JSON_TYPE_STRING);
    test_assert(strcmp(json_string(&json), "Hello, World!") == 0);
    json_free(&json);

    test_success();
}
