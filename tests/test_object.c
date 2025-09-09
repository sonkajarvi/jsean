//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stddef.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_object_count)
{
    jsean a, b;

    // Null
    ASSERT(jsean_object_count(NULL) == 0);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_object_count(&b) == 0);

    // Empty object
    jsean_set_object(&a);
    ASSERT(jsean_object_count(&a) == 0);

    // Success
    jsean_object_insert(&a, "a", &b);
    ASSERT(jsean_object_count(&a) == 1);

    jsean_free(&a);
}

TEST(jsean_object_get)
{
    jsean a, b;

    // Null
    ASSERT(jsean_object_get(NULL, "a") == NULL);

    // Null
    jsean_set_object(&a);
    ASSERT(jsean_object_get(&a, NULL) == NULL);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_object_get(&b, NULL) == NULL);

    // No key
    ASSERT(jsean_object_get(&a, "a") == NULL);

    // Success
    jsean_object_insert(&a, "a", &b);
    ASSERT(jsean_object_get(&a, "a") != NULL);

    jsean_free(&a);
}

TEST(jsean_object_insert)
{
    jsean a, b;

    // Null
    jsean_set_number(&b, 32.0);
    ASSERT(jsean_object_insert(NULL, "a", &b) == NULL);

    // Null
    jsean_set_object(&a);
    ASSERT(jsean_object_insert(&a, NULL, &b) == NULL);

    // Null
    ASSERT(jsean_object_insert(&a, "a", NULL) == NULL);

    // Wrong type
    ASSERT(jsean_object_insert(&b, "a", &a) == NULL);

    // Success
    ASSERT(jsean_object_insert(&a, "a", &b) != NULL);
    ASSERT(jsean_typeof(jsean_object_get(&a, "a")) == JSEAN_NUMBER);
    ASSERT(jsean_get_number(jsean_object_get(&a, "a")) == 32.0);

    // Duplicate key
    ASSERT(jsean_object_insert(&a, "a", &b) == NULL);

    jsean_free(&a);
}

TEST(jsean_object_replace)
{
    jsean a, b;

    // Null
    jsean_set_number(&b, 32.0);
    ASSERT(jsean_object_replace(NULL, "a", &b) == NULL);

    // Null
    jsean_set_object(&a);
    ASSERT(jsean_object_replace(&a, NULL, &b) == NULL);

    // Null
    ASSERT(jsean_object_replace(&a, "a", NULL) == NULL);

    // Wrong type
    ASSERT(jsean_object_replace(&b, "a", &a) == NULL);

    // Success
    ASSERT(jsean_object_replace(&a, "a", &b) != NULL);
    ASSERT(jsean_typeof(jsean_object_get(&a, "a")) == JSEAN_NUMBER);
    ASSERT(jsean_get_number(jsean_object_get(&a, "a")) == 32.0);

    // Success
    jsean_set_number(&b, 64.0);
    ASSERT(jsean_object_replace(&a, "a", &b) != NULL);
    ASSERT(jsean_typeof(jsean_object_get(&a, "a")) == JSEAN_NUMBER);
    ASSERT(jsean_get_number(jsean_object_get(&a, "a")) == 64.0);

    jsean_free(&a);
}

TEST(jsean_object_remove)
{
    jsean a, b;

    // Success
    jsean_set_object(&a);
    jsean_set_null(&b);
    jsean_object_insert(&a, "a", &b);
    ASSERT(jsean_object_get(&a, "a") != NULL);

    jsean_object_remove(&a, "a");
    ASSERT(jsean_object_get(&a, "a") == NULL);

    jsean_free(&a);
}

TEST(jsean_object_clear)
{
    jsean a, b;

    // Success
    jsean_set_object(&a);
    jsean_set_null(&b);
    jsean_object_insert(&a, "a", &b);
    jsean_object_insert(&a, "b", &b);
    jsean_object_insert(&a, "c", &b);
    ASSERT(jsean_object_count(&a) == 3);

    jsean_object_clear(&a);
    ASSERT(jsean_object_count(&a) == 0);

    jsean_free(&a);
}
