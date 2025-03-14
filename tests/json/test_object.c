#include <errno.h>

#include <jsean/jsean.h>
#include <test/test.h>

test_case(jsean_set_object)
{
    jsean_t a;

    // Invalid parameters
    test_assert(jsean_set_object(NULL) == EINVAL);

    // Valid
    test_assert(jsean_set_object(&a) == 0);
    test_assert(jsean_type(&a) == JSEAN_TYPE_OBJECT);
    test_assert(jsean_object_capacity(&a) == JSEAN_OBJECT_DEFAULT_CAPACITY);
    test_assert(jsean_object_count(&a) == 0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_count)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_object_count(NULL) == 0);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_object_count(&a) == 0);

    // Empty object
    jsean_set_object(&a);
    test_assert(jsean_object_count(&a) == 0);

    // Populated object
    jsean_set_number(&b, 2.0);
    jsean_object_insert(&a, "a", &b);
    test_assert(jsean_object_count(&a) == 1);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_capacity)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_object_capacity(NULL) == 0);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_object_count(&a) == 0);

    // Valid
    jsean_set_object(&a);
    test_assert(jsean_object_capacity(&a) == JSEAN_OBJECT_DEFAULT_CAPACITY);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_clear)
{
    jsean_t a, b;

    jsean_set_object(&a);
    test_assert(jsean_object_count(&a) == 0);
    test_assert(jsean_object_capacity(&a) == JSEAN_OBJECT_DEFAULT_CAPACITY);

    // Empty object
    jsean_object_clear(&a);
    test_assert(jsean_object_count(&a) == 0);
    test_assert(jsean_object_capacity(&a) == JSEAN_OBJECT_DEFAULT_CAPACITY);

    // Populated object
    jsean_set_number(&b, 2.0);
    jsean_object_insert(&a, "a", &b);
    jsean_object_insert(&a, "b", &b);
    jsean_object_insert(&a, "c", &b);
    test_assert(jsean_object_count(&a) == 3);
    test_assert(jsean_object_capacity(&a) > 0);

    jsean_object_clear(&a);
    test_assert(jsean_object_count(&a) == 0);
    test_assert(jsean_object_capacity(&a) > 0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_get)
{
    jsean_t a, b;

    // Invalid parameters
    test_assert(jsean_object_get(NULL, "a") == NULL);

    jsean_set_number(&a, 2.0);
    test_assert(jsean_object_get(&a, "a") == NULL);

    jsean_set_object(&a);
    test_assert(jsean_object_get(&a, NULL) == NULL);

    // Empty object
    test_assert(jsean_object_get(&a, "a") == NULL);

    // Populated object
    jsean_set_number(&b, 2.0);
    jsean_object_insert(&a, "a", &b);
    test_assert(jsean_get_number(jsean_object_get(&a, "a")) == 2.0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_insert)
{
    jsean_t a, b;

    // Invalid parameters
    jsean_set_number(&a, 2.0);
    jsean_set_number(&b, 2.0);
    test_assert(jsean_object_insert(&a, "a", &b) == EINVAL);

    test_assert(jsean_object_insert(NULL, "a", &b) == EINVAL);
    jsean_set_object(&a);
    test_assert(jsean_object_insert(&a, NULL, &b) == EINVAL);
    test_assert(jsean_object_insert(&a, "a", NULL) == EINVAL);

    // Valid values
    test_assert(jsean_object_insert(&a, "a", &b) == 0);
    test_assert(jsean_object_count(&a) == 1);
    test_assert(jsean_get_number(jsean_object_get(&a, "a")) == 2.0);

    // Duplicate key
    test_assert(jsean_object_insert(&a, "a", &b) == EINVAL);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_overwrite)
{
    jsean_t a, b;

    // Invalid parameters
    jsean_set_number(&a, 2.0);
    jsean_set_number(&b, 2.0);
    test_assert(jsean_object_overwrite(&a, "a", &b) == EINVAL);

    test_assert(jsean_object_overwrite(NULL, "a", &b) == EINVAL);
    jsean_set_object(&a);
    test_assert(jsean_object_overwrite(&a, NULL, &b) == EINVAL);
    test_assert(jsean_object_overwrite(&a, "a", NULL) == EINVAL);

    // Valid values
    test_assert(jsean_object_overwrite(&a, "a", &b) == 0);
    test_assert(jsean_object_count(&a) == 1);
    test_assert(jsean_get_number(jsean_object_get(&a, "a")) == 2.0);

    // Duplicate key
    jsean_set_number(&b, 4.0);
    test_assert(jsean_object_overwrite(&a, "a", &b) == 0);
    test_assert(jsean_object_count(&a) == 1);
    test_assert(jsean_get_number(jsean_object_get(&a, "a")) == 4.0);
    jsean_free(&a);

    test_success();
}

test_case(jsean_object_remove)
{
    jsean_t a, b;

    jsean_set_object(&a);
    jsean_set_number(&b, 2.0);
    jsean_object_insert(&a, "a", &b);
    test_assert(jsean_object_count(&a) == 1);

    // Invalid key
    jsean_object_remove(&a, "b");
    test_assert(jsean_object_count(&a) == 1);

    // Valid key
    jsean_object_remove(&a, "a");
    test_assert(jsean_object_count(&a) == 0);
    jsean_free(&a);

    test_success();
}