//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <string.h>

#include "jsean/jsean.h"
#include "test.h"

#define TEST_STRING(name, input, output)                                   \
    TEST(jsean_read_string, name)                                          \
    {                                                                      \
        jsean a;                                                           \
        ASSERT(jsean_read(&a, JSEAN_S(input)) == JSEAN_SUCCESS);           \
        ASSERT(jsean_get_type(&a) == JSEAN_TYPE_STRING);                   \
        ASSERT(memcmp(jsean_get_str(&a), output, jsean_str_len(&a)) == 0); \
        jsean_free(&a);                                                    \
    }

#define TEST_ERROR(name, input, output)                   \
    TEST(jsean_read_string, name)                         \
    {                                                     \
        jsean a;                                          \
        ASSERT(jsean_read(&a, JSEAN_S(input)) == output); \
    }

TEST_STRING(hello, "\"hello, world\"", "hello, world");

TEST_STRING(quotation_mark, "\"\\\"\"", "\"");
TEST_STRING(reverse_solidus, "\"\\\\\"", "\\");
TEST_STRING(solidus, "\"\\/\"", "/");
TEST_STRING(backspace, "\"\\b\"", "\b");
TEST_STRING(form_feed, "\"\\f\"", "\f");
TEST_STRING(line_feed, "\"\\n\"", "\n");
TEST_STRING(carriage_return, "\"\\r\"", "\r");
TEST_STRING(tab, "\"\\t\"", "\t");

TEST_STRING(unicode, "\"\\u0024\"", "$");
TEST_STRING(unicode2, "\"\\u20ac\"", "€");
TEST_STRING(surrogate, "\"\\ud801\\udc37\"", "𐐷");
TEST_STRING(surrogate2, "\"\\ud834\\udd1e\"", "𝄞");
TEST_STRING(surrogate3, "\"\\ud852\\udf62\"", "𤭢");

TEST_ERROR(expected_quotation_mark, "\"", JSEAN_EXPECTED_QUOTATION_MARK);
TEST_ERROR(invalid_escape, "\"\\x\"", JSEAN_INVALID_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_unicode, "\"\\u\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_unicode2, "\"\\u0\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_unicode3, "\"\\u00\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_unicode4, "\"\\u000\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_surrogate, "\"\\ud852\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(invalid_surrogate2, "\"\\ud852\\u0061\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
