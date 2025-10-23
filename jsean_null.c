//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include "jsean.h"

int jsean_set_null(jsean *json)
{
    if (!json)
        return JSEAN_INVALID_ARGUMENTS;

    json->type = JSEAN_TYPE_NULL;

    return JSEAN_SUCCESS;
}
