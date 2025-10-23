//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean.h"

int jsean_set_bool(jsean *json, bool b)
{
    if (!json)
        return JSEAN_INVALID_ARGUMENTS;

    json->type = JSEAN_TYPE_BOOLEAN;
    json->b_val = b;

    return JSEAN_SUCCESS;
}

bool jsean_get_bool(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_BOOLEAN)
        return json->b_val;

    return false;
}
