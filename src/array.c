//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/jsean.h>

static struct jsean_array_ *jsean_get_array(const jsean_t *json)
{
    if (!json || json->type != JSEAN_TYPE_ARRAY)
        return NULL;

    return json->data._array;
}

int jsean_set_array(jsean_t *json, size_t cap)
{
    struct jsean_array_ *arr;

    if (!json)
        return EINVAL;

    if ((arr = malloc(sizeof(*arr))) == NULL)
        return ENOMEM;

    cap = cap ?: JSEAN_ARRAY_DEFAULT_CAPACITY;
    if ((arr->data = malloc(sizeof(*arr->data) * cap)) == NULL)
        return ENOMEM;

    arr->cap = cap;
    arr->size = 0;

    json->data._array = arr;
    json->type = JSEAN_TYPE_ARRAY;

    return 0;
}

size_t jsean_array_size(const jsean_t *json)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL)
        return 0;

    return arr->size;
}

size_t jsean_array_capacity(const jsean_t *json)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL)
        return 0;

    return arr->cap;
}

int jsean_array_reserve(jsean_t *json, size_t cap)
{
    struct jsean_array_ *arr;
    jsean_t *tmp;

    if ((arr = jsean_get_array(json)) == NULL)
        return EINVAL;

    cap = cap ?: JSEAN_ARRAY_DEFAULT_CAPACITY;
    if (cap <= arr->cap)
        return EINVAL;

    if ((tmp = realloc(arr->data, sizeof(*arr->data) * cap)) == NULL)
        return ENOMEM;

    arr->data = tmp;
    arr->cap = cap;

    return 0;
}

void jsean_array_clear(jsean_t *json)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL)
        return;

    for (size_t i = 0; i < arr->size; i++)
        jsean_free(jsean_array_at(json, i));

    arr->size = 0;
}

jsean_t *jsean_array_at(const jsean_t *json, const size_t idx)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL || idx >= arr->size)
        return NULL;

    return &arr->data[idx];
}

int jsean_array_push(jsean_t *json, const jsean_t *val)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL)
        return EINVAL;

    if (jsean_type(val) == JSEAN_TYPE_UNKNOWN_)
        return EINVAL;

    // Allocate more memory when needed
    if (arr->size >= arr->cap) {
        if (jsean_array_reserve(json, 2 * arr->cap) != 0)
            return ENOMEM;

        arr = jsean_get_array(json);
    }

    jsean_move_(&arr->data[arr->size], val);
    arr->size++;

    return 0;
}

void jsean_array_pop(jsean_t *json)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL || arr->size == 0)
        return;

    jsean_free(&arr->data[arr->size - 1]);
    arr->size--;
}

int jsean_array_insert(jsean_t *json, const size_t idx, const jsean_t *val)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL || idx > arr->size)
        return EINVAL;

    if (jsean_type(val) == JSEAN_TYPE_UNKNOWN_)
        return EINVAL;

    if (idx == arr->size)
        return jsean_array_push(json, val);

    // Allocate more memory when needed
    if (arr->size >= arr->cap) {
        if (jsean_array_reserve(json, 2 * arr->cap) != 0)
            return ENOMEM;

        arr = jsean_get_array(json);
    }

    // Shift elements to the right
    memcpy(&arr->data[idx + 1], &arr->data[idx],
        (arr->size - idx) * sizeof(*arr->data));

    jsean_move_(&arr->data[idx], val);
    arr->size++;

    return 0;
}

void jsean_array_remove(jsean_t *json, const size_t idx)
{
    struct jsean_array_ *arr;

    if ((arr = jsean_get_array(json)) == NULL || idx >= arr->size)
        return;

    jsean_free(&arr->data[idx]);

    // Shift elements to the left
    memcpy(&arr->data[idx], &arr->data[idx + 1],
        (arr->size - idx - 1) * sizeof(*arr->data));

    arr->size--;
}
