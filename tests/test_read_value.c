//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_invalid_arguments)
{
    jsean a;

    ASSERT(jsean_read(NULL, "null", 4) == JSEAN_INVALID_ARGUMENTS);
    ASSERT(jsean_read(&a, NULL, 4) == JSEAN_INVALID_ARGUMENTS);
    ASSERT(jsean_read(&a, "null", 0) == JSEAN_INVALID_ARGUMENTS);
}

TEST(jsean_read_false)
{
    jsean a;

    ASSERT(jsean_reads(&a, "false") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_BOOLEAN);
    ASSERT(jsean_get_boolean(&a) == false);
}

TEST(jsean_read_null)
{
    jsean a;

    ASSERT(jsean_reads(&a, "null") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_NULL);
}

TEST(jsean_read_true)
{
    jsean a;

    ASSERT(jsean_reads(&a, "true") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_BOOLEAN);
    ASSERT(jsean_get_boolean(&a) == true);
}

TEST(jsean_read_expected_false)
{
    jsean a;

    ASSERT(jsean_reads(&a, "friday") == JSEAN_EXPECTED_FALSE);
}

TEST(jsean_read_expected_null)
{
    jsean a;

    ASSERT(jsean_reads(&a, "nothing") == JSEAN_EXPECTED_NULL);
}

TEST(jsean_read_expected_true)
{
    jsean a;

    ASSERT(jsean_reads(&a, "tuesday") == JSEAN_EXPECTED_TRUE);
}

TEST(jsean_read_unexpected_character_after_value)
{
    jsean a;

    ASSERT(jsean_reads(&a, "true false") == JSEAN_UNEXPECTED_CHARACTER);
}
