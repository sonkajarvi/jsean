//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdio.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_read_stream, file_64kb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/64KB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}

TEST(jsean_read_stream, file_128kb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/128KB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}

TEST(jsean_read_stream, file_256kb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/256KB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}

TEST(jsean_read_stream, file_512kb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/512KB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}

TEST(jsean_read_stream, file_1mb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/1MB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}

TEST(jsean_read_stream, file_5mb)
{
    FILE *fp;
    jsean a;

    fp = fopen(SAMPLES_DIR "/5MB.json", "r");
    ASSERT(fp != NULL);

    ASSERT(jsean_read_stream(&a, fp) == JSEAN_SUCCESS);
    ASSERT(jsean_get_type(&a) == JSEAN_TYPE_ARRAY);
    ASSERT(jsean_arr_len(&a) > 0);

    fclose(fp);
    jsean_free(&a);
}
