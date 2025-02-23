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

static struct json_array *to_array(JSON *json)
{
    if (!json || json->type != JSON_TYPE_ARRAY)
        return NULL;

    return json->data._array;
}

static struct json_array *array_resize(JSON *json,
    struct json_array *array, size_t new_cap)
{
    struct json_array *tmp;
    tmp = realloc(array, sizeof(struct json_array) + new_cap * sizeof(JSON));
    if (!tmp)
        goto fail;

    tmp->capacity = new_cap;
    json->data._array = tmp;

fail:
    return tmp;
}

int json_init_array(JSON *json, const size_t n)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_ARRAY) {
        json_free(json);
        json->data._array = NULL;
        json->type = JSON_TYPE_ARRAY;

        struct json_array *array = to_array(json);
        if (!(array = array_resize(json, array, n ?: JSON_ARRAY_DEFAULT_CAPACITY)))
            return ENOMEM;

        array->length = 0;
    }

    return 0;
}

size_t json_array_length(JSON *json)
{
    struct json_array *array;
    if (!(array = to_array(json)))
        return 0;

    return array->length;
}

size_t json_array_capacity(JSON *json)
{
    struct json_array *array;
    if (!(array = to_array(json)))
        return 0;

    return array->capacity;
}

int json_array_reserve(JSON *json, const size_t n)
{
    struct json_array *array;
    if (!(array = to_array(json)))
        return EFAULT;

    if (n == 0)
        return EINVAL;

    if (n <= array->capacity)
        return 0;

    if (!(array = array_resize(json, array, n)))
        return ENOMEM;

    return 0;
}

void json_array_clear(JSON *json)
{
    struct json_array *array;
    if (!(array = to_array(json)))
        return;

    for (size_t i = 0; i < array->length; i++)
        json_free(&json_array_buffer(array)[i]);

    array->length = 0;
}

JSON *json_array_at(JSON *json, size_t i)
{
    struct json_array *array;
    if (!(array = to_array(json)) || i >= array->length)
        return NULL;

    return &json_array_buffer(array)[i];
}

int json_array_push(JSON *json, JSON *value)
{
    struct json_array *array;
    if (!json || !value)
        return EFAULT;

    if (!(array = to_array(json)) || json_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    // Allocate more memory when needed
    if (array->length >= array->capacity &&
        !(array = array_resize(json, array, 2 * array->capacity)))
            return ENOMEM;

    json_move(&json_array_buffer(array)[array->length], value);
    array->length++;

    return 0;
}

void json_array_pop(JSON *json)
{
    struct json_array *array;
    if (!(array = to_array(json)) || array->length == 0)
        return;

    json_free(&json_array_buffer(array)[array->length - 1]);
    array->length--;

    if (array->length < array->capacity / 4)
        array_resize(json, array, array->capacity / 2);
}

int json_array_insert(JSON *json, size_t i, JSON *value)
{
    struct json_array *array;
    if (!json || !value)
        return EFAULT;

    if (!(array = to_array(json)) || i > array->length ||
        json_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    if (i == array->length)
        return json_array_push(json, value);

    // Allocate more memory when needed
    if (array->length >= array->capacity &&
        !(array = array_resize(json, array, 2 * array->capacity)))
            return ENOMEM;

    // Shift elements to the right
    memcpy(&json_array_buffer(array)[i + 1], &json_array_buffer(array)[i],
        (array->length - i) * sizeof(JSON));

    json_move(&json_array_buffer(array)[i], value);
    array->length++;

    return 0;
}

void json_array_remove(JSON *json, size_t i)
{
    struct json_array *array;
    if (!(array = to_array(json)) || i >= array->length)
        return;

    json_free(&json_array_buffer(array)[i]);

    // Shift elements to the left
    memcpy(&json_array_buffer(array)[i], &json_array_buffer(array)[i + 1],
        (array->length - i - 1) * sizeof(JSON));

    array->length--;
}
