//
// Copyright (c) 2024, sonkajarvi
//

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>

typedef struct string
{
    char *data;
    size_t capacity;
    size_t length;
} string;

void string_free(string *str);

bool string_append_char(string *str, char c);

bool string_append_chars(string *str, const char *s);
bool string_append_chars_n(string *str, const char *s, const size_t n);

bool string_reserve(string *str, const size_t n);

#endif // STRING_H
