//
// Copyright (c) 2024, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/json.h>

static void array_free(struct json *json)
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

static void object_free(struct json *json)
{
    struct json_object *object;
    if (!(object = json->data._object))
        return;

    struct json_object_entry *buffer = json_object_buffer(object);
    for (size_t i = 0; i < object->capacity; i++) {
        if (buffer[i].key) {
            json_free(&buffer[i].value);
            free(buffer[i].key);
        }
    }

    free(object);
}

void json_move(void *lhs, void *rhs)
{
    if (!lhs || !rhs)
        return;

    memcpy(lhs, rhs, sizeof(struct json));
}

enum json_type json_type(struct json *json)
{
    return json->type;
}

void json_free(struct json *json)
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

struct json json_new_null(void)
{
    struct json json;
    json.type = JSON_TYPE_NULL;
    return json;
}

int json_init_null(struct json *json)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_NULL) {
        json_free(json);
        json->type = JSON_TYPE_NULL;
    }

    return 0;
}

int json_init_boolean(struct json *json, bool b)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_BOOLEAN) {
        json_free(json);
        json->type = JSON_TYPE_BOOLEAN;
    }

    json->data._boolean = b;
    return 0;
}

bool json_boolean(struct json *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_BOOLEAN) {
        return json->data._boolean;
    } else if (json->type == JSON_TYPE_NUMBER) {
        switch (json->number_type) {
        case JSON_NUMBER_SIGNED:
        case JSON_NUMBER_UNSIGNED:
            return (bool)json->data._signed;

        case JSON_NUMBER_DOUBLE:
            return (bool)json->data._double;

        default:
            break;
        }
    }

    return 0;
}

int json_set_signed(struct json *json, int64_t i)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_NUMBER) {
        json_free(json);
        json->type = JSON_TYPE_NUMBER;
    }

    json->number_type = JSON_NUMBER_SIGNED;
    json->data._signed = i;
    return 0;
}

int64_t json_signed(struct json *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->number_type) {
        case JSON_NUMBER_SIGNED:
            return json->data._signed;

        case JSON_NUMBER_UNSIGNED:
            if (json->data._unsigned > INT64_MAX)
                return INT64_MAX;
            return json->data._unsigned;

        case JSON_NUMBER_DOUBLE:
            return (int64_t)json->data._double;

        default:
            break;
        }
     } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (int64_t)json->data._boolean;
    }

    return 0;
}

int json_set_unsigned(struct json *json, uint64_t u)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_NUMBER) {
        json_free(json);
        json->type = JSON_TYPE_NUMBER;
    }

    json->number_type = JSON_NUMBER_UNSIGNED;
    json->data._unsigned = u;
    return 0;
}

uint64_t json_unsigned(struct json *json)
{
    if (!json)
        return 0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->number_type) {
        case JSON_NUMBER_UNSIGNED:
            return json->data._unsigned;

        case JSON_NUMBER_SIGNED:
            if (json->data._signed < 0)
                return 0;
            return json->data._unsigned;

        case JSON_NUMBER_DOUBLE:
            return (uint64_t)json->data._double;

        default:
            break;
        }
    } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (uint64_t)json->data._boolean;
    }

    return 0;
}

int json_set_double(struct json *json, double d)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_NUMBER) {
        json_free(json);
        json->type = JSON_TYPE_NUMBER;
    }

    json->number_type = JSON_NUMBER_DOUBLE;
    json->data._double = d;
    return 0;
}

double json_double(struct json *json)
{
    if (!json)
        return 0.0;

    if (json->type == JSON_TYPE_NUMBER) {
        switch (json->number_type) {
        case JSON_NUMBER_DOUBLE:
            return json->data._double;

        case JSON_NUMBER_SIGNED:
            return (double)json->data._signed;

        case JSON_NUMBER_UNSIGNED:
            return (double)json->data._unsigned;

        default:
            break;
        }
    } else if (json->type == JSON_TYPE_BOOLEAN) {
        return (double)json->data._boolean;
    }

    return 0.0;
}

// Internal
struct json json_new_string_without_copy(char *s)
{
    struct json json;
    json.type = JSON_TYPE_STRING;
    json.data._string = s;
    return json;
}

int json_set_string(struct json *json, const char *s)
{
    if (!json || !s)
        return EFAULT;

    json_free(json);
    json->type = JSON_TYPE_STRING;
    json->data._string = strdup(s);
    return 0;
}

char *json_string(struct json *json)
{
    if (!json || json->type != JSON_TYPE_STRING)
        return NULL;

    return json->data._string;
}
