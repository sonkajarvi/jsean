//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

#define JSEAN_ARRAY_DEFAULT_CAPACITY 8

struct array {
    jsean *data;
    size_t len;
    size_t cap;
};

// Grows approximately by a factor of 1.6
static inline size_t next_capacity(const size_t n)
{
    if (n == 1)
        return 2;

    return n + (n >> 1) + (n >> 3);
}

static struct array *init_array(void)
{
    struct array *arr;
    jsean *data;

    arr = malloc(sizeof(*arr));
    if (!arr)
        return NULL;

    arr->len = 0;
    arr->cap = JSEAN_ARRAY_DEFAULT_CAPACITY;
    arr->data = NULL;

    data = malloc(sizeof(*data) * next_capacity(arr->cap));
    if (!data)
        return NULL;
    arr->data = data;

    return arr;
}

void jsean_set_array(jsean *json)
{
    if (json) {
        json->type = JSEAN_ARRAY;
        json->pointer = NULL;
    }
}

size_t jsean_array_length(const jsean *json)
{
    const struct array *arr;

    if (!json || json->type != JSEAN_ARRAY || !json->pointer)
        return 0;

    arr = json->pointer;
    return arr->len;
}

jsean *jsean_array_at(const jsean *json, const size_t index)
{
    const struct array *arr;

    if (!json || json->type != JSEAN_ARRAY || !json->pointer)
        return NULL;

    arr = json->pointer;
    if (index >= arr->len)
        return NULL;

    return &arr->data[index];
}

jsean *jsean_array_insert(jsean *json, const size_t index, const jsean *val)
{
    struct array *arr;
    jsean *data;
    size_t len;

    if (!json || json->type != JSEAN_ARRAY || !val)
        return NULL;

    if (!json->pointer && (json->pointer = init_array()) == NULL)
        return NULL;

    arr = json->pointer;
    if (index > arr->len)
        return NULL;

    if (arr->len == arr->cap) {
        data = realloc(arr->data, sizeof(*data) * next_capacity(arr->cap));
        if (!data)
            return NULL;
        arr->data = data;
    }

    if (index != arr->len) {
        len = sizeof(*val) * (arr->len - index);
        memmove(&arr->data[index + 1], &arr->data[index], len);
    }

    memcpy(&arr->data[index], val, sizeof(*val));
    arr->len++;
    return &arr->data[index];
}

void jsean_array_remove(jsean *json, const size_t index, jsean *out)
{
    struct array *arr;
    size_t len;

    if (!json || json->type != JSEAN_ARRAY || !json->pointer)
        return;

    arr = json->pointer;
    if (!arr->data || index >= arr->len)
        return;

    if (index != arr->len - 1) {
        len = sizeof(*arr->data) * (arr->len - index - 1);
        memmove(&arr->data[index], &arr->data[index + 1], len);
    }

    if (out)
        memcpy(out, &arr->data[index], sizeof(*out));
    else
        jsean_free(&arr->data[index]);

    arr->len--;
}

void jsean_array_clear(jsean *json)
{
    struct array *arr;
    jsean *tmp, *last;

    if (!json || json->type != JSEAN_ARRAY || !json->pointer)
        return;

    arr = json->pointer;
    if (!arr->len)
        return;

    for (tmp = &arr->data[0], last = &arr->data[arr->len]; tmp != last; tmp++)
        jsean_free(tmp);
    arr->len = 0;
}

void free_array(jsean *json)
{
    struct array *arr = json->pointer;
    jsean *tmp, *last;

    if (!arr || !arr->data)
        return;

    for (tmp = &arr->data[0], last = &arr->data[arr->len]; tmp != last; tmp++)
        jsean_free(tmp);

    free(arr->data);
    free(arr);
}
