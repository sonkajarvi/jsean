//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_array, empty)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[]") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) == 0);

    jsean_free(&a);
}

TEST(jsean_read_array, strings)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[\"hello\", \"world\"]") == JSEAN_SUCCESS);

    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) == 2);
    ASSERT(jsean_get_type(jsean_arr_at(&a, 0)) == JSEAN_TYPE_STRING);

    jsean_free(&a);
}

TEST(jsean_read_array, whitespace)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[\n\t123,\n\t456,\n\t789\n]") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) == 3);

    jsean_free(&a);
}

TEST(jsean_read_array, expected_comma)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[true false]") == JSEAN_EXPECTED_COMMA);
}

TEST(jsean_read_array, expected_value)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[true, ]") == JSEAN_EXPECTED_VALUE);
}
