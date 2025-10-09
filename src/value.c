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

void jsean_set_null(jsean *json)
{
    if (json)
        json->type = JSEAN_TYPE_NULL;
}

void jsean_set_boolean(jsean *json, bool b)
{
    if (json) {
        json->type = JSEAN_TYPE_BOOLEAN;
        json->boolean = b;
    }
}

bool jsean_get_boolean(const jsean *json)
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
