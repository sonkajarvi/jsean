#include <errno.h>

#include <jsean/JSON.h>
#include <test.h>

test_case(json_init_object)
{
    JSON json = {0};

    // Try initializing JSON object with invalid arguments
    test_assert(json_init_object(NULL, 0) == EFAULT);

    // Initialize JSON object with default capacity
    test_assert(json_init_object(&json, 0) == 0);
    test_assert(JSON_type(&json) == JSON_TYPE_OBJECT);
    test_assert(JSON_object_capacity(&json) == JSON_OBJECT_DEFAULT_CAPACITY);
    test_assert(JSON_object_count(&json) == 0);
    json_free(&json);

    // Initialize JSON object with own capacity
    test_assert(json_init_object(&json, 1) == 0);
    test_assert(JSON_type(&json) == JSON_TYPE_OBJECT);
    test_assert(JSON_object_capacity(&json) == 1);
    test_assert(JSON_object_count(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(JSON_object_count)
{
    JSON json = {0}, tmp = {0};

    // Try getting count of NULL
    test_assert(JSON_object_count(NULL) == 0);

    // Get count of empty JSON object
    json_init_object(&json, 0);
    test_assert(JSON_object_count(&json) == 0);

    // Get count of populated JSON object
    json_set_number(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        JSON_object_insert(&json, key, &tmp);
    }
    test_assert(JSON_object_count(&json) == 26);
    json_free(&json);

    // Try getting count of non-object JSON value
    json_set_number(&json, -1);
    test_assert(JSON_object_count(&json) == 0);

    test_success();
}

#include <stdio.h>

test_case(JSON_object_capacity)
{
    JSON json = {0}, tmp = {0};

    // Try getting capacity of NULL
    test_assert(JSON_object_capacity(NULL) == 0);

    // Get capacity of initialized JSON object
    json_init_object(&json, 0);
    const size_t capacity = JSON_object_capacity(&json);
    test_assert(capacity > 0);

    // Try getting capacity of populated JSON object
    json_set_number(&json, -1);
    test_assert(JSON_object_capacity(&json) == 0);

    test_success();
}

test_case(JSON_object_clear)
{
    JSON json = {0}, tmp = {0};

    json_init_object(&json, 0);
    JSON_object_clear(&json);
    test_assert(JSON_object_capacity(&json) > 0);

    // Clear populated JSON object
    json_set_number(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        JSON_object_insert(&json, key, &tmp);
    }
    JSON_object_clear(&json);
    test_assert(JSON_object_capacity(&json) > 0);
    test_assert(JSON_object_count(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(JSON_object_get)
{
    JSON json = {0}, tmp = {0};

    // Try getting value from NULL
    test_assert(JSON_object_get(NULL, "key") == NULL);

    // Try getting value from empty JSON object
    json_init_object(&json, 0);
    test_assert(JSON_object_get(&json, "key") == NULL);

    // Get value from populated JSON object
    json_set_number(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        JSON_object_insert(&json, key, &tmp);
    }
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(json_number(JSON_object_get(&json, key)) == -1);
    }
    json_free(&json);

    // Try getting value from non-object JSON value
    json_set_number(&json, -1);
    test_assert(JSON_object_get(&json, "key") == NULL);

    test_success();
}

test_case(JSON_object_insert)
{
    JSON json = {0}, tmp = {0};

    // Try inserting into unknown JSON value
    json_init_object(&json, 1);
    test_assert(JSON_object_insert(&json, "key", &tmp) == EINVAL);

    // Try inserting with invalid arguments
    json_set_number(&tmp, -1);
    test_assert(JSON_object_insert(NULL, "key", &tmp) == EFAULT);
    test_assert(JSON_object_insert(&json, NULL, &tmp) == EFAULT);
    test_assert(JSON_object_insert(&json, "key", NULL) == EFAULT);

    // Insert with valid arguments
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(JSON_object_insert(&json, key, &tmp) == 0);
    }
    test_assert(JSON_object_count(&json) == 26);

    // Try inserting with duplicate key
    test_assert(JSON_object_insert(&json, "key", &tmp) == 0);
    test_assert(JSON_object_insert(&json, "key", &tmp) == EINVAL);
    json_free(&json);

    // Try inserting into non-object JSON value
    json_set_number(&json, -1);
    test_assert(JSON_object_insert(&json, "key", &tmp) == EINVAL);

    test_success();
}

test_case(JSON_object_overwrite)
{
    JSON json = {0}, tmp = {0};

    // Try overwriting into unknown JSON value
    json_init_object(&json, 0);
    test_assert(JSON_object_overwrite(&json, "key", &tmp) == EINVAL);

    // Try overwriting with invalid arguments
    json_set_number(&tmp, -1);
    test_assert(JSON_object_overwrite(NULL, "key", &tmp) == EFAULT);
    test_assert(JSON_object_overwrite(&json, NULL, &tmp) == EFAULT);
    test_assert(JSON_object_overwrite(&json, "key", NULL) == EFAULT);

    // Overwrite with valid arguments
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(JSON_object_overwrite(&json, key, &tmp) == 0);
    }
    test_assert(JSON_object_count(&json) == 26);

    // Overwrite value
    test_assert(JSON_object_overwrite(&json, "key", &tmp) == 0);
    test_assert(json_number(JSON_object_get(&json, "key")) == -1);
    json_set_number(&tmp, 1);
    test_assert(JSON_object_overwrite(&json, "key", &tmp) == 0);
    test_assert(json_number(JSON_object_get(&json, "key")) == 1);
    json_free(&json);

    // Try overwriting into non-object JSON value
    json_set_number(&json, -1);
    test_assert(JSON_object_overwrite(&json, "key", &tmp) == EINVAL);

    test_success();
}

test_case(JSON_object_remove)
{
    JSON json = {0}, tmp = {0};

    json_init_object(&json, 0);
    json_set_number(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        JSON_object_insert(&json, key, &tmp);
    }

    // Try removing non-existent key
    JSON_object_remove(&json, "key");
    test_assert(JSON_object_count(&json) == 26);

    // Remove existing key
    JSON_object_remove(&json, "a");
    test_assert(JSON_object_count(&json) == 25);
    json_free(&json);

    test_success();
}