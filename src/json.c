//
// Copyright (c) 2024, sonkajarvi
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

static void string_free(json_t *json)
{
    char *s;
    if ((s = json_string(json)))
        free(s);
}

static void array_free(json_t *json)
{
    struct json_array *array;
    if (!(array = json->data._array))
        return;

    const size_t length = json_array_length(json);
    for (size_t i = 0; i < length; i++) {
        json_free(json_array_at(json, i));
    }

    free(array);
}

static void object_free(json_t *json)
{
    struct json_object *object;
    if (!(object = json->data._object))
        return;

    struct json_object_node *buffer = json_object_buffer(object);
    for (size_t i = 0; i < object->capacity; i++) {
        if (buffer[i].key) {
            json_free(buffer[i].value);
            free(buffer[i].key);
        }
    }

    free(object);
}

enum json_type json_type(json_t *json)
{
    return json->type;
}

void json_free_underlying(json_t *json)
{
    if (json->type == JSON_TYPE_STRING)
        string_free(json);
    else if (json->type == JSON_TYPE_ARRAY)
        array_free(json);
    else if (json->type == JSON_TYPE_OBJECT)
        object_free(json);
}

void json_free(json_t *json)
{
    if (!json)
        return;

    json_free_underlying(json);
    free(json);
}

json_t *json_new_null(void)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    return json;
}

void json_set_null(json_t *json)
{
    if (!json)
        return;

    json_free_underlying(json);
    json->type = JSON_TYPE_NULL;
}

json_t *json_new_boolean(bool b)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    json_set_boolean(json, b);
    return json;
}

void json_set_boolean(json_t *json, bool b)
{
    if (!json)
        return;

    json_free_underlying(json);

    json->type = JSON_TYPE_BOOLEAN;
    json->data._boolean = b;
}

bool json_boolean(json_t *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_BOOLEAN) {
        return json->data._boolean;
    } else if (json->type == JSON_TYPE_NUMBER) {
        switch (json->type_ext) {
        case JSON_TYPE_EXT_INT:
        case JSON_TYPE_EXT_UINT:
            return (bool)json->data._signed;

        case JSON_TYPE_EXT_DOUBLE:
            return (bool)json->data._double;

        default:
            break;
        }
    }

    return 0;
}

json_t *json_new_signed(int64_t i)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    json_set_signed(json, i);
    return json;
}

void json_set_signed(json_t *json, int64_t i)
{
    if (!json)
        return;

    json_free_underlying(json);

    json->type = JSON_TYPE_NUMBER;
    json->type_ext = JSON_TYPE_EXT_INT;
    json->data._signed = i;
}

int64_t json_signed(json_t *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->type_ext) {
        case JSON_TYPE_EXT_INT:
            return json->data._signed;

        case JSON_TYPE_EXT_UINT:
            if (json->data._unsigned > INT64_MAX)
                return INT64_MAX;
            return json->data._unsigned;

        case JSON_TYPE_EXT_DOUBLE:
            return (int64_t)json->data._double;

        default:
            break;
        }
     } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (int64_t)json->data._boolean;
    }

    return 0;
}

json_t *json_new_unsigned(uint64_t u)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    json_set_unsigned(json, u);
    return json;
}

void json_set_unsigned(json_t *json, uint64_t u)
{
    if (!json)
        return;

    json_free_underlying(json);

    json->type = JSON_TYPE_NUMBER;
    json->type_ext = JSON_TYPE_EXT_UINT;
    json->data._unsigned = u;
}

uint64_t json_unsigned(json_t *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->type_ext) {
        case JSON_TYPE_EXT_UINT:
            return json->data._unsigned;

        case JSON_TYPE_EXT_INT:
            if (json->data._signed < 0)
                return 0;
            return json->data._unsigned;

        case JSON_TYPE_EXT_DOUBLE:
            return (uint64_t)json->data._double;

        default:
            break;
        }
    } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (uint64_t)json->data._boolean;
    }

    return 0;
}

json_t *json_new_double(double d)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    json_set_double(json, d);
    return json;
}

void json_set_double(json_t *json, double d)
{
    if (!json)
        return;

    json_free_underlying(json);

    json->type = JSON_TYPE_NUMBER;
    json->type_ext = JSON_TYPE_EXT_DOUBLE;
    json->data._double = d;
}

double json_double(json_t *json)
{
    if (!json)
        return 0.0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->type_ext) {
        case JSON_TYPE_EXT_DOUBLE:
            return json->data._double;

        case JSON_TYPE_EXT_INT:
            return (double)json->data._signed;

        case JSON_TYPE_EXT_UINT:
            return (double)json->data._unsigned;

        default:
            break;
        }
    } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (double)json->data._boolean;
    }

    return 0.0;
}

json_t *json_new_string(const char *s)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_NULL;
    json_set_string(json, s);
    return json;
}

// Internal
json_t *json_new_string_without_copy(char *s)
{
    json_t *json = malloc(sizeof(*json));
    if (!json)
        return NULL;

    json->type = JSON_TYPE_STRING;
    json->data._string = s;
    return json;
}

void json_set_string(json_t *json, const char *s)
{
    if (!json || !s)
        return;

    json_free_underlying(json);

    json->type = JSON_TYPE_STRING;
    json->data._string = strdup(s);
}

char *json_string(json_t *json)
{
    if (!json || json->type != JSON_TYPE_STRING)
        return NULL;

    return json->data._string;
}
