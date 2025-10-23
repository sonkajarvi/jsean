//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean.h"

bool jsean_set_null(jsean *json)
{
    if (!json)
        return false;

    json->type = JSEAN_TYPE_NULL;

    return true;
}
