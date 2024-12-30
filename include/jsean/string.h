//
// Copyright (c) 2024, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#ifndef JSEAN_STRING_H
#define JSEAN_STRING_H

#include <stdbool.h>
#include <stddef.h>

#ifndef STRING_DEFAULT_CAPACITY
#define STRING_DEFAULT_CAPACITY 16
#endif

struct string
{
    char *data;
    size_t capacity;
    size_t length;
};

void string_free(struct string *str);

bool string_append_char(struct string *str, char c);

bool string_append_chars(struct string *str, const char *s);
bool string_append_chars_n(struct string *str, const char *s, const size_t n);

bool string_reserve(struct string *str, const size_t n);

const char *string_ref(struct string *str);
char *string_release(struct string *str);

#endif // JSEAN_STRING_H
