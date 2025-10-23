//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean.h"
#include "test.h"

TEST(jsean_value, get_type)
{
    jsean a;

    ASSERT(jsean_get_type(NULL) == JSEAN_TYPE_UNKNOWN);

    jsean_set_null(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NULL);
    jsean_free(&a);

    jsean_set_bool(&a, true);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_BOOLEAN);
    jsean_free(&a);

    jsean_set_obj(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_OBJECT);
    jsean_free(&a);

    jsean_set_arr(&a);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    jsean_free(&a);

    jsean_set_num(&a, 0);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    jsean_free(&a);

    jsean_set_str(&a, "", 0, NULL);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_STRING);
    jsean_free(&a);
}

// null

TEST(jsean_null, set_null)
{
    jsean a;

    ASSERT(jsean_set_null(NULL) == false);
    ASSERT(jsean_set_null(&a) == true);

    jsean_free(&a);
}

// boolean

TEST(jsean_bool, set_bool)
{
    jsean a;

    ASSERT(jsean_set_bool(NULL, true) == false);
    ASSERT(jsean_set_bool(&a, true) == true);

    jsean_free(&a);
}

TEST(jsean_bool, get_bool)
{
    jsean a;

    ASSERT(jsean_get_bool(NULL) == false);

    jsean_set_bool(&a, true);
    ASSERT(jsean_get_bool(&a) == true);

    jsean_free(&a);
}

// number

TEST(jsean_number, set_num)
{
    jsean a;

    ASSERT(jsean_set_num(NULL, 2.0) == false);
    ASSERT(jsean_set_num(&a, NAN) == false);
    ASSERT(jsean_set_num(&a, INFINITY) == false);

    ASSERT(jsean_set_num(&a, 2.0) == true);

    jsean_free(&a);
}

TEST(jsean_number, get_num)
{
    jsean a;

    ASSERT(jsean_get_num(NULL) == 0.0);

    jsean_set_num(&a, 2.0);
    ASSERT(jsean_get_num(&a) == 2.0);

    jsean_free(&a);
}

// string

TEST(jsean_string, set_str)
{
    jsean a;

    ASSERT(jsean_set_str(NULL, NULL, 0, NULL) == false);
    ASSERT(jsean_set_str(&a, NULL, 0, NULL) == false);

    ASSERT(jsean_set_str(&a, "abc", 0, NULL) == true);
    jsean_free(&a);

    ASSERT(jsean_set_str(&a, strdup("def"), 3, free) == true);
    jsean_free(&a);
}

TEST(jsean_string, get_str)
{
    jsean a;

    ASSERT(jsean_get_str(NULL) == NULL);

    jsean_set_str(&a, "abc", 0, NULL);
    ASSERT(strcmp(jsean_get_str(&a), "abc") == 0);

    jsean_free(&a);
}

TEST(jsean_string, len)
{
    jsean a;

    ASSERT(jsean_str_len(NULL) == 0);

    jsean_set_str(&a, "abc", 0, NULL);
    jsean_free(&a);

    jsean_set_str(&a, "abc", 1, NULL);
    ASSERT(jsean_str_len(&a) == 1);
    jsean_free(&a);
}
