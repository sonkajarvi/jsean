#include <errno.h>

#include <jsean/JSON.h>
#include <test.h>

test_case(JSON_set_object)
{
    JSON a;

    // Invalid parameters
    test_assert(JSON_set_object(NULL) == EINVAL);

    // Valid
    test_assert(JSON_set_object(&a) == 0);
    test_assert(JSON_type(&a) == JSON_TYPE_OBJECT);
    test_assert(JSON_object_capacity(&a) == JSON_OBJECT_DEFAULT_CAPACITY);
    test_assert(JSON_object_count(&a) == 0);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_count)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_object_count(NULL) == 0);

    JSON_set_number(&a, 2.0);
    test_assert(JSON_object_count(&a) == 0);

    // Empty object
    JSON_set_object(&a);
    test_assert(JSON_object_count(&a) == 0);

    // Populated object
    JSON_set_number(&b, 2.0);
    JSON_object_insert(&a, "a", &b);
    test_assert(JSON_object_count(&a) == 1);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_capacity)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_object_capacity(NULL) == 0);

    JSON_set_number(&a, 2.0);
    test_assert(JSON_object_count(&a) == 0);

    // Valid
    JSON_set_object(&a);
    test_assert(JSON_object_capacity(&a) == JSON_OBJECT_DEFAULT_CAPACITY);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_clear)
{
    JSON a, b;

    JSON_set_object(&a);
    test_assert(JSON_object_count(&a) == 0);
    test_assert(JSON_object_capacity(&a) == JSON_OBJECT_DEFAULT_CAPACITY);

    // Empty object
    JSON_object_clear(&a);
    test_assert(JSON_object_count(&a) == 0);
    test_assert(JSON_object_capacity(&a) == JSON_OBJECT_DEFAULT_CAPACITY);

    // Populated object
    JSON_set_number(&b, 2.0);
    JSON_object_insert(&a, "a", &b);
    JSON_object_insert(&a, "b", &b);
    JSON_object_insert(&a, "c", &b);
    test_assert(JSON_object_count(&a) == 3);
    test_assert(JSON_object_capacity(&a) > 0);

    JSON_object_clear(&a);
    test_assert(JSON_object_count(&a) == 0);
    test_assert(JSON_object_capacity(&a) > 0);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_get)
{
    JSON a, b;

    // Invalid parameters
    test_assert(JSON_object_get(NULL, "a") == NULL);

    JSON_set_number(&a, 2.0);
    test_assert(JSON_object_get(&a, "a") == NULL);

    JSON_set_object(&a);
    test_assert(JSON_object_get(&a, NULL) == NULL);

    // Empty object
    test_assert(JSON_object_get(&a, "a") == NULL);

    // Populated object
    JSON_set_number(&b, 2.0);
    JSON_object_insert(&a, "a", &b);
    test_assert(JSON_number(JSON_object_get(&a, "a")) == 2.0);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_insert)
{
    JSON a, b;

    // Invalid parameters
    JSON_set_number(&a, 2.0);
    JSON_set_number(&b, 2.0);
    test_assert(JSON_object_insert(&a, "a", &b) == EINVAL);

    test_assert(JSON_object_insert(NULL, "a", &b) == EINVAL);
    JSON_set_object(&a);
    test_assert(JSON_object_insert(&a, NULL, &b) == EINVAL);
    test_assert(JSON_object_insert(&a, "a", NULL) == EINVAL);

    // Valid values
    test_assert(JSON_object_insert(&a, "a", &b) == 0);
    test_assert(JSON_object_count(&a) == 1);
    test_assert(JSON_number(JSON_object_get(&a, "a")) == 2.0);

    // Duplicate key
    test_assert(JSON_object_insert(&a, "a", &b) == EINVAL);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_overwrite)
{
    JSON a, b;

    // Invalid parameters
    JSON_set_number(&a, 2.0);
    JSON_set_number(&b, 2.0);
    test_assert(JSON_object_overwrite(&a, "a", &b) == EINVAL);

    test_assert(JSON_object_overwrite(NULL, "a", &b) == EINVAL);
    JSON_set_object(&a);
    test_assert(JSON_object_overwrite(&a, NULL, &b) == EINVAL);
    test_assert(JSON_object_overwrite(&a, "a", NULL) == EINVAL);

    // Valid values
    test_assert(JSON_object_overwrite(&a, "a", &b) == 0);
    test_assert(JSON_object_count(&a) == 1);
    test_assert(JSON_number(JSON_object_get(&a, "a")) == 2.0);

    // Duplicate key
    JSON_set_number(&b, 4.0);
    test_assert(JSON_object_overwrite(&a, "a", &b) == 0);
    test_assert(JSON_object_count(&a) == 1);
    test_assert(JSON_number(JSON_object_get(&a, "a")) == 4.0);
    JSON_free(&a);

    test_success();
}

test_case(JSON_object_remove)
{
    JSON a, b;

    JSON_set_object(&a);
    JSON_set_number(&b, 2.0);
    JSON_object_insert(&a, "a", &b);
    test_assert(JSON_object_count(&a) == 1);

    // Invalid key
    JSON_object_remove(&a, "b");
    test_assert(JSON_object_count(&a) == 1);

    // Valid key
    JSON_object_remove(&a, "a");
    test_assert(JSON_object_count(&a) == 0);
    JSON_free(&a);

    test_success();
}