//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <string.h>

#include "jsean/jsean.h"
#include "test.h"

#define TEST_STRING(name, input, output)                   \
    TEST(name)                                             \
    {                                                      \
        jsean a;                                           \
        ASSERT(jsean_reads(&a, input) == JSEAN_SUCCESS);   \
        ASSERT(jsean_typeof(&a) == JSEAN_STRING);          \
        ASSERT(strcmp(jsean_get_string(&a), output) == 0); \
        jsean_free(&a); \
    }

#define TEST_ERROR(name, input, output)           \
    TEST(name)                                    \
    {                                             \
        jsean a;                                  \
        ASSERT(jsean_reads(&a, input) == output); \
    }

TEST_STRING(jsean_read_string, "\"hello, world\"", "hello, world");

TEST_STRING(jsean_read_string_quotation_mark, "\"\\\"\"", "\"");
TEST_STRING(jsean_read_string_reverse_solidus, "\"\\\\\"", "\\");
TEST_STRING(jsean_read_string_solidus, "\"\\/\"", "/");
TEST_STRING(jsean_read_string_backspace, "\"\\b\"", "\b");
TEST_STRING(jsean_read_string_form_feed, "\"\\f\"", "\f");
TEST_STRING(jsean_read_string_line_feed, "\"\\n\"", "\n");
TEST_STRING(jsean_read_string_carriage_return, "\"\\r\"", "\r");
TEST_STRING(jsean_read_string_tab, "\"\\t\"", "\t");

TEST_STRING(jsean_read_string_unicode, "\"\\u0024\"", "$");
TEST_STRING(jsean_read_string_unicode2, "\"\\u20ac\"", "€");
TEST_STRING(jsean_read_string_surrogate, "\"\\ud801\\udc37\"", "𐐷");
TEST_STRING(jsean_read_string_surrogate2, "\"\\ud834\\udd1e\"", "𝄞");
TEST_STRING(jsean_read_string_surrogate3, "\"\\ud852\\udf62\"", "𤭢");

TEST_ERROR(jsean_read_string_expected_quotation_mark, "\"", JSEAN_EXPECTED_QUOTATION_MARK);
TEST_ERROR(jsean_read_string_invalid_escape, "\"\\x\"", JSEAN_INVALID_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_unicode, "\"\\u\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_unicode2, "\"\\u0\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_unicode3, "\"\\u00\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_unicode4, "\"\\u000\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_surrogate, "\"\\ud852\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
TEST_ERROR(jsean_read_string_invalid_surrogate2, "\"\\ud852\\u0061\"", JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE);
