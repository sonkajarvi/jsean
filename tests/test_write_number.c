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

#define TEST_NUMBER(name, input, output)                 \
    TEST(jsean_write_number, name)                       \
    {                                                    \
        jsean a;                                         \
        char *buf;                                       \
                                                         \
        jsean_set_num(&a, input);                        \
        ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER); \
                                                         \
        buf = jsean_write(&a, NULL, NULL);               \
        ASSERT(buf != NULL);                             \
        ASSERT(strcmp(buf, output) == 0);                \
                                                         \
        jsean_free(&a);                                  \
        free(buf);                                       \
    }

TEST_NUMBER(zero, 0.0, "0");

TEST_NUMBER(non_zero, 1.0, "1");
TEST_NUMBER(non_zero2, 1.1, "1.1");
TEST_NUMBER(non_zero3, 1.000001, "1.000001");
TEST_NUMBER(non_zero4, 1.0000001, "1");

TEST_NUMBER(exp, 1e20, "100000000000000000000");
TEST_NUMBER(exp2, 1e100, "1e+100");
TEST_NUMBER(exp3, 1e-5, "0.00001");
TEST_NUMBER(exp4, 1e-10, "1e-10");
TEST_NUMBER(exp5, 1.1e20, "110000000000000000000");
TEST_NUMBER(exp6, 1.1e100, "1.1e+100");
TEST_NUMBER(exp7, 1.1e-5, "0.000011");
TEST_NUMBER(exp8, 1.1e-10, "1.1e-10");
