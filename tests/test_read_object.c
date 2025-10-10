//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_object_empty)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{}") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_object_count(&a) == 0);

    jsean_free(&a);
}

TEST(jsean_read_object_strings)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{\"a\": \"hello\", \"b\": \"world\"}") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_object_count(&a) == 2);
    ASSERT(jsean_typeof(jsean_object_get(&a, "a")) == JSEAN_TYPE_STRING);

    jsean_free(&a);
}

TEST(jsean_read_object_whitespace)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{\n\t\"a\": 1,\n\t\"b\": 2,\n\t\"c\": 3\n}") == JSEAN_SUCCESS);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_object_count(&a) == 3);

    jsean_free(&a);
}

TEST(jsean_read_object_expected_comma)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{\"a\":1 \"b\":2}") == JSEAN_EXPECTED_COMMA);
}

TEST(jsean_read_object_expected_value)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{\"a\":}") == JSEAN_EXPECTED_VALUE);
}

TEST(jsean_read_object_expected_quotation_mark)
{
    jsean a;

    ASSERT(jsean_reads(&a, "{\"a\":1,}") == JSEAN_EXPECTED_QUOTATION_MARK);
}
