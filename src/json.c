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
        json_free(JSON_array_at(json, i));

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
            json_free(&buffer[i].value);
            free(buffer[i].key);
        }
    }

    free(object);
}

void JSON_move(void *lhs, const void *rhs)
{
    if (!lhs || !rhs)
        return;

    memcpy(lhs, rhs, sizeof(JSON));
}

enum JSON_type JSON_type(const JSON *json)
{
    if (json == NULL || json->type >= JSON_TYPE_UNKNOWN)
        return JSON_TYPE_UNKNOWN;

    return json->type;
}

void json_free(JSON *json)
{
    if (!json)
        return;

    if (json->type == JSON_TYPE_STRING)
        free(json_string(json));
    else if (json->type == JSON_TYPE_ARRAY)
        array_free(json);
    else if (json->type == JSON_TYPE_OBJECT)
        object_free(json);

    json->type = JSON_TYPE_UNKNOWN;
}

JSON json_new_null(void)
{
    JSON json;
    json.type = JSON_TYPE_NULL;
    return json;
}

int json_init_null(JSON *json)
{
    if (!json)
        return EFAULT;

    json->type = JSON_TYPE_NULL;

    return 0;
}

int json_init_boolean(JSON *json, bool b)
{
    if (!json)
        return EFAULT;

    json->data._boolean = b;
    json->type = JSON_TYPE_BOOLEAN;

    return 0;
}

bool json_boolean(JSON *json)
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

int json_set_number(JSON *json, double d)
{
    if (!json)
        return EFAULT;

    json->data._double = d;
    json->type = JSON_TYPE_NUMBER;

    return 0;
}

double json_number(JSON *json)
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

// Internal
JSON json_new_string_without_copy(char *s)
{
    JSON json;
    json.type = JSON_TYPE_STRING;
    json.data._string = s;
    return json;
}

int json_set_string(JSON *json, const char *s)
{
    if (!json || !s)
        return EFAULT;

    json_free(json);
    json->type = JSON_TYPE_STRING;
    json->data._string = strdup(s);

    return 0;
}

char *json_string(JSON *json)
{
    if (!json || json->type != JSON_TYPE_STRING)
        return NULL;

    return json->data._string;
}
