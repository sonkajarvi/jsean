#include <errno.h>

#include <jsean/jsean.h>
#include <test/test.h>

test_case(jsean_set_array)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_set_array(NULL, 0) == EINVAL);

    // Default capacity
    test_assert(jsean_set_array(&a, 0) == 0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_ARRAY);
    test_assert(jsean_array_capacity(&a) == JSEAN_ARRAY_DEFAULT_CAPACITY);
    test_assert(jsean_array_size(&a) == 0);
    jsean_free(&a);

    // Custom capacity
    test_assert(jsean_set_array(&a, 10) == 0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_ARRAY);
    test_assert(jsean_array_capacity(&a) == 10);
    test_assert(jsean_array_size(&a) == 0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_size)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_array_size(NULL) == 0);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_array_size(&a) == 0);

    // Empty array
    jsean_set_array(&a, 0);
    test_assert(jsean_array_size(&a) == 0);
    jsean_array_clear(&a);

    // Populated array
    jsean_set_number(&b, 2.0);
    for (int i = 0; i < 100; i++)
        jsean_array_push(&a, &b);

    test_assert(jsean_array_size(&a) == 100);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_capacity)
{
    jsean_t a;

    // Invalid parameters
    test_assert(jsean_array_capacity(NULL) == 0);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_array_capacity(&a) == 0);

    // Valid array
    jsean_set_array(&a, 10);
    test_assert(jsean_array_capacity(&a) == 10);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_reserve)
{
    jsean_t a;

    // Invalid parameters
    test_assert(jsean_array_reserve(NULL, 0) == EINVAL);

    // Less than current capacity
    jsean_set_array(&a, 10);
    test_assert(jsean_array_reserve(&a, 1) == EINVAL);
    test_assert(jsean_array_capacity(&a) == 10);

    // More than current capacity
    test_assert(jsean_array_reserve(&a, 100) == 0);
    test_assert(jsean_array_capacity(&a) == 100);
    jsean_free(&a);

    // Reserve default
    jsean_set_array(&a, 1);
    test_assert(jsean_array_reserve(&a, 0) == 0);
    test_assert(jsean_array_capacity(&a) == JSEAN_ARRAY_DEFAULT_CAPACITY);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_clear)
{
    jsean_t a, b;

    jsean_set_array(&a, 0);
    jsean_array_reserve(&a, 100);
    test_assert(jsean_array_size(&a) == 0);

    // Empty array
    jsean_array_clear(&a);
    test_assert(jsean_array_size(&a) == 0);
    test_assert(jsean_array_capacity(&a) == 100);

    // Populated array
    jsean_set_number(&b, 2.0);
    for (int i = 0; i < 100; i++)
        jsean_array_push(&a, &b);

    test_assert(jsean_array_size(&a) == 100);
    test_assert(jsean_array_capacity(&a) == 100);
    jsean_array_clear(&a);

    test_assert(jsean_array_size(&a) == 0);
    test_assert(jsean_array_capacity(&a) == 100);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_at)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_array_at(NULL, 0) == NULL);

    // Empty array
    jsean_set_array(&a, 0);
    test_assert(jsean_array_at(&a, 0) == NULL);

    // Populated array
    jsean_set_number(&b, 2.0);
    jsean_array_push(&a, &b);
    test_assert(jsean_get_number(jsean_array_at(&a, 0)) == 2.0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_push)
{
    jsean_t a, b;

    // Invalid parameters
    jsean_set_number(&a, 2.0);
    jsean_set_number(&b, 2.0);
    test_assert(jsean_array_push(&a, &b) == EINVAL);

    test_assert(jsean_array_push(NULL, &b) == EINVAL);
    jsean_set_array(&a, 0);
    test_assert(jsean_array_push(&a, NULL) == EINVAL);

    // Valid value
    test_assert(jsean_array_push(&a, &b) == 0);
    test_assert(jsean_array_size(&a) == 1);
    test_assert(jsean_get_number(jsean_array_at(&a, 0)) == 2.0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_pop)
{
    jsean_t a, b;

    jsean_set_array(&a, 0);

    // Empty array
    test_assert(jsean_array_size(&a) == 0);
    jsean_array_pop(&a);
    test_assert(jsean_array_size(&a) == 0);

    // Populated array
    jsean_set_number(&b, 2.0);
    jsean_array_push(&a, &b);

    test_assert(jsean_array_size(&a) == 1);
    jsean_array_pop(&a);
    test_assert(jsean_array_size(&a) == 0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_insert)
{
    jsean_t a, b;

    // Not array
    jsean_set_number(&a, 2.0);
    test_assert(jsean_array_insert(&a, 0, &b) == EINVAL);

    // Invalid parameters
    test_assert(jsean_array_insert(&a, 0, NULL) == EINVAL);
    jsean_set_number(&b, 8.0);
    test_assert(jsean_array_insert(NULL, 0, &b) == EINVAL);

    // Out of bounds
    jsean_set_array(&a, 0);
    test_assert(jsean_array_insert(&a, 100, &b) == EINVAL);

    // Insert at the end
    test_assert(jsean_array_insert(&a, 0, &b) == 0);

    // Insert at the beginning
    jsean_set_number(&b, 2.0);
    test_assert(jsean_array_insert(&a, 0, &b) == 0);

    // Insert in the middle
    jsean_set_number(&b, 4.0);
    test_assert(jsean_array_insert(&a, 1, &b) == 0);

    test_assert(jsean_get_number(jsean_array_at(&a, 0)) == 2.0);
    test_assert(jsean_get_number(jsean_array_at(&a, 1)) == 4.0);
    test_assert(jsean_get_number(jsean_array_at(&a, 2)) == 8.0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_array_remove)
{
    jsean_t a, b;

    jsean_set_array(&a, 0);
    jsean_set_number(&b, 2.0);

    jsean_array_push(&a, &b);
    jsean_array_push(&a, &b);
    jsean_array_push(&a, &b);
    test_assert(jsean_array_size(&a) == 3);

    // Out of bounds
    jsean_array_remove(&a, 10);
    test_assert(jsean_array_size(&a) == 3);

    // Remove from the middle
    jsean_array_remove(&a, 1);
    test_assert(jsean_array_size(&a) == 2);

    // Remove from the beginning
    jsean_array_remove(&a, 0);
    test_assert(jsean_array_size(&a) == 1);

    // Remove from the end
    jsean_array_remove(&a, 0);
    test_assert(jsean_array_size(&a) == 0);
    jsean_free(&a);

    test_success();
}
