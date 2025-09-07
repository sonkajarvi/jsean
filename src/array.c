//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/JSEAN.h"
#include "internal.h"

#define JSEAN_ARRAY_DEFAULT_CAPACITY 8

typedef struct {
    JSEAN *data;
    size_t len;
    size_t cap;
} ArrayType;

// Grows approximately by a factor of 1.6
static inline size_t NextCapacity(const size_t n)
{
    if (n == 1)
        return 2;

    return n + (n >> 1) + (n >> 3);
}

static ArrayType *InitArray(void)
{
    ArrayType *arr;
    JSEAN *data;

    arr = malloc(sizeof(*arr));
    if (!arr)
        return NULL;

    arr->len = 0;
    arr->cap = JSEAN_ARRAY_DEFAULT_CAPACITY;
    arr->data = NULL;

    data = malloc(sizeof(*data) * NextCapacity(arr->cap));
    if (!data)
        return NULL;
    arr->data = data;

    return arr;
}

void JSEAN_SetArray(JSEAN *json)
{
    if (json) {
        json->type = JSEAN_Array;
        json->pointer = NULL;
    }
}

size_t JSEAN_ArrayLength(const JSEAN *json)
{
    const ArrayType *arr;

    if (!json || json->type != JSEAN_Array || !json->pointer)
        return 0;

    arr = json->pointer;
    return arr->len;
}

JSEAN *JSEAN_ArrayAt(const JSEAN *json, const size_t index)
{
    const ArrayType *arr;

    if (!json || json->type != JSEAN_Array || !json->pointer)
        return NULL;

    arr = json->pointer;
    if (index >= arr->len)
        return NULL;

    return &arr->data[index];
}

JSEAN *JSEAN_ArrayInsert(JSEAN *json, const size_t index, const JSEAN *val)
{
    ArrayType *arr;
    JSEAN *data;
    size_t len;

    if (!json || json->type != JSEAN_Array || !val)
        return NULL;

    if (!json->pointer && (json->pointer = InitArray()) == NULL)
        return NULL;

    arr = json->pointer;
    if (index > arr->len)
        return NULL;

    if (arr->len == arr->cap) {
        data = realloc(arr->data, sizeof(*data) * NextCapacity(arr->cap));
        if (!data)
            return NULL;
        arr->data = data;
    }

    if (index != arr->len) {
        len = sizeof(*val) * (arr->len - index);
        memmove(&arr->data[index + 1], &arr->data[index], len);
    }

    memcpy(&arr->data[index], val, sizeof(*val));
    arr->len++;
    return &arr->data[index];
}

void JSEAN_ArrayRemove(JSEAN *json, const size_t index, JSEAN *out)
{
    ArrayType *arr;
    size_t len;

    if (!json || json->type != JSEAN_Array || !json->pointer)
        return;

    arr = json->pointer;
    if (!arr->data || index >= arr->len)
        return;

    if (index != arr->len - 1) {
        len = sizeof(*arr->data) * (arr->len - index - 1);
        memmove(&arr->data[index], &arr->data[index + 1], len);
    }

    if (out)
        memcpy(out, &arr->data[index], sizeof(*out));
    else
        JSEAN_Free(&arr->data[index]);

    arr->len--;
}

void JSEAN_ArrayClear(JSEAN *json)
{
    ArrayType *arr;
    JSEAN *tmp, *last;

    if (!json || json->type != JSEAN_Array || !json->pointer)
        return;

    arr = json->pointer;
    if (!arr->len)
        return;

    for (tmp = &arr->data[0], last = &arr->data[arr->len]; tmp != last; tmp++)
        JSEAN_Free(tmp);
    arr->len = 0;
}

void FreeArray(JSEAN *json)
{
    ArrayType *arr = json->pointer;
    JSEAN *tmp, *last;

    if (!arr || !arr->data)
        return;

    for (tmp = &arr->data[0], last = &arr->data[arr->len]; tmp != last; tmp++)
        JSEAN_Free(tmp);

    free(arr->data);
    free(arr);
}
