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

static struct json_array *to_array(const JSON *json)
{
    if (!json || json->type != JSON_TYPE_ARRAY)
        return NULL;

    return json->data._array;
}

int JSON_set_array(JSON *json, size_t capacity)
{
    struct json_array *array;

    if (!json)
        return EINVAL;

    if ((array = malloc(sizeof(*array))) == NULL)
        return ENOMEM;

    capacity = capacity ?: JSON_ARRAY_DEFAULT_CAPACITY;
    if ((array->data = malloc(sizeof(*array->data) * capacity)) == NULL)
        return ENOMEM;

    array->capacity = capacity;
    array->length = 0;

    json->data._array = array;
    json->type = JSON_TYPE_ARRAY;

    return 0;
}

size_t JSON_array_size(const JSON *json)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL)
        return 0;

    return array->length;
}

size_t JSON_array_capacity(const JSON *json)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL)
        return 0;

    return array->capacity;
}

int JSON_array_reserve(JSON *json, const size_t capacity)
{
    struct json_array *array;
    JSON *tmp;

    if (capacity == 0)
        return EINVAL;

    if ((array = to_array(json)) == NULL)
        return EINVAL;

    if (capacity <= array->capacity)
        return 0;

    if ((tmp = realloc(array->data, sizeof(*array->data) * capacity)) == NULL)
        return ENOMEM;

    array->data = tmp;
    array->capacity = capacity;

    return 0;
}

void JSON_array_clear(JSON *json)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL)
        return;

    for (size_t i = 0; i < array->length; i++)
        json_free(JSON_array_at(json, i));

    array->length = 0;
}

JSON *JSON_array_at(const JSON *json, const size_t index)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL || index >= array->length)
        return NULL;

    return &array->data[index];
}

int JSON_array_push(JSON *json, const JSON *value)
{
    struct json_array *array;
    int retval;

    if ((array = to_array(json)) == NULL)
        return EINVAL;

    if (JSON_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    // Allocate more memory when needed
    if (array->length >= array->capacity) {
        if ((retval = JSON_array_reserve(json, array->capacity * 2)) != 0)
            return retval;
    }

    array = to_array(json);
    JSON_move(&array->data[array->length], value);
    array->length++;

    return 0;
}

void JSON_array_pop(JSON *json)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL || array->length == 0)
        return;

    json_free(&array->data[array->length - 1]);
    array->length--;
}

int JSON_array_insert(JSON *json, const size_t index, const JSON *value)
{
    struct json_array *array;
    int retval;

    if ((array = to_array(json)) == NULL || index > array->length)
        return EINVAL;

    if (JSON_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    if (index == array->length)
        return JSON_array_push(json, value);

    // Allocate more memory when needed
    if (array->length >= array->capacity) {
        if ((retval = JSON_array_reserve(json, array->capacity * 2)) != 0)
            return ENOMEM;
    }

    // Shift elements to the right
    memcpy(&array->data[index + 1], &array->data[index],
        (array->length - index) * sizeof(*array->data));

    JSON_move(&array->data[index], value);
    array->length++;

    return 0;
}

void JSON_array_remove(JSON *json, const size_t index)
{
    struct json_array *array;

    if ((array = to_array(json)) == NULL || index >= array->length)
        return;

    json_free(&array->data[index]);

    // Shift elements to the left
    memcpy(&array->data[index], &array->data[index + 1],
        (array->length - index - 1) * sizeof(*array->data));

    array->length--;
}
