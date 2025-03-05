//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/JSON.h>

static struct JSON_array *JSON_array(const JSON *json)
{
    if (!json || json->type != JSON_TYPE_ARRAY)
        return NULL;

    return json->data._array;
}

int JSON_set_array(JSON *json, size_t cap)
{
    struct JSON_array *arr;

    if (!json)
        return EINVAL;

    if ((arr = malloc(sizeof(*arr))) == NULL)
        return ENOMEM;

    cap = cap ?: JSON_ARRAY_DEFAULT_CAPACITY;
    if ((arr->data = malloc(sizeof(*arr->data) * cap)) == NULL)
        return ENOMEM;

    arr->cap = cap;
    arr->size = 0;

    json->data._array = arr;
    json->type = JSON_TYPE_ARRAY;

    return 0;
}

size_t JSON_array_size(const JSON *json)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL)
        return 0;

    return arr->size;
}

size_t JSON_array_capacity(const JSON *json)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL)
        return 0;

    return arr->cap;
}

int JSON_array_reserve(JSON *json, size_t cap)
{
    struct JSON_array *arr;
    JSON *tmp;

    if ((arr = JSON_array(json)) == NULL)
        return EINVAL;

    cap = cap ?: JSON_ARRAY_DEFAULT_CAPACITY;
    if (cap <= arr->cap)
        return EINVAL;

    if ((tmp = realloc(arr->data, sizeof(*arr->data) * cap)) == NULL)
        return ENOMEM;

    arr->data = tmp;
    arr->cap = cap;

    return 0;
}

void JSON_array_clear(JSON *json)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL)
        return;

    for (size_t i = 0; i < arr->size; i++)
        JSON_free(JSON_array_at(json, i));

    arr->size = 0;
}

JSON *JSON_array_at(const JSON *json, const size_t idx)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL || idx >= arr->size)
        return NULL;

    return &arr->data[idx];
}

int JSON_array_push(JSON *json, const JSON *val)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL)
        return EINVAL;

    if (JSON_type(val) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    // Allocate more memory when needed
    if (arr->size >= arr->cap) {
        if (JSON_array_reserve(json, 2 * arr->cap) != 0)
            return ENOMEM;

        arr = JSON_array(json);
    }

    JSON_move(&arr->data[arr->size], val);
    arr->size++;

    return 0;
}

void JSON_array_pop(JSON *json)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL || arr->size == 0)
        return;

    JSON_free(&arr->data[arr->size - 1]);
    arr->size--;
}

int JSON_array_insert(JSON *json, const size_t idx, const JSON *val)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL || idx > arr->size)
        return EINVAL;

    if (JSON_type(val) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    if (idx == arr->size)
        return JSON_array_push(json, val);

    // Allocate more memory when needed
    if (arr->size >= arr->cap) {
        if (JSON_array_reserve(json, 2 * arr->cap) != 0)
            return ENOMEM;

        arr = JSON_array(json);
    }

    // Shift elements to the right
    memcpy(&arr->data[idx + 1], &arr->data[idx],
        (arr->size - idx) * sizeof(*arr->data));

    JSON_move(&arr->data[idx], val);
    arr->size++;

    return 0;
}

void JSON_array_remove(JSON *json, const size_t idx)
{
    struct JSON_array *arr;

    if ((arr = JSON_array(json)) == NULL || idx >= arr->size)
        return;

    JSON_free(&arr->data[idx]);

    // Shift elements to the left
    memcpy(&arr->data[idx], &arr->data[idx + 1],
        (arr->size - idx - 1) * sizeof(*arr->data));

    arr->size--;
}
