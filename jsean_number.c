//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <math.h>

#include "jsean.h"

bool jsean_set_num(jsean *json, double num)
{
    if (json) {
        if (isnan(num) || isinf(num))
            return false;

        json->type = JSEAN_TYPE_NUMBER;
        json->n_val = num;

        return true;
    }

    return false;
}

double jsean_get_num(const jsean *json)
{
    if (json && json->type == JSEAN_TYPE_NUMBER)
        return json->n_val;

    return 0.0;
}
