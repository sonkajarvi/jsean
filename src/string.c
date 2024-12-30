//
// Copyright (c) 2024, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

void string_free(struct string *str)
{
    if (!str)
        return;

    free(str->data);
    memset(str, 0, sizeof(*str));
}

bool string_append_char(struct string *str, char c)
{
    if (!str || !c)
        return false;

    char *tmp = str->data;
    if (str->length + 1 < str->capacity)
        goto skip_realloc;

    str->capacity = 2 * str->capacity ?: STRING_DEFAULT_CAPACITY;
    if ((tmp = realloc(tmp, str->capacity)) == NULL)
        return false;
    str->data = tmp;

skip_realloc:
    str->data[str->length++] = c;
    str->data[str->length] = '\0';
    return true;
}

bool string_append_chars(struct string *str, const char *s)
{
    return string_append_chars_n(str, s, strlen(s));
}

bool string_append_chars_n(struct string *str, const char *s, const size_t n)
{
    if (!str || !s || !s[0] || !n)
        return false;

    char *tmp = str->data;
    if (str->length + n + 1 < str->capacity)
        goto skip_realloc;

    while (str->capacity < str->length + n)
        str->capacity = 2 * str->capacity ?: STRING_DEFAULT_CAPACITY;

    if ((tmp = realloc(tmp, str->capacity)) == NULL)
        return false;
    str->data = tmp;

skip_realloc:
    memcpy(&str->data[str->length], s, n);
    str->length += n;
    str->data[str->length] = '\0';
    return true;
}

bool string_reserve(struct string *str, const size_t n)
{
    if (!str || n <= str->capacity)
        return false;

    char *tmp;
    if ((tmp = realloc(str->data, n)) == NULL)
        return false;

    str->data = tmp;
    str->capacity = n;
    return true;
}


const char *string_ref(struct string *str)
{
    return str ? str->data : NULL;
}

char *string_release(struct string *str)
{
    if (!str)
        return NULL;

    char *tmp = str->data;
    memset(str, 0, sizeof(*str));
    return tmp;
}
