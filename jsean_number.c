//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <math.h>

#include "jsean.h"

int jsean_set_num(jsean *json, double num)
{
    if (json) {
        if (isnan(num) || isinf(num))
            return JSEAN_INVALID_ARGUMENTS;

        json->type = JSEAN_TYPE_NUMBER;
        json->n_val = num;

        return JSEAN_SUCCESS;
    }

    return JSEAN_INVALID_ARGUMENTS;
}

double jsean_get_num(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_NUMBER)
        return json->n_val;

    return 0.0;
}
