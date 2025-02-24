//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/JSON.h>

static void array_free(JSON *json)
{
    struct json_array *array;

    if ((array = json->data._array) == NULL)
        return;

    for (size_t i = 0; i < array->length; i++)
        JSON_free(JSON_array_at(json, i));

    free(array->data);
    free(array);
}

static void object_free(JSON *json)
{
    struct JSON_object *object;
    if (!(object = json->data._object))
        return;

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    for (size_t i = 0; i < object->capacity; i++) {
        if (buffer[i].key) {
            JSON_free(&buffer[i].value);
            free(buffer[i].key);
        }
    }

    free(object);
}

enum JSON_type JSON_type(const JSON *json)
{
    if (json == NULL || json->type >= JSON_TYPE_UNKNOWN)
        return JSON_TYPE_UNKNOWN;

    return json->type;
}

void JSON_move(void *lhs, const void *rhs)
{
    if (!lhs || JSON_type(rhs) == JSON_TYPE_UNKNOWN)
        return;

    memcpy(lhs, rhs, sizeof(JSON));
}

void JSON_free(JSON *json)
{
    if (!json)
        return;

    if (json->type == JSON_TYPE_STRING)
        free(JSON_string(json));
    else if (json->type == JSON_TYPE_ARRAY)
        array_free(json);
    else if (json->type == JSON_TYPE_OBJECT)
        object_free(json);

    JSON_set_null(json);
}

int JSON_set_null(JSON *json)
{
    if (!json)
        return EINVAL;

    json->type = JSON_TYPE_NULL;

    return 0;
}

int JSON_set_boolean(JSON *json, const bool b)
{
    if (!json)
        return EINVAL;

    json->data._boolean = b;
    json->type = JSON_TYPE_BOOLEAN;

    return 0;
}

bool JSON_boolean(const JSON *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_BOOLEAN) {
        return json->data._boolean;
    } else if (json->type == JSON_TYPE_NUMBER) {
        return (bool)json->data._double;
    }

    return 0;
}

int JSON_set_number(JSON *json, const double d)
{
    if (!json)
        return EINVAL;

    json->data._double = d;
    json->type = JSON_TYPE_NUMBER;

    return 0;
}

double JSON_number(const JSON *json)
{
    if (!json)
        return 0.0;

    if (json->type == JSON_TYPE_NUMBER) {
        return json->data._double;
    } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (double)json->data._boolean;
    }

    return 0.0;
}

int JSON_set_string(JSON *json, const char *s)
{
    if (!json || !s)
        return EINVAL;

    json->type = JSON_TYPE_STRING;
    json->data._string = strdup(s);

    return 0;
}

int JSON_move_string(JSON *json, char *s)
{
    if (!json || !s)
        return EINVAL;

    json->type = JSON_TYPE_STRING;
    json->data._string = s;

    return 0;
}

char *JSON_string(JSON *json)
{
    if (!json || json->type != JSON_TYPE_STRING)
        return NULL;

    return json->data._string;
}
