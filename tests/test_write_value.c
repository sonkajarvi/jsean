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

#define LEN 64

TEST(jsean_write_false)
{
    jsean a;
    char *buf;

    jsean_set_bool(&a, false);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_BOOLEAN);
    ASSERT(jsean_get_bool(&a) == false);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "false") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_null)
{
    jsean a;
    char *buf;

    jsean_set_null(&a);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_NULL);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "null") == 0);

    jsean_free(&a);
    free(buf);
}

TEST(jsean_write_true)
{
    jsean a;
    char *buf;

    jsean_set_bool(&a, true);
    ASSERT(jsean_typeof(&a) == JSEAN_TYPE_BOOLEAN);
    ASSERT(jsean_get_bool(&a) == true);

    buf = jsean_write(&a, NULL, NULL);
    ASSERT(buf != NULL);
    ASSERT(strcmp(buf, "true") == 0);

    jsean_free(&a);
    free(buf);
}
