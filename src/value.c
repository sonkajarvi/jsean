//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/JSEAN.h"
#include "internal.h"

static void FreeString(JSEAN *json);

JSEAN_Type JSEAN_TypeOf(const JSEAN *json)
{
    if (json && json->type < JSEAN_TypeCount)
        return json->type;

    return JSEAN_Unknown;
}

void JSEAN_Free(JSEAN *json)
{
    switch (JSEAN_TypeOf(json)) {
    case JSEAN_Unknown:
        return;

    case JSEAN_Array:
        FreeArray(json);
        break;

    case JSEAN_String:
        FreeString(json);
        break;

    default:
        break;
    }
}

void JSEAN_SetNull(JSEAN *json)
{
    if (json)
        json->type = JSEAN_Null;
}

void JSEAN_SetBoolean(JSEAN *json, bool b)
{
    if (json) {
        json->type = JSEAN_Boolean;
        json->boolean = b;
    }
}

bool JSEAN_GetBoolean(const JSEAN *json)
{
    if (json && json->type == JSEAN_Boolean)
        return json->boolean;

    return false;
}

void JSEAN_SetNumber(JSEAN *json, double num)
{
    if (json) {
        if (isnan(num) || isinf(num)) {
            json->type = JSEAN_Null;
            return;
        }

        json->type = JSEAN_Number;
        json->number = num;
    }
}

double JSEAN_GetNumber(const JSEAN *json)
{
    if (json && json->type == JSEAN_Number)
        return json->number;

    return 0.0;
}

void JSEAN_SetString(JSEAN *json, const char *str)
{
    if (json) {
        if (!str) {
            json->type = JSEAN_Unknown;
            return;
        }

        json->type = JSEAN_String;
        json->pointer = strdup(str);
    }
}

const char *JSEAN_GetString(JSEAN *json)
{
    if (json && json->type == JSEAN_String)
        return json->pointer;

    return NULL;
}

void FreeString(JSEAN *json)
{
    if (json)
        free(json->pointer);
}
