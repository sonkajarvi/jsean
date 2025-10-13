//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_get_type)
{
    jsean a;

    ASSERT(jsean_get_type(NULL) == JSEAN_TYPE_UNKNOWN);

    a.type = -1;
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_UNKNOWN);

    jsean_set_null(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NULL);

    jsean_set_bool(&a, true);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_BOOLEAN);

    jsean_set_obj(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_OBJECT);
    jsean_free(&a);

    jsean_set_array(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    jsean_free(&a);

    jsean_set_num(&a, 3.14);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);

    jsean_set_str(&a, "", 0, NULL);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_STRING);
    jsean_free(&a);
}

TEST(jsean_null)
{
    jsean a;

    ASSERT(jsean_set_null(NULL) == false);
    ASSERT(jsean_set_null(&a) == true);

    jsean_free(&a);
}

TEST(jsean_bool)
{
    jsean a;

    ASSERT(!jsean_set_bool(NULL, true));
    ASSERT(jsean_set_bool(&a, true));

    ASSERT(jsean_get_bool(NULL) == false);
    ASSERT(jsean_get_bool(&a) == true);

    jsean_free(&a);
}

TEST(jsean_num)
{
    jsean a;

    ASSERT(jsean_get_num(NULL) == 0.0);

    ASSERT(jsean_set_num(&a, 2.0) == true);
    ASSERT(jsean_get_num(&a) == 2.0);

    ASSERT(jsean_set_num(&a, NAN) == false);
    ASSERT(jsean_set_num(&a, INFINITY) == false);
}

TEST(jsean_str)
{
    jsean a;

    ASSERT(jsean_set_str(&a, NULL, 0, NULL) == false);
    ASSERT(jsean_set_str(&a, "hello, world", 0, NULL) == true);

    ASSERT(jsean_get_str(NULL) == NULL);
    ASSERT(strcmp(jsean_get_str(&a), "hello, world") == 0);

    jsean_free(&a);
}
