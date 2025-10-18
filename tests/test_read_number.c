//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_number, ints)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.0);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "-123") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == -123.0);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "0") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "-0") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);
    jsean_free(&a);
}

TEST(jsean_read_number, fractions)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123.456") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.456);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "0.456") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.456);
    jsean_free(&a);
}

TEST(jsean_read_number, exponents)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123.456e2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "123.456E2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "123.456e+2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "12345.6e-2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.456);
    jsean_free(&a);

    ASSERT(jsean_reads(&a, "0e2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);
    jsean_free(&a);
}

TEST(jsean_read_number, expected_nonzero_digit)
{
    jsean a;

    ASSERT(jsean_reads(&a, "-") == JSEAN_EXPECTED_NONZERO_DIGIT);
}

TEST(jsean_read_number, expected_digit)
{
    jsean a;

    ASSERT(jsean_reads(&a, "0.") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e+") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e-") == JSEAN_EXPECTED_DIGIT);
}
