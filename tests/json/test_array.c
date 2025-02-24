#include <errno.h>

#include <jsean/JSON.h>
#include <test.h>

test_case(JSON_set_array)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_set_array(NULL, 0) == EINVAL);

    // Default capacity
    test_assert(JSON_set_array(&a, 0) == 0);
    test_assert(JSON_type(&a) == JSON_TYPE_ARRAY);
    test_assert(JSON_array_capacity(&a) == JSON_ARRAY_DEFAULT_CAPACITY);
    test_assert(JSON_array_size(&a) == 0);
    json_free(&a);

    // Custom capacity
    test_assert(JSON_set_array(&a, 10) == 0);
    test_assert(JSON_type(&a) == JSON_TYPE_ARRAY);
    test_assert(JSON_array_capacity(&a) == 10);
    test_assert(JSON_array_size(&a) == 0);
    json_free(&a);

    test_success();
}

test_case(JSON_array_size)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_array_size(NULL) == 0);

    json_set_number(&a, 2.0);
    test_assert(JSON_array_size(&a) == 0);

    // Empty array
    JSON_set_array(&a, 0);
    test_assert(JSON_array_size(&a) == 0);
    JSON_array_clear(&a);

    // Populated array
    json_set_number(&b, 2.0);
    for (int i = 0; i < 100; i++)
        JSON_array_push(&a, &b);

    test_assert(JSON_array_size(&a) == 100);
    json_free(&a);

    test_success();
}

test_case(JSON_array_capacity)
{
    JSON a;

    // Invalid parameters
    test_assert(JSON_array_capacity(NULL) == 0);

    json_set_number(&a, 2.0);
    test_assert(JSON_array_capacity(&a) == 0);

    // Valid array
    JSON_set_array(&a, 10);
    test_assert(JSON_array_capacity(&a) == 10);
    json_free(&a);

    test_success();
}

test_case(JSON_array_reserve)
{
    JSON a;

    // Invalid parameters
    test_assert(JSON_array_reserve(NULL, 0) == EINVAL);

    // Less than current capacity
    JSON_set_array(&a, 10);
    test_assert(JSON_array_reserve(&a, 0) == EINVAL);
    test_assert(JSON_array_capacity(&a) == 10);

    // More than current capacity
    test_assert(JSON_array_reserve(&a, 100) == 0);
    test_assert(JSON_array_capacity(&a) == 100);
    json_free(&a);

    test_success();
}

test_case(JSON_array_clear)
{
    JSON a, b;

    JSON_set_array(&a, 0);
    JSON_array_reserve(&a, 1000);

    // Empty array
    JSON_array_clear(&a);
    test_assert(JSON_array_capacity(&a) == 1000);

    // Populated array
    json_set_number(&b, 2.0);
    for (int i = 0; i < 1000; i++)
        JSON_array_push(&a, &b);

    JSON_array_clear(&a);
    test_assert(JSON_array_capacity(&a) == 1000);
    test_assert(JSON_array_size(&a) == 0);
    json_free(&a);

    test_success();
}

test_case(JSON_array_at)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_array_at(NULL, 0) == NULL);

    // Empty array
    JSON_set_array(&a, 0);
    test_assert(JSON_array_at(&a, 0) == NULL);

    // Populated array
    json_set_number(&b, 2.0);
    JSON_array_push(&a, &b);
    test_assert(JSON_array_at(&a, 0) != NULL);
    json_free(&a);

    test_success();
}

#include <stdio.h>

test_case(JSON_array_push)
{
    JSON a, b;

    // Invalid parameters
    json_set_number(&a, 2.0);
    json_set_number(&b, 2.0);
    test_assert(JSON_array_push(&a, &b) == EINVAL);

    test_assert(JSON_array_push(&a, NULL) == EINVAL);
    test_assert(JSON_array_push(NULL, &b) == EINVAL);

    // Valid value
    JSON_set_array(&a, 0);
    test_assert(JSON_array_push(&a, &b) == 0);
    test_assert(JSON_array_size(&a) == 1);
    test_assert(json_number(JSON_array_at(&a, 0)) == 2.0);
    json_free(&a);

    test_success();
}

test_case(JSON_array_pop)
{
    JSON a, b;

    JSON_set_array(&a, 0);

    // Empty array
    test_assert(JSON_array_size(&a) == 0);
    JSON_array_pop(&a);
    test_assert(JSON_array_size(&a) == 0);

    // Populated array
    json_set_number(&b, 2.0);
    JSON_array_push(&a, &b);

    test_assert(JSON_array_size(&a) == 1);
    JSON_array_pop(&a);
    test_assert(JSON_array_size(&a) == 0);
    json_free(&a);

    test_success();
}

test_case(JSON_array_insert)
{
    JSON a, b;

    // Not array
    json_set_number(&a, 2.0);
    test_assert(JSON_array_insert(&a, 0, &b) == EINVAL);

    // Invalid parameters
    test_assert(JSON_array_insert(&a, 0, NULL) == EINVAL);
    json_set_number(&b, 8.0);
    test_assert(JSON_array_insert(NULL, 0, &b) == EINVAL);

    // Out of bounds
    JSON_set_array(&a, 0);
    test_assert(JSON_array_insert(&a, 100, &b) == EINVAL);

    // Insert at the end
    test_assert(JSON_array_insert(&a, 0, &b) == 0);

    // Insert at the beginning
    json_set_number(&b, 2.0);
    test_assert(JSON_array_insert(&a, 0, &b) == 0);

    // Insert in the middle
    json_set_number(&b, 4.0);
    test_assert(JSON_array_insert(&a, 1, &b) == 0);

    test_assert(json_number(JSON_array_at(&a, 0)) == 2.0);
    test_assert(json_number(JSON_array_at(&a, 1)) == 4.0);
    test_assert(json_number(JSON_array_at(&a, 2)) == 8.0);
    json_free(&a);

    test_success();
}

test_case(JSON_array_remove)
{
    JSON a, b;

    JSON_set_array(&a, 0);
    json_set_number(&b, 2.0);

    JSON_array_push(&a, &b);
    JSON_array_push(&a, &b);
    JSON_array_push(&a, &b);
    test_assert(JSON_array_size(&a) == 3);

    // Out of bounds
    JSON_array_remove(&a, 10);
    test_assert(JSON_array_size(&a) == 3);

    // Remove from the middle
    JSON_array_remove(&a, 1);
    test_assert(JSON_array_size(&a) == 2);

    // Remove from the beginning
    JSON_array_remove(&a, 0);
    test_assert(JSON_array_size(&a) == 1);

    // Remove from the end
    JSON_array_remove(&a, 0);
    test_assert(JSON_array_size(&a) == 0);
    json_free(&a);

    test_success();
}
