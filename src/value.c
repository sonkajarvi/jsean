//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

static const char *__status_strings[] = {
#define X(status_, string_) string_,
    __JSEAN_STATUS_LIST(X)
#undef X
};

static void free_string(jsean *json);

unsigned int jsean_typeof(const jsean *json)
{
    if (json && json->type < __JSEAN_TYPE_COUNT)
        return json->type;

    return JSEAN_UNKNOWN;
}

void jsean_free(jsean *json)
{
    switch (jsean_typeof(json)) {
    case JSEAN_UNKNOWN:
        return;

    case JSEAN_TYPE_OBJECT:
        free_object(json);
        break;

    case JSEAN_TYPE_ARRAY:
        free_array(json);
        break;

    case JSEAN_TYPE_STRING:
        free_string(json);
        break;

    default:
        break;
    }
}

const char *jsean_status_to_string(int status)
{
    if (status > __JSEAN_STATUS_COUNT)
        return NULL;

    return __status_strings[status];
}

bool jsean_set_null(jsean *self)
{
    if (!self)
        return false;

    self->type = JSEAN_TYPE_NULL;

    return true;
}

bool jsean_set_bool(jsean *self, bool b)
{
    if (!self)
        return false;

    self->type = JSEAN_TYPE_BOOLEAN;
    self->boolean = b;

    return true;
}

bool jsean_get_bool(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_BOOLEAN)
        return json->boolean;

    return false;
}

void jsean_set_number(jsean *json, double num)
{
    if (json) {
        if (isnan(num) || isinf(num)) {
            json->type = JSEAN_TYPE_NULL;
            return;
        }

        json->type = JSEAN_TYPE_NUMBER;
        json->number = num;
    }
}

double jsean_get_number(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_NUMBER)
        return json->number;

    return 0.0;
}

void jsean_set_string(jsean *json, const char *str)
{
    if (json) {
        if (!str) {
            json->type = JSEAN_UNKNOWN;
            return;
        }

        json->type = JSEAN_TYPE_STRING;
        json->pointer = strdup(str);
    }
}

const char *jsean_get_string(jsean *json)
{
    if (json && json->type == JSEAN_TYPE_STRING)
        return json->pointer;

    return NULL;
}

void free_string(jsean *json)
{
    if (json)
        free(json->pointer);
}
