//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

static struct arr *arr_init(void)
{
    struct arr *arr;
    jsean *ptr;

    arr = malloc(sizeof(*arr));
    if (!arr)
        return NULL;

    arr->cap = ARRAY_DEFAULT_CAPACITY;
    arr->len = 0;

    ptr = malloc(sizeof(*ptr) * arr->cap);
    if (!ptr)
        return NULL;
    arr->ptr = ptr;

    return arr;
}

bool jsean_set_arr(jsean *json)
{
    if (!json)
        return false;

    json->type = JSEAN_TYPE_ARRAY;
    json->ao_ptr = NULL;

    return true;
}

size_t jsean_arr_len(const jsean *json)
{
    const struct arr *arr;

    if (jsean_get_type(json) != JSEAN_TYPE_ARRAY)
        return 0;

    arr = json->ao_ptr;
    if (!arr)
        return 0;

    return arr->len;
}

jsean *jsean_arr_at(const jsean *json, const size_t index)
{
    const struct arr *arr;

    if (jsean_get_type(json) != JSEAN_TYPE_ARRAY)
        return NULL;

    arr = json->ao_ptr;
    if (!arr)
        return NULL;

    if (index >= arr->len)
        return NULL;

    return &arr->ptr[index];
}

jsean *jsean_arr_set(jsean *json, const size_t index, const jsean *val)
{
    struct arr *arr;

    if (jsean_get_type(json) != JSEAN_TYPE_ARRAY)
        return NULL;

    if (jsean_get_type(val) == JSEAN_TYPE_UNKNOWN)
        return NULL;

    if (!json->ao_ptr && (json->ao_ptr = arr_init()) == NULL)
        return NULL;

    arr = json->ao_ptr;

    if (index > arr->len)
        return NULL;

    if (arr->len == index)
        return jsean_arr_add(json, index, val);

    memcpy(&arr->ptr[index], val, sizeof(*val));
    return &arr->ptr[index];
}

jsean *jsean_arr_add(jsean *json, const size_t index, const jsean *val)
{
    struct arr *arr;
    jsean *data;
    size_t cap, len;

    if (!json || json->type != JSEAN_TYPE_ARRAY || !val)
        return NULL;

    if (!json->ao_ptr && (json->ao_ptr = arr_init()) == NULL)
        return NULL;

    arr = json->ao_ptr;

    if (index > arr->len)
        return NULL;

    if (arr->len == arr->cap) {
        cap = next_capacity(arr->cap);

        data = realloc(arr->ptr, sizeof(*data) * cap);
        if (!data)
            return NULL;

        arr->ptr = data;
        arr->cap = cap;
    }

    if (index != arr->len) {
        len = sizeof(*val) * (arr->len - index);
        memmove(&arr->ptr[index + 1], &arr->ptr[index], len);
    }

    memcpy(&arr->ptr[index], val, sizeof(*val));
    arr->len++;
    return &arr->ptr[index];
}

void jsean_arr_del(jsean *json, const size_t index)
{
    struct arr *arr;
    size_t len;

    if (!json || json->type != JSEAN_TYPE_ARRAY || !json->ao_ptr)
        return;

    arr = json->ao_ptr;
    if (!arr->ptr || index >= arr->len)
        return;

    if (index != arr->len - 1) {
        len = sizeof(*arr->ptr) * (arr->len - index - 1);
        memmove(&arr->ptr[index], &arr->ptr[index + 1], len);
    }

    jsean_free(&arr->ptr[index]);
    arr->len--;
}

void jsean_arr_clear(jsean *json)
{
    struct arr *arr;
    jsean *tmp, *last;

    if (!json || json->type != JSEAN_TYPE_ARRAY || !json->ao_ptr)
        return;

    arr = json->ao_ptr;
    if (!arr->len)
        return;

    for (tmp = &arr->ptr[0], last = &arr->ptr[arr->len]; tmp != last; tmp++)
        jsean_free(tmp);
    arr->len = 0;
}

void arr_free(jsean *json)
{
    struct arr *arr = json->ao_ptr;
    jsean *tmp, *last;

    if (!arr || !arr->ptr)
        return;

    for (tmp = &arr->ptr[0], last = &arr->ptr[arr->len]; tmp != last; tmp++)
        jsean_free(tmp);

    free(arr->ptr);
    free(arr);
}
