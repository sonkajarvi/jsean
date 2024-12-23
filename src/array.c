//
// Copyright (c) 2024, sonkajarvi
//

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "internal.h"

#define ARRAY_INITIAL_CAPACITY 16

static struct json_array *to_array(json_t *json)
{
    if (!json || json->type != JSON_TYPE_ARRAY)
        return NULL;

    return json->data._array;
}

static struct json_array *array_resize(json_t *json,
    struct json_array *array, size_t new_cap)
{
    if (new_cap < ARRAY_INITIAL_CAPACITY)
        new_cap = ARRAY_INITIAL_CAPACITY;

    struct json_array *tmp;
    tmp = realloc(array, sizeof(struct json_array) + new_cap * sizeof(json_t *));
    if (!tmp)
        goto fail;

    tmp->capacity = new_cap;
    json->data._array = tmp;

fail:
    return tmp;
}

json_t *json_new_array(void)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->data._array = NULL;
    json->type = JSON_TYPE_ARRAY;
    struct json_array *array = to_array(json);
    if (!(array = array_resize(json, array, ARRAY_INITIAL_CAPACITY)))
        goto fail;

    array->length = 0;
    return json;

fail:
    free(json);
    return NULL;
}

size_t json_array_length(json_t *json)
{
    struct json_array *array;
    if (!(array = to_array(json)))
        return 0;

    return array->length;
}

json_t *json_array_at(json_t *json, size_t i)
{
    struct json_array *array;
    if (!(array = to_array(json)) || i >= array->length)
        return NULL;

    return json_array_buffer(array)[i];
}

json_t *json_array_push(json_t *json, json_t *other)
{
    struct json_array *array;
    if (!(array = to_array(json)) || !other)
        return NULL;

    // Allocate more memory when needed
    if (array->length >= array->capacity &&
        !(array = array_resize(json, array, 2 * array->capacity)))
            return NULL;

    json_array_buffer(array)[array->length] = other;
    array->length++;

    return other;
}

void json_array_pop(json_t *json)
{
    struct json_array *array;
    if (!(array = to_array(json)) || array->length == 0)
        return;

    json_free(json_array_buffer(array)[array->length - 1]);
    array->length--;

    if (array->length < array->capacity / 4)
        array_resize(json, array, array->capacity / 2);
}
