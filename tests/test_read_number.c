//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_number_int)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.0);

    ASSERT(jsean_reads(&a, "-123") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == -123.0);

    ASSERT(jsean_reads(&a, "0") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);

    ASSERT(jsean_reads(&a, "-0") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);
}

TEST(jsean_read_number_frac)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123.456") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.456);

    ASSERT(jsean_reads(&a, "0.456") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.456);
}

TEST(jsean_read_number_exp)
{
    jsean a;

    ASSERT(jsean_reads(&a, "123.456e2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);

    ASSERT(jsean_reads(&a, "123.456E2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);

    ASSERT(jsean_reads(&a, "123.456e+2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 12345.6);

    ASSERT(jsean_reads(&a, "12345.6e-2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 123.456);

    ASSERT(jsean_reads(&a, "0e2") == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_NUMBER);
    ASSERT(jsean_get_num(&a) == 0.0);
}

TEST(jsean_read_expected_nonzero_digit)
{
    jsean a;

    ASSERT(jsean_reads(&a, "-") == JSEAN_EXPECTED_NONZERO_DIGIT);
}

TEST(jsean_read_expected_digit)
{
    jsean a;

    ASSERT(jsean_reads(&a, "0.") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e+") == JSEAN_EXPECTED_DIGIT);
    ASSERT(jsean_reads(&a, "0e-") == JSEAN_EXPECTED_DIGIT);
}
