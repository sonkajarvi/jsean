//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "test.h"

#define TEST_STRING(name, input, len, output)            \
    TEST(jsean_write_string, name)                       \
    {                                                    \
        jsean a;                                         \
        char *buf;                                       \
                                                         \
        jsean_set_str(&a, input, len, NULL);             \
        ASSERT(jsean_get_type(&a) == JSEAN_TYPE_STRING); \
                                                         \
        buf = jsean_write(&a, NULL, NULL);               \
        ASSERT(buf != NULL);                             \
        ASSERT(strcmp(buf, output) == 0);                \
                                                         \
        jsean_free(&a);                                  \
        free(buf);                                       \
    }

TEST_STRING(hello, "hello, world", 0, "\"hello, world\"");

TEST_STRING(zero, "\x00", 1, "\"\\u0000\"");
TEST_STRING(backspace, "\x08", 1, "\"\\b\"");
TEST_STRING(tab, "\x09", 1, "\"\\t\"");
TEST_STRING(line_feed, "\x0a", 1, "\"\\n\"");
TEST_STRING(form_feed, "\x0c", 1, "\"\\f\"");
TEST_STRING(carriage_return, "\x0d", 1, "\"\\r\"");
TEST_STRING(quotation_mark, "\x22", 1, "\"\\\"\"");
TEST_STRING(reverse_solidus, "\x5c", 1, "\"\\\\\"");
