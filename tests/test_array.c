//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stddef.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_array_length)
{
    jsean a, b;

    // Null
    ASSERT(jsean_array_length(NULL) == 0);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_array_length(&b) == 0);

    // Empty array
    jsean_set_array(&a);
    ASSERT(jsean_array_length(&a) == 0);

    // Success
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_length(&a) == 1);

    jsean_free(&a);
}

TEST(jsean_array_at)
{
    jsean a, b;

    // Null
    ASSERT(jsean_array_at(NULL, 0) == NULL);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_array_at(&b, 0) == NULL);

    // Empty
    jsean_set_array(&a);
    ASSERT(jsean_array_at(&a, 0) == NULL);

    // Success
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_at(&a, 0) != NULL);

    jsean_free(&a);
}

TEST(jsean_array_insert)
{
    jsean a, b;

    // Null
    jsean_set_number(&b, 1.0);
    ASSERT(jsean_array_insert(NULL, 0, &b) == NULL);

    // Null
    jsean_set_array(&a);
    ASSERT(jsean_array_insert(&a, 0, NULL) == NULL);

    // Wrong type
    ASSERT(jsean_array_insert(&b, 0, &a) == NULL);

    // Out of bounds
    ASSERT(jsean_array_insert(&a, 1, &b) == NULL);

    // Success
    ASSERT(jsean_array_insert(&a, 0, &b) != NULL);
    ASSERT(jsean_get_number(jsean_array_at(&a, 0)) == 1.0);

    // Success
    jsean_set_number(&b, 2.0);
    ASSERT(jsean_array_insert(&a, 0, &b) != NULL);
    ASSERT(jsean_get_number(jsean_array_at(&a, 0)) == 2.0);
    ASSERT(jsean_get_number(jsean_array_at(&a, 1)) == 1.0);

    jsean_free(&a);
}

TEST(jsean_array_push)
{
    jsean a, b;

    // Null
    jsean_set_null(&b);
    ASSERT(jsean_array_push(NULL, &b) == NULL);

    // Null
    jsean_set_array(&a);
    ASSERT(jsean_array_push(&a, NULL) == NULL);

    // Wrong type
    ASSERT(jsean_array_push(&b, &a) == NULL);

    // Success
    ASSERT(jsean_array_push(&a, &b) != NULL);

    jsean_free(&a);
}

TEST(jsean_array_remove)
{
    jsean a, b;

    // Out of bounds
    jsean_set_array(&a);
    jsean_set_number(&b, 1.0);
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_length(&a) == 1);
    jsean_array_remove(&a, 1, NULL);
    ASSERT(jsean_array_length(&a) == 1);

    // Success
    jsean_set_number(&b, 2.0);
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_length(&a) == 2);
    ASSERT(jsean_get_number(jsean_array_at(&a, 0)) == 1.0);
    ASSERT(jsean_get_number(jsean_array_at(&a, 1)) == 2.0);

    jsean_array_remove(&a, 0, NULL);
    ASSERT(jsean_array_length(&a) == 1);
    ASSERT(jsean_get_number(jsean_array_at(&a, 0)) == 2.0);

    jsean_array_remove(&a, 0, NULL);
    ASSERT(jsean_array_length(&a) == 0);

    // Success
    jsean_set_number(&b, 64.0);
    jsean_array_push(&a, &b);
    jsean_set_null(&b);
    ASSERT(jsean_typeof(&b) != JSEAN_TYPE_NUMBER);
    jsean_array_remove(&a, 0, &b);
    ASSERT(jsean_typeof(&b) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_number(&b) == 64.0);

    jsean_free(&a);
}

TEST(jsean_array_pop)
{
    jsean a, b;

    // Empty
    jsean_set_array(&a);
    jsean_array_pop(&a, NULL);
    ASSERT(jsean_array_length(&a) == 0);

    // Success
    jsean_set_null(&b);
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_length(&a) == 1);
    jsean_array_pop(&a, NULL);
    ASSERT(jsean_array_length(&a) == 0);

    // Success
    jsean_set_number(&b, 64.0);
    jsean_array_push(&a, &b);
    jsean_set_null(&b);
    ASSERT(jsean_typeof(&b) != JSEAN_TYPE_NUMBER);

    jsean_array_pop(&a, &b);
    ASSERT(jsean_typeof(&b) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_number(&b) == 64.0);

    jsean_free(&a);
}

TEST(jsean_array_clear)
{
    jsean a, b;

    jsean_set_array(&a);
    jsean_set_null(&b);
    jsean_array_push(&a, &b);
    jsean_array_push(&a, &b);
    jsean_array_push(&a, &b);
    ASSERT(jsean_array_length(&a) == 3);

    jsean_array_clear(&a);
    ASSERT(jsean_array_length(&a) == 0);

    jsean_free(&a);
}
