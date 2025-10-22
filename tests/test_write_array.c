//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "test.h"

TEST(jsean_write_array, empty)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[]") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_array, empty_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[]") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_array, string)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[\"hello\"]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[\"hello\"]") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_array, string_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[\"hello\"]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[\n\t\"hello\"\n]") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_array, strings)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[\"hello\",\"world\"]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[\"hello\",\"world\"]") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_array, strings_space)
{
    jsean a;
    char *buf;

    ASSERT(jsean_read(&a, JSEAN_S("[\"hello\",\"world\"]")) == JSEAN_SUCCESS);

    buf = jsean_write(&a, NULL, "\t");
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "[\n\t\"hello\",\n\t\"world\"\n]") == 0);

    jsean_free(&a);
    free(buf);
}
