//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_array_empty)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[]") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_array_length(&a) == 0);

    jsean_free(&a);
}

TEST(jsean_read_array_strings)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[\"hello\", \"world\"]") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_array_length(&a) == 2);
    ASSERT(jsean_typeof(jsean_array_at(&a, 0)) == JSEAN_TYPE_STRING);

    jsean_free(&a);
}

TEST(jsean_read_array_whitespace)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[\n\t123,\n\t456,\n\t789\n]") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_array_length(&a) == 3);

    jsean_free(&a);
}

TEST(jsean_read_array_expected_comma)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[true false]") == JSEAN_EXPECTED_COMMA);
}

TEST(jsean_read_array_unexpected_character)
{
    jsean a;

    ASSERT(jsean_reads(&a, "[true, ]") == JSEAN_UNEXPECTED_CHARACTER);
}
