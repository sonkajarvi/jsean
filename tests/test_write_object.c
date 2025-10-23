//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdlib.h>
#include <string.h>

#include "jsean.h"
#include "test.h"

TEST(jsean_write_object, empty)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{}") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_object, empty_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{}") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_object, string)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":\"hello\"}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{\"a\":\"hello\"}") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_object, string_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":\"hello\"}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{\n\t\"a\": \"hello\"\n}") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_object, strings)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":\"hello\",\"b\":\"world\"}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{\"a\":\"hello\",\"b\":\"world\"}") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_object, strings_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("{\"a\":\"hello\",\"b\":\"world\"}")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "{\n\t\"a\": \"hello\",\n\t\"b\": \"world\"\n}") == 0);

    jsean_free(&a);
    free(buf);
}
