#include <errno.h>

#include <jsean/json.h>
#include <test.h>

test_case(json_set_object)
{
    struct json json = {0};

    test_assert(json_set_object(&json) == 0);
    test_assert(json_type(&json) == JSON_TYPE_OBJECT);
    test_assert(json_object_capacity(&json) > 0);
    test_assert(json_object_count(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_object_count)
{
    struct json json = {0}, tmp = {0};

    // Try getting count of NULL
    test_assert(json_object_count(NULL) == 0);

    // Get count of empty JSON object
    json_set_object(&json);
    test_assert(json_object_count(&json) == 0);

    // Get count of populated JSON object
    json_set_signed(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        json_object_insert(&json, key, &tmp);
    }
    test_assert(json_object_count(&json) == 26);
    json_free(&json);

    // Try getting count of non-object JSON value
    json_set_signed(&json, -1);
    test_assert(json_object_count(&json) == 0);

    test_success();
}

#include <stdio.h>

test_case(json_object_capacity)
{
    struct json json = {0}, tmp = {0};

    // Try getting capacity of NULL
    test_assert(json_object_capacity(NULL) == 0);

    // Get capacity of initialized JSON object
    json_set_object(&json);
    const size_t capacity = json_object_capacity(&json);
    test_assert(capacity > 0);

    // Try getting capacity of populated JSON object
    json_set_signed(&json, -1);
    test_assert(json_object_capacity(&json) == 0);

    test_success();
}

test_case(json_object_clear)
{
    struct json json = {0}, tmp = {0};

    json_set_object(&json);
    json_object_clear(&json);
    test_assert(json_object_capacity(&json) > 0);

    // Clear populated JSON object
    json_set_signed(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        json_object_insert(&json, key, &tmp);
    }
    json_object_clear(&json);
    test_assert(json_object_capacity(&json) > 0);
    test_assert(json_object_count(&json) == 0);
    json_free(&json);

    test_success();
}

test_case(json_object_get)
{
    struct json json = {0}, tmp = {0};

    // Try getting value from NULL
    test_assert(json_object_get(NULL, "key") == NULL);

    // Try getting value from empty JSON object
    json_set_object(&json);
    test_assert(json_object_get(&json, "key") == NULL);

    // Get value from populated JSON object
    json_set_signed(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        json_object_insert(&json, key, &tmp);
    }
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(json_signed(json_object_get(&json, key)) == -1);
    }
    json_free(&json);

    // Try getting value from non-object JSON value
    json_set_signed(&json, -1);
    test_assert(json_object_get(&json, "key") == NULL);

    test_success();
}

test_case(json_object_insert)
{
    struct json json = {0}, tmp = {0};

    // Try inserting into unknown JSON value
    json_set_object(&json);
    test_assert(json_object_insert(&json, "key", &tmp) == EINVAL);

    // Try inserting with invalid arguments
    json_set_signed(&tmp, -1);
    test_assert(json_object_insert(NULL, "key", &tmp) == EFAULT);
    test_assert(json_object_insert(&json, NULL, &tmp) == EFAULT);
    test_assert(json_object_insert(&json, "key", NULL) == EFAULT);

    // Insert with valid arguments
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(json_object_insert(&json, key, &tmp) == 0);
    }
    test_assert(json_object_count(&json) == 26);

    // Try inserting with duplicate key
    test_assert(json_object_insert(&json, "key", &tmp) == 0);
    test_assert(json_object_insert(&json, "key", &tmp) == EINVAL);
    json_free(&json);

    // Try inserting into non-object JSON value
    json_set_signed(&json, -1);
    test_assert(json_object_insert(&json, "key", &tmp) == EINVAL);

    test_success();
}

test_case(json_object_overwrite)
{
    struct json json = {0}, tmp = {0};

    // Try overwriting into unknown JSON value
    json_set_object(&json);
    test_assert(json_object_overwrite(&json, "key", &tmp) == EINVAL);

    // Try overwriting with invalid arguments
    json_set_signed(&tmp, -1);
    test_assert(json_object_overwrite(NULL, "key", &tmp) == EFAULT);
    test_assert(json_object_overwrite(&json, NULL, &tmp) == EFAULT);
    test_assert(json_object_overwrite(&json, "key", NULL) == EFAULT);

    // Overwrite with valid arguments
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        test_assert(json_object_overwrite(&json, key, &tmp) == 0);
    }
    test_assert(json_object_count(&json) == 26);

    // Overwrite value
    test_assert(json_object_overwrite(&json, "key", &tmp) == 0);
    test_assert(json_signed(json_object_get(&json, "key")) == -1);
    json_set_signed(&tmp, 1);
    test_assert(json_object_overwrite(&json, "key", &tmp) == 0);
    test_assert(json_signed(json_object_get(&json, "key")) == 1);
    json_free(&json);

    // Try overwriting into non-object JSON value
    json_set_signed(&json, -1);
    test_assert(json_object_overwrite(&json, "key", &tmp) == EINVAL);

    test_success();
}

test_case(json_object_remove)
{
    struct json json = {0}, tmp = {0};

    json_set_object(&json);
    json_set_signed(&tmp, -1);
    for (int i = 'a'; i <= 'z'; i++) {
        char key[2] = { i, '\0' };
        json_object_insert(&json, key, &tmp);
    }

    // Try removing non-existent key
    json_object_remove(&json, "key");
    test_assert(json_object_count(&json) == 26);

    // Remove existing key
    json_object_remove(&json, "a");
    test_assert(json_object_count(&json) == 25);
    json_free(&json);

    test_success();
}