//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_set_obj)
{
    jsean a;

    ASSERT(jsean_set_obj(NULL) == false);

    ASSERT(jsean_set_obj(&a) == true);

    jsean_free(&a);
}

TEST(jsean_obj_len)
{
    jsean a, b;

    // Null
    ASSERT(jsean_obj_len(NULL) == 0);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_obj_len(&b) == 0);

    // Empty
    jsean_set_obj(&a);
    ASSERT(jsean_obj_len(&a) == 0);

    // Ok
    jsean_obj_add(&a, JSEAN_S("a"), &b);
    ASSERT(jsean_obj_len(&a) == 1);

    jsean_free(&a);
}

TEST(jsean_obj_at)
{
    jsean a, b;

    // Null
    ASSERT(jsean_obj_at(NULL, NULL) == NULL);

    // Null
    jsean_set_obj(&a);
    ASSERT(jsean_obj_at(&a, NULL) == NULL);

    // Wrong type
    jsean_set_null(&b);
    ASSERT(jsean_obj_at(&b, JSEAN_S("a")) == NULL);

    // Empty
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) == NULL);

    // Ok
    jsean_obj_add(&a, JSEAN_S("a"), &b);
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) != NULL);

    jsean_free(&a);
}

TEST(jsean_obj_add)
{
    jsean a, b;

    // Null
    jsean_set_num(&b, 32.0);
    ASSERT(jsean_obj_add(NULL, JSEAN_S("a"), &b) == NULL);

    // Null
    jsean_set_obj(&a);
    ASSERT(jsean_obj_add(&a, NULL, &b) == NULL);

    // Null
    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), NULL) == NULL);

    // Wrong type
    ASSERT(jsean_obj_add(&b, JSEAN_S("a"), &a) == NULL);

    // Ok
    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 32.0);

    // Duplicate key
    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) == NULL);

    jsean_free(&a);
}

TEST(jsean_obj_set)
{
    jsean a, b;

    // Null
    jsean_set_num(&b, 32.0);
    ASSERT(jsean_obj_set(NULL, JSEAN_S("a"), &b) == NULL);

    // Null
    jsean_set_obj(&a);
    ASSERT(jsean_obj_set(&a, NULL, &b) == NULL);

    // Null
    ASSERT(jsean_obj_set(&a, JSEAN_S("a"), NULL) == NULL);

    // Wrong type
    ASSERT(jsean_obj_set(&b, JSEAN_S("a"), &a) == NULL);

    // Ok
    ASSERT(jsean_obj_set(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 32.0);

    // Ok
    jsean_set_num(&b, 64.0);
    ASSERT(jsean_obj_set(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 64.0);

    jsean_free(&a);
}

TEST(jsean_obj_del)
{
    jsean a, b;

    jsean_set_obj(&a);
    jsean_set_null(&b);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) != NULL);

    jsean_obj_del(&a, JSEAN_S("a"));
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) == NULL);

    jsean_free(&a);
}

TEST(jsean_obj_clear)
{
    jsean a, b;

    jsean_set_obj(&a);
    jsean_set_null(&b);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_obj_add(&a, JSEAN_S("b"), &b) != NULL);
    ASSERT(jsean_obj_add(&a, JSEAN_S("c"), &b) != NULL);
    ASSERT(jsean_obj_len(&a) == 3);

    jsean_object_clear(&a);
    ASSERT(jsean_obj_len(&a) == 0);

    jsean_free(&a);
}
