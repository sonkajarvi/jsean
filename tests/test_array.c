//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_set_arr)
{
    jsean a;

    ASSERT(jsean_set_arr(NULL) == false);
    ASSERT(jsean_set_arr(&a) == true);

    jsean_free(&a);
}

TEST(jsean_arr_len)
{
    jsean a, b;

    // Null
    ASSERT(jsean_arr_len(NULL) == 0);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_arr_len(&b) == 0);

    // Empty
    jsean_set_arr(&a);
    ASSERT(jsean_arr_len(&a) == 0);

    // Ok
    ASSERT(jsean_arr_push(&a, &b) != NULL);
    ASSERT(jsean_arr_len(&a) == 1);

    jsean_free(&a);
}

TEST(jsean_arr_at)
{
    jsean a, b;

    // Null
    ASSERT(jsean_arr_at(NULL, 0) == NULL);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_arr_at(&b, 0) == NULL);

    // Empty
    jsean_set_arr(&a);
    ASSERT(jsean_arr_at(&a, 0) == NULL);

    // Ok
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_at(&a, 0) != NULL);

    jsean_free(&a);
}

TEST(jsean_arr_set)
{
    jsean a, b;

    // Null
    jsean_set_num(&b, 1.0);
    ASSERT(jsean_arr_set(NULL, 0, &b) == NULL);

    // Null
    jsean_set_arr(&a);
    ASSERT(jsean_arr_set(&a, 0, NULL) == NULL);

    // Wrong type
    ASSERT(jsean_arr_set(&b, 0, &a) == NULL);

    // Out of bounds
    ASSERT(jsean_arr_set(&a, 1, &b) == NULL);

    // Ok
    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);

    jsean_set_num(&b, 2.0);
    ASSERT(jsean_arr_set(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);

    jsean_set_num(&b, 4.0);
    ASSERT(jsean_arr_set(&a, 1, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 4.0);

    jsean_free(&a);
}

TEST(jsean_arr_add)
{
    jsean a, b;

    // Null
    jsean_set_num(&b, 1.0);
    ASSERT(jsean_arr_add(NULL, 0, &b) == NULL);

    // Null
    jsean_set_arr(&a);
    ASSERT(jsean_arr_add(&a, 0, NULL) == NULL);

    // Wrong type
    ASSERT(jsean_arr_add(&b, 0, &a) == NULL);

    // Out of bounds
    ASSERT(jsean_arr_add(&a, 1, &b) == NULL);

    // Ok
    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);

    // Ok
    jsean_set_num(&b, 2.0);
    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 1.0);

    jsean_free(&a);
}

TEST(jsean_arr_push)
{
    jsean a, b;

    // Null
    jsean_set_null(&b);
    ASSERT(jsean_arr_push(NULL, &b) == NULL);

    // Null
    jsean_set_arr(&a);
    ASSERT(jsean_arr_push(&a, NULL) == NULL);

    // Wrong type
    ASSERT(jsean_arr_push(&b, &a) == NULL);

    // Ok
    ASSERT(jsean_arr_push(&a, &b) != NULL);

    jsean_free(&a);
}

TEST(jsean_arr_del)
{
    jsean a, b;

    // Out of bounds
    jsean_set_arr(&a);
    jsean_set_num(&b, 1.0);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 1);
    jsean_arr_del(&a, 1, NULL);
    ASSERT(jsean_arr_len(&a) == 1);

    // Ok
    jsean_set_num(&b, 2.0);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 2);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 2.0);

    jsean_arr_del(&a, 0, NULL);
    ASSERT(jsean_arr_len(&a) == 1);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);

    jsean_arr_del(&a, 0, NULL);
    ASSERT(jsean_arr_len(&a) == 0);

    // Ok
    jsean_set_num(&b, 64.0);
    jsean_arr_push(&a, &b);
    jsean_set_null(&b);
    ASSERT(jsean_get_type(&b) != JSEAN_TYPE_NUMBER);
    jsean_arr_del(&a, 0, &b);
    ASSERT(jsean_get_type(&b) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&b) == 64.0);

    jsean_free(&a);
}

TEST(jsean_arr_pop)
{
    jsean a, b;

    // Empty
    jsean_set_arr(&a);
    jsean_arr_pop(&a, NULL);
    ASSERT(jsean_arr_len(&a) == 0);

    // Ok
    jsean_set_null(&b);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 1);
    jsean_arr_pop(&a, NULL);
    ASSERT(jsean_arr_len(&a) == 0);

    // Ok
    jsean_set_num(&b, 64.0);
    jsean_arr_push(&a, &b);
    jsean_set_null(&b);
    ASSERT(jsean_get_type(&b) != JSEAN_TYPE_NUMBER);

    jsean_arr_pop(&a, &b);
    ASSERT(jsean_get_type(&b) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&b) == 64.0);

    jsean_free(&a);
}

TEST(jsean_arr_clear)
{
    jsean a, b;

    jsean_set_arr(&a);
    jsean_set_null(&b);
    jsean_arr_push(&a, &b);
    jsean_arr_push(&a, &b);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 3);

    jsean_arr_clear(&a);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_free(&a);
}
