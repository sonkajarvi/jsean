#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/jsean.h>
#include "../test.h"

TEST(jsean_type)
{
    jsean_t a;

    jsean_set_null(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_boolean(&a, true);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_BOOLEAN);

    jsean_set_object(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_OBJECT);
    jsean_free(&a);

    jsean_set_array(&a, 0);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_ARRAY);
    jsean_free(&a);

    jsean_set_number(&a, 42);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NUMBER);

    jsean_set_string(&a, "hello, world");
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_STRING);
    jsean_free(&a);
}

TEST(jsean_move_)
{
    jsean_t a, b;

    jsean_set_number(&a, 2.0);
    jsean_set_boolean(&b, true);

    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NUMBER);
    ASSERT_EQ(jsean_get_number(&a), 2.0);
    ASSERT_EQ(jsean_type(&b), JSEAN_TYPE_BOOLEAN);
    ASSERT_EQ(jsean_get_boolean(&a), true);

    jsean_move_(&b, &a);

    ASSERT_EQ(jsean_type(&b), JSEAN_TYPE_NUMBER);
    ASSERT_EQ(jsean_get_number(&b), 2.0);
    // ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);
}

TEST(jsean_free)
{
    jsean_t a;

    jsean_set_null(&a);
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_boolean(&a, true);
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_object(&a);
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_array(&a, 0);
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_number(&a, 42);
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);

    jsean_set_string(&a, "hello, world");
    jsean_free(&a);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);
}

TEST(jsean_set_null)
{
    jsean_t a;

    ASSERT_EQ(jsean_set_null(NULL), EINVAL);

    ASSERT_EQ(jsean_set_null(&a), 0);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NULL);
}

TEST(jsean_set_and_get_boolean)
{
    jsean_t a;

    ASSERT_EQ(jsean_set_boolean(NULL, true), EINVAL);

    jsean_set_boolean(&a, true);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_BOOLEAN);
    ASSERT_EQ(jsean_get_boolean(&a), true);

    jsean_set_boolean(&a, false);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_BOOLEAN);
    ASSERT_EQ(jsean_get_boolean(&a), false);
}

TEST(jsean_set_and_get_double)
{
    jsean_t a;

    ASSERT_EQ(jsean_set_number(NULL, 2.0), EINVAL);

    jsean_set_number(&a, 2.0);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_NUMBER);
    ASSERT_EQ(jsean_get_number(&a), 2.0);
}

TEST(jsean_set_and_get_string)
{
    jsean_t a;

    char *s = malloc(13);
    strcpy(s, "hello, world");

    jsean_set_string(&a, s);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_STRING);
    ASSERT_EQ(strcmp(jsean_get_string(&a), s), 0);
    ASSERT_NE(jsean_get_string(&a), s);
    jsean_free(&a);

    jsean_move_string(&a, s);
    ASSERT_EQ(jsean_type(&a), JSEAN_TYPE_STRING);
    ASSERT_EQ(strcmp(jsean_get_string(&a), s), 0);
    ASSERT_EQ(jsean_get_string(&a), s);
    jsean_free(&a);
}
