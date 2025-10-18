//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_object, empty)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{}")) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_obj_len(&a) == 0);

    jsean_free(&a);
}

TEST(jsean_read_object, strings)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\": \"hello\", \"b\": \"world\"}")) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_obj_len(&a) == 2);
    ASSERT(jsean_get_type(jsean_obj_at(&a, JSEAN_S("a"))) == JSEAN_TYPE_STRING);

    jsean_free(&a);
}

TEST(jsean_read_object, whitespace)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{\n\t\"a\": 1,\n\t\"b\": 2,\n\t\"c\": 3\n}")) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_OBJECT);
    ASSERT(jsean_obj_len(&a) == 3);

    jsean_free(&a);
}

TEST(jsean_read_object, expected_comma)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":1 \"b\":2}")) == JSEAN_EXPECTED_COMMA);
}

TEST(jsean_read_object, expected_value)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":}")) == JSEAN_EXPECTED_VALUE);
}

TEST(jsean_read_object, expected_quotation_mark)
{
    jsean a;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":1,}")) == JSEAN_EXPECTED_QUOTATION_MARK);
}
