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

TEST(jsean_typeof)
{
    jsean a;

    ASSERT(jsean_typeof(NULL) == JSEAN_UNKNOWN);

    a.type = -1;
    ASSERT(jsean_typeof(&a) == JSEAN_UNKNOWN);

    jsean_set_null(&a);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_NULL);

    jsean_set_boolean(&a, true);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_BOOLEAN);

    jsean_set_object(&a);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_OBJECT);
    jsean_free(&a);

    jsean_set_array(&a);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_ARRAY);
    jsean_free(&a);

    jsean_set_number(&a, 3.14);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_NUMBER);

    jsean_set_string(&a, "");
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_STRING);
    jsean_free(&a);
}

TEST(jsean_number)
{
    jsean a;

    ASSERT(jsean_get_number(NULL) == 0.0);

    jsean_set_number(&a, 2.0);
    ASSERT(jsean_get_number(&a) == 2.0);

    jsean_set_number(&a, NAN);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_NULL);

    jsean_set_number(&a, INFINITY);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_NULL);
}

TEST(jsean_string)
{
    jsean a;

    ASSERT(jsean_get_string(NULL) == NULL);

    jsean_set_string(&a, "hello, world");
    ASSERT(strcmp(jsean_get_string(&a), "hello, world") == 0);
    jsean_free(&a);

    jsean_set_string(&a, NULL);
    ASSERT(jsean_typeof(&a) == JSEAN_UNKNOWN);
}
