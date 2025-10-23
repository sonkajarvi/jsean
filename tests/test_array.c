//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>

#include "jsean.h"
#include "test.h"

TEST(jsean_array, set_arr)
{
    jsean a;

    ASSERT(jsean_set_arr(NULL) == false);
    ASSERT(jsean_set_arr(&a) == true);

    jsean_free(&a);
}

TEST(jsean_array, len)
{
    jsean a, b;

    ASSERT(jsean_arr_len(NULL) == 0);

    jsean_set_null(&b);
    ASSERT(jsean_arr_len(&b) == 0);

    jsean_set_arr(&a);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 1);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, at)
{
    jsean a, b;

    ASSERT(jsean_arr_at(NULL, 0) == NULL);

    jsean_set_null(&b);
    ASSERT(jsean_arr_at(&b, 0) == NULL);

    jsean_set_arr(&a);
    ASSERT(jsean_arr_at(&a, 0) == NULL);

    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_at(&a, 0) != NULL);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, set)
{
    jsean a, b;

    jsean_set_num(&b, 1.0);
    ASSERT(jsean_arr_set(NULL, 0, &b) == NULL);

    jsean_set_arr(&a);
    ASSERT(jsean_arr_set(&a, 0, NULL) == NULL);
    ASSERT(jsean_arr_set(&b, 0, &a) == NULL);

    ASSERT(jsean_arr_set(&a, 1, &b) == NULL);

    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);

    jsean_set_num(&b, 2.0);
    ASSERT(jsean_arr_set(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);

    jsean_free(&b);
    jsean_set_num(&b, 4.0);
    ASSERT(jsean_arr_set(&a, 1, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 4.0);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, add)
{
    jsean a, b;

    jsean_set_num(&b, 1.0);
    ASSERT(jsean_arr_add(NULL, 0, &b) == NULL);

    jsean_set_arr(&a);
    ASSERT(jsean_arr_add(&a, 0, NULL) == NULL);
    ASSERT(jsean_arr_add(&b, 0, &a) == NULL);

    ASSERT(jsean_arr_add(&a, 1, &b) == NULL);

    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);

    jsean_free(&b);
    jsean_set_num(&b, 2.0);
    ASSERT(jsean_arr_add(&a, 0, &b) != NULL);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 1.0);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, push)
{
    jsean a, b;

    jsean_set_null(&b);
    ASSERT(jsean_arr_push(NULL, &b) == NULL);

    jsean_set_arr(&a);
    ASSERT(jsean_arr_push(&a, NULL) == NULL);
    ASSERT(jsean_arr_push(&b, &a) == NULL);

    ASSERT(jsean_arr_push(&a, &b) != NULL);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, del)
{
    jsean a, b;

    jsean_arr_del(NULL, 0);

    jsean_set_arr(&a);
    jsean_arr_del(&a, 0);

    jsean_set_num(&b, 1.0);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 1);
    jsean_arr_del(&a, 1);
    ASSERT(jsean_arr_len(&a) == 1);

    jsean_free(&b);
    jsean_set_num(&b, 2.0);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 2);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 1.0);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 1)) == 2.0);

    jsean_arr_del(&a, 0);
    ASSERT(jsean_arr_len(&a) == 1);
    ASSERT(jsean_get_num(jsean_arr_at(&a, 0)) == 2.0);

    jsean_arr_del(&a, 0);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, pop)
{
    jsean a, b;

    jsean_set_arr(&a);
    jsean_arr_pop(&a);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_set_null(&b);
    jsean_arr_push(&a, &b);
    ASSERT(jsean_arr_len(&a) == 1);
    jsean_arr_pop(&a);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_array, clear)
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
    jsean_free(&b);
}
