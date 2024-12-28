#include <errno.h>

#include <jsean/json.h>
#include <test.h>

test_case(json_set_array)
{
    struct json json = {0};

    json_set_array(&json);
    test_assert(json_type(&json) == JSON_TYPE_ARRAY);
    test_assert(json_array_length(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_array_length)
{
    struct json json = {0}, tmp = {0};

    // Length of NULL
    test_assert(json_array_length(NULL) == 0);

    // Length of empty JSON array
    json_set_array(&json);
    test_assert(json_array_length(&json) == 0);
    json_array_clear(&json);

    // Length of populated JSON array
    json_set_signed(&tmp, -1);
    for (int i = 0; i < 1000; i++) {
        json_array_push(&json, &tmp);
    }
    test_assert(json_array_length(&json) == 1000);
    json_free(&json);

    // Length of non-array JSON value
    json_set_signed(&json, -1);
    test_assert(json_array_length(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_array_capacity)
{
    struct json json = {0};

    // Capacity of NULL
    test_assert(json_array_capacity(NULL) == 0);

    // Capacity of initialized JSON array
    json_set_array(&json);
    test_assert(json_array_capacity(&json) > 0);
    json_free(&json);

    // Capacity of non-array JSON value
    json_set_signed(&json, -1);
    test_assert(json_array_capacity(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_array_reserve)
{
    struct json json = {0};

    // Reserve on NULL
    test_assert(json_array_reserve(NULL, 0) == EFAULT);

    // Reserve invalid
    json_set_array(&json);
    test_assert(json_array_reserve(&json, 0) == EINVAL);

    // Reserve valid
    test_assert(json_array_reserve(&json, 100) == 0);
    test_assert(json_array_capacity(&json) >= 100);
    json_free(&json);

    test_success();
}

test_case(json_array_clear)
{
    struct json json = {0}, tmp = {0};

    json_set_array(&json);
    json_array_reserve(&json, 1000);
    json_array_clear(&json);
    test_assert(json_array_capacity(&json) >= 1000);

    // Clear populated JSON array
    json_set_signed(&tmp, -1);
    for (int i = 0; i < 1000; i++)
        json_array_push(&json, &tmp);
    json_array_clear(&json);
    test_assert(json_array_length(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_array_at)
{
    struct json json = {0}, tmp = {0};

    // At NULL
    test_assert(json_array_at(NULL, 0) == NULL);

    // At empty JSON array
    json_set_array(&json);
    test_assert(json_array_at(&json, 0) == NULL);

    // At populated JSON array
    json_set_signed(&tmp, -1);
    json_array_push(&json, &tmp);
    test_assert(json_array_at(&json, 0) != NULL);
    json_free(&json);

    test_success();
}

test_case(json_array_push)
{
    struct json json = {0}, tmp = {0};

    // Push with invalid arguments
    test_assert(json_array_push(&json, NULL) == EFAULT);
    test_assert(json_array_push(NULL, &tmp) == EFAULT);

    // Push unknown JSON value
    json_set_array(&json);
    test_assert(json_array_push(&json, &tmp) == EINVAL);

    // Push with valid arguments
    json_set_signed(&tmp, -1);
    for (int i = 0; i < 1000; i++)
        json_array_push(&json, &tmp);
    test_assert(json_array_length(&json) == 1000);
    json_free(&json);

    test_success();
}

test_case(json_array_pop)
{
    struct json json = {0}, tmp = {0};

    json_set_array(&json);
    json_set_signed(&tmp, -1);

    for (int i = 0; i < 1000; i++)
        json_array_push(&json, &tmp);
    test_assert(json_array_length(&json) == 1000);

    for (int i = 999; i >= 0; i--) {
        json_array_pop(&json);
        test_assert(json_array_length(&json) == i);
    }
    json_free(&json);

    test_success();
}

test_case(json_array_insert)
{
    struct json json = {0}, tmp = {0};

    // Insert with invalid arguments
    test_assert(json_array_insert(&json, 0, NULL) == EFAULT);
    test_assert(json_array_insert(NULL, 0, &tmp) == EFAULT);

    // Insert unknown JSON value
    json_set_array(&json);
    test_assert(json_array_insert(&json, 0, &tmp) == EINVAL);

    // Insert out of bounds
    json_set_signed(&tmp, -1);
    test_assert(json_array_insert(&json, 1, &tmp) == EINVAL);

    json_set_signed(&tmp, 1);
    json_array_push(&json, &tmp);
    json_set_signed(&tmp, 3);
    json_array_push(&json, &tmp);
    
    // Insert at the beginning
    json_set_signed(&tmp, 0);
    test_assert(json_array_insert(&json, 0, &tmp) == 0);

    // Insert at the middle
    json_set_signed(&tmp, 2);
    test_assert(json_array_insert(&json, 2, &tmp) == 0);

    // Insert at the end
    json_set_signed(&tmp, 4);
    test_assert(json_array_insert(&json, 4, &tmp) == 0);

    test_assert(json_array_length(&json) == 5);
    for (int i = 0; i < 5; i++)
        test_assert(json_signed(json_array_at(&json, i)) == i);
    json_free(&json);

    test_success();
}

test_case(json_array_remove)
{
    struct json json = {0}, tmp = {0};

    json_set_array(&json);
    json_set_signed(&tmp, -1);
    for (int i = 0; i < 3; i++)
        json_array_push(&json, &tmp);

    // Remove out of bounds
    json_array_remove(&json, 3);
    test_assert(json_array_length(&json) == 3);

    // Remove at the middle
    json_array_remove(&json, 1);
    test_assert(json_array_length(&json) == 2);

    // Remove at the beginning
    json_array_remove(&json, 0);
    test_assert(json_array_length(&json) == 1);

    // Remove at the end
    json_array_remove(&json, 0);
    test_assert(json_array_length(&json) == 0);
    json_free(&json);

    test_success();
}
