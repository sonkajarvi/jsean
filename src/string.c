//
// Copyright (c) 2024, sonkajarvi
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

#define STRING_INITIAL_CAPACITY 16

void string_free(string *str)
{
    if (!str)
        return;

    free(str->data);
    memset(str, 0, sizeof(*str));
}

bool string_append_char(string *str, char c)
{
    if (!str || !c)
        return false;

    char *tmp = str->data;
    if (str->length + 1 < str->capacity)
        goto skip_realloc;

    str->capacity = 2 * str->capacity ?: STRING_INITIAL_CAPACITY;
    if ((tmp = realloc(tmp, str->capacity)) == NULL)
        return false;
    str->data = tmp;

skip_realloc:
    str->data[str->length++] = c;
    str->data[str->length] = '\0';
    return true;
}

bool string_append_chars(string *str, const char *s)
{
    return string_append_chars_n(str, s, strlen(s));
}

bool string_append_chars_n(string *str, const char *s, const size_t n)
{
    if (!str || !s || !s[0] || !n)
        return false;

    char *tmp = str->data;
    if (str->length + n + 1 < str->capacity)
        goto skip_realloc;

    while (str->capacity < str->length + n)
        str->capacity = 2 * str->capacity ?: STRING_INITIAL_CAPACITY;

    if ((tmp = realloc(tmp, str->capacity)) == NULL)
        return false;
    str->data = tmp;

skip_realloc:
    memcpy(&str->data[str->length], s, n);
    str->length += n;
    str->data[str->length] = '\0';
    return true;
}
