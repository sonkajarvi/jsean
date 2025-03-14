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

static void jsean_array_free(jsean_t *json)
{
    struct jsean_array *arr;

    if ((arr = json->data._array) == NULL)
        return;

    for (size_t i = 0; i < arr->size; i++)
        jsean_free(jsean_array_at(json, i));

    free(arr->data);
    free(arr);
}

static void jsean_object_free(jsean_t *json)
{
    struct jsean_object *obj;

    if (!(obj = json->data._object))
        return;

    for (size_t i = 0; i < obj->cap; i++) {
        if (obj->data[i].key) {
            jsean_free(&obj->data[i].value);
            free(obj->data[i].key);
        }
    }

    free(obj->data);
    free(obj);
}

enum jsean_type jsean_type(const jsean_t *json)
{
    if (json == NULL || json->type >= JSEAN_TYPE_UNKNOWN)
        return JSEAN_TYPE_UNKNOWN;

    return json->type;
}

void jsean_move(void *lhs, const void *rhs)
{
    if (!lhs || jsean_type(rhs) == JSEAN_TYPE_UNKNOWN)
        return;

    memcpy(lhs, rhs, sizeof(jsean_t));
}

void jsean_free(jsean_t *json)
{
    if (!json)
        return;

    if (json->type == JSEAN_TYPE_STRING)
        free(jsean_get_string(json));
    else if (json->type == JSEAN_TYPE_ARRAY)
        jsean_array_free(json);
    else if (json->type == JSEAN_TYPE_OBJECT)
        jsean_object_free(json);

    jsean_set_null(json);
}

int jsean_set_null(jsean_t *json)
{
    if (!json)
        return EINVAL;

    json->type = JSEAN_TYPE_NULL;

    return 0;
}

int jsean_set_boolean(jsean_t *json, const bool b)
{
    if (!json)
        return EINVAL;

    json->data._boolean = b;
    json->type = JSEAN_TYPE_BOOLEAN;

    return 0;
}

bool jsean_get_boolean(const jsean_t *json)
{
    if (!json)
        return 0;

    if (json->type == JSEAN_TYPE_BOOLEAN) {
        return json->data._boolean;
    } else if (json->type == JSEAN_TYPE_NUMBER) {
        return (bool)json->data._double;
    }

    return 0;
}

int jsean_set_number(jsean_t *json, const double d)
{
    if (!json)
        return EINVAL;

    json->data._double = d;
    json->type = JSEAN_TYPE_NUMBER;

    return 0;
}

double jsean_get_number(const jsean_t *json)
{
    if (!json)
        return 0.0;

    if (json->type == JSEAN_TYPE_NUMBER) {
        return json->data._double;
    } else if (json->type == JSEAN_TYPE_BOOLEAN) {
        return (double)json->data._boolean;
    }

    return 0.0;
}

int jsean_set_string(jsean_t *json, const char *s)
{
    if (!json || !s)
        return EINVAL;

    json->type = JSEAN_TYPE_STRING;
    json->data._string = strdup(s);

    return 0;
}

int jsean_move_string(jsean_t *json, char *s)
{
    if (!json || !s)
        return EINVAL;

    json->type = JSEAN_TYPE_STRING;
    json->data._string = s;

    return 0;
}

char *jsean_get_string(jsean_t *json)
{
    if (!json || json->type != JSEAN_TYPE_STRING)
        return NULL;

    return json->data._string;
}
