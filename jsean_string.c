//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean.h"
#include "jsean_internal.h"

int jsean_set_str(jsean *json, char *str, size_t len, void (*free_fn)(void *))
{
    if (!json || !str || len > STRING_LENGTH_MAX)
        return JSEAN_INVALID_ARGUMENTS;

    if (!len) {
        len = strlen(str);

        if (len > STRING_LENGTH_MAX)
            return JSEAN_INVALID_ARGUMENTS;
    }

    json->s_val = str;
    json->s_len = len;
    json->s_free_fn = free_fn;
    json->type = JSEAN_TYPE_STRING;

    return JSEAN_SUCCESS;
}

const char *jsean_get_str(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_STRING)
        return json->s_val;

    return NULL;
}

size_t jsean_str_len(const jsean *json)
{
    if (!json || !json->s_val || json->type != JSEAN_TYPE_STRING)
        return 0;

    return json->s_len;
}

bool str_cmp(const jsean *json, const jsean *other)
{
    if (jsean_get_type(json) != JSEAN_TYPE_STRING)
        return false;

    if (jsean_get_type(other) != JSEAN_TYPE_STRING)
        return false;

    if (jsean_str_len(json) != jsean_str_len(other))
        return false;

    return memcmp(json->s_val, other->s_val, jsean_str_len(json)) == 0;
}

// Hashes the string with djb2, http://www.cse.yorku.ca/~oz/hash.html
size_t str_hash(const jsean *json)
{
    size_t hash;

    if (!json || json->type != JSEAN_TYPE_STRING)
        return STRING_HASH_UNDEFINED;

    hash = 638130537;
    for (size_t i = 0; i < jsean_str_len(json); i++)
        hash = ((hash << 5) + hash) + json->s_val[i];

    if (hash == STRING_HASH_UNDEFINED)
        hash++;

    return hash;
}

void str_free(jsean *json)
{
    if (json && json->s_val && json->s_free_fn)
        json->s_free_fn(json->s_val);
}
