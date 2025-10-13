//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

static const char *__type_strings[] = {
#define X(type_, str_) str_,
    __JSEAN_TYPE_LIST(X)
#undef X
};

static const char *__status_strings[] = {
#define X(status_, str_) str_,
    __JSEAN_STATUS_LIST(X)
#undef X
};

unsigned int get_internal_type(const jsean *json)
{
    if (!json || json->type >= __INTERNAL_TYPE_COUNT)
        return INTERNAL_TYPE_UNKNOWN;

    return json->type;
}

unsigned int jsean_get_type(const jsean *json)
{
    if (!json || json->type >= __JSEAN_TYPE_COUNT)
        return JSEAN_TYPE_UNKNOWN;

    return json->type;
}

const char *jsean_type_to_str(unsigned int type)
{
    if (type > __JSEAN_TYPE_COUNT)
        return NULL;

    return __type_strings[type];
}

const char *jsean_status_to_str(int status)
{
    if (status > __JSEAN_STATUS_COUNT)
        return NULL;

    return __status_strings[status];
}

void jsean_free(jsean *json)
{
    switch (jsean_get_type(json)) {
    case JSEAN_TYPE_OBJECT:
        obj_free(json);
        break;

    case JSEAN_TYPE_ARRAY:
        free_array(json);
        break;

    case JSEAN_TYPE_STRING:
        str_free(json);
        break;

    default:
        break;
    }
}

bool jsean_set_null(jsean *json)
{
    if (!json)
        return false;

    json->type = JSEAN_TYPE_NULL;

    return true;
}

bool jsean_set_bool(jsean *json, bool b)
{
    if (!json)
        return false;

    json->type = JSEAN_TYPE_BOOLEAN;
    json->b_val = b;

    return true;
}

bool jsean_get_bool(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_BOOLEAN)
        return json->b_val;

    return false;
}

bool jsean_set_num(jsean *json, double num)
{
    if (json) {
        if (isnan(num) || isinf(num))
            return false;

        json->type = JSEAN_TYPE_NUMBER;
        json->n_val = num;

        return true;
    }

    return false;
}

double jsean_get_num(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_NUMBER)
        return json->n_val;

    return 0.0;
}

bool jsean_set_str(jsean *json, char *str, size_t len, void (*free_fn)(void *))
{
    if (!json || !str || len > STRING_LENGTH_MAX)
        return false;

    if (!len) {
        len = strlen(str);

        if (len > STRING_LENGTH_MAX)
            return false;
    }

    json->s_val = str;
    json->s_len = len;
    json->s_free_fn = free_fn;
    json->type = JSEAN_TYPE_STRING;

    return true;
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

// JZ_TYPE_STRING
// JZ_TYPE_STRING_REF
// JZ_TYPE_STRING_EMPTY
 