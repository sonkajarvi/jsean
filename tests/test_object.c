//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>

#include "jsean.h"
#include "test.h"

TEST(jsean_object, obj)
{
    jsean a;

    ASSERT(jsean_set_obj(NULL) != JSEAN_SUCCESS);
    ASSERT(jsean_set_obj(&a) == JSEAN_SUCCESS);

    jsean_free(&a);
}

TEST(jsean_object, len)
{
    jsean a, b;

    ASSERT(jsean_obj_len(NULL) == 0);

    jsean_set_null(&b);
    ASSERT(jsean_obj_len(&b) == 0);

    jsean_set_obj(&a);
    ASSERT(jsean_obj_len(&a) == 0);

    jsean_obj_add(&a, JSEAN_S("a"), &b);
    ASSERT(jsean_obj_len(&a) == 1);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_object, at)
{
    jsean a, b;

    ASSERT(jsean_obj_at(NULL, NULL) == NULL);

    jsean_set_obj(&a);
    ASSERT(jsean_obj_at(&a, NULL) == NULL);

    jsean_set_null(&b);
    ASSERT(jsean_obj_at(&b, JSEAN_S("a")) == NULL);
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) == NULL);

    jsean_obj_add(&a, JSEAN_S("a"), &b);
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) != NULL);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_object, add)
{
    jsean a, b;

    jsean_set_num(&b, 32.0);
    ASSERT(jsean_obj_add(NULL, JSEAN_S("a"), &b) == NULL);

    jsean_set_obj(&a);
    ASSERT(jsean_obj_add(&a, NULL, &b) == NULL);
    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), NULL) == NULL);
    ASSERT(jsean_obj_add(&b, JSEAN_S("a"), &a) == NULL);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 32.0);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) == NULL);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_object, set)
{
    jsean a, b, c;

    jsean_set_num(&b, 32.0);
    ASSERT(jsean_obj_set(NULL, JSEAN_S("a"), &b) == NULL);

    jsean_set_obj(&a);
    ASSERT(jsean_obj_set(&a, NULL, &b) == NULL);
    ASSERT(jsean_obj_set(&a, JSEAN_S("a"), NULL) == NULL);
    ASSERT(jsean_obj_set(&b, JSEAN_S("a"), &a) == NULL);

    jsean_set_str(&c, "a", 1, NULL);
    ASSERT(jsean_obj_set(&a, &c, &b) != NULL);
    ASSERT(jsean_get_type(&c) == JSEAN_TYPE_NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 32.0);

    jsean_free(&b);
    jsean_set_num(&b, 64.0);
    ASSERT(jsean_obj_set(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(jsean_obj_at(&a, JSEAN_S("a"))) == 64.0);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_object, del)
{
    jsean a, b;

    jsean_set_obj(&a);
    jsean_set_null(&b);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) != NULL);

    jsean_obj_del(&a, JSEAN_S("a"));
    ASSERT(jsean_obj_at(&a, JSEAN_S("a")) == NULL);

    jsean_free(&a);
    jsean_free(&b);
}

TEST(jsean_object, clear)
{
    jsean a, b;

    jsean_set_obj(&a);
    jsean_set_null(&b);

    ASSERT(jsean_obj_add(&a, JSEAN_S("a"), &b) != NULL);
    ASSERT(jsean_obj_add(&a, JSEAN_S("b"), &b) != NULL);
    ASSERT(jsean_obj_add(&a, JSEAN_S("c"), &b) != NULL);
    ASSERT(jsean_obj_len(&a) == 3);

    jsean_obj_clear(&a);
    ASSERT(jsean_obj_len(&a) == 0);

    jsean_free(&a);
    jsean_free(&b);
}
