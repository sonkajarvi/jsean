//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

#define OBJECT_DEFAULT_CAPACITY 16
#define OBJECT_LOAD_FACTOR 0.6

#define DEAD ((void *)(size_t)-1)

struct pair {
    char *key;
    jsean value;
};

struct object {
    struct pair *data;
    size_t cap;
    size_t used;
    size_t dead;
};

// Hash a string with djb2:
//
//     http://www.cse.yorku.ca/~oz/hash.html
//
static size_t hash_string(const char *str)
{
    size_t hash = 638130537;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static inline float load_factor(struct object *obj)
{
    return (float)(obj->used + obj->dead) / obj->cap;
}

static struct object *init_object(void)
{
    struct object *obj;
    struct pair *data;

    obj = malloc(sizeof(*obj));
    if (!obj)
        return NULL;

    obj->cap = OBJECT_DEFAULT_CAPACITY;
    obj->used = 0;
    obj->dead = 0;

    data = malloc(sizeof(*data) * obj->cap);
    if (!data)
        return NULL;

    memset(data, 0, sizeof(*data) * obj->cap);
    obj->data = data;

    return obj;
}

static void rehash_object(struct object *obj, size_t old_cap)
{
    for (size_t i = 0; obj->dead; i++) {
        if (obj->data[i].key == DEAD) {
            obj->data[i].key = NULL;
            obj->dead--;
        }
    }

    printf("CAPACITY %zu\n", obj->cap);

    for (size_t i = 0, j; i < old_cap; i++) {
        if (obj->data[i].key == NULL)
            continue;

        j = hash_string(obj->data[i].key) % obj->cap;
        while (obj->data[j].key != NULL) {
            j++;
            if (j >= obj->cap)
                j = 0;
        }

        memcpy(&obj->data[j], &obj->data[i], sizeof(obj->data[j]));
        obj->data[i].key = NULL;

        // printf("rehash %zu -> %zu", i, j);

        // if (j >= obj->cap)
        //     printf(" ERROR\n");
        // else
        //     printf("\n");
    }
}

void jsean_object_print(jsean *json)
{
    struct object *obj;

    if (jsean_typeof(json) != JSEAN_TYPE_OBJECT)
        return;

    obj = json->pointer;
    if (!obj || !obj->data)
        return;

    for (size_t i = 0; i < obj->cap; i++) {
        if (obj->data[i].key == NULL)
            printf("%3zu : -\n", i);
        else if (obj->data[i].key == DEAD)
            printf("%3zu : dead\n", i);
        else
            printf("%3zu : %s %f\n", i, obj->data[i].key, obj->data[i].value.number);
    }
}

void jsean_set_object(jsean *json)
{
    if (json) {
        json->type = JSEAN_TYPE_OBJECT;
        json->pointer = NULL;
    }
}

size_t jsean_object_count(const jsean *json)
{
    const struct object *obj;

    if (!json || json->type != JSEAN_TYPE_OBJECT || !json->pointer)
        return 0;

    obj = json->pointer;
    return obj->used;
}

jsean *jsean_object_get(const jsean *json, const char *key)
{
    struct object *obj;

    if (!json || json->type != JSEAN_TYPE_OBJECT || !key)
        return NULL;

    obj = json->pointer;
    if (!obj || !obj->used || !obj->data)
        return NULL;

    for (size_t j = obj->cap, i = hash_string(key) % j; j; i++, j--) {
        if (i >= obj->cap)
            i = 0;

        if (obj->data[i].key == NULL)
            return NULL;
        if (obj->data[i].key == DEAD)
            continue;
        if (strcmp(key, obj->data[i].key) == 0)
            return &obj->data[i].value;
    }

    return NULL;
}

jsean *jsean_object_insert(jsean *json, const char *key, jsean *val)
{
    struct object *obj;
    struct pair *data;
    size_t index, old_cap;

    if (!json || json->type != JSEAN_TYPE_OBJECT || !key || !val)
        return NULL;

    if (!json->pointer && (json->pointer = init_object()) == NULL)
        return NULL;

    obj = json->pointer;
    if (load_factor(obj) > OBJECT_LOAD_FACTOR) {
        old_cap = obj->cap;
        data = realloc(obj->data, sizeof(*data) * next_capacity(old_cap));
        if (!data)
            return NULL;

        obj->cap = next_capacity(old_cap);
        memset(&data[old_cap], 0, sizeof(*data) * (obj->cap - old_cap));

        obj->data = data;
        rehash_object(obj, old_cap);
    }

    for (index = hash_string(key) % obj->cap;; index++) {
        if (index >= obj->cap)
            index = 0;

        if (obj->data[index].key == NULL)
            break;
        if (obj->data[index].key == DEAD)
            continue;
        if (strcmp(key, obj->data[index].key) == 0)
            return NULL;
    }

    obj->data[index].key = strdup(key);
    memcpy(&obj->data[index].value, val, sizeof(*val));
    obj->used++;

    return &obj->data[index].value;
}

jsean *jsean_object_replace(jsean *json, const char *key, jsean *val)
{
    jsean *tmp;

    if (!json || json->type != JSEAN_TYPE_OBJECT || !key || !val)
        return NULL;

    if (!json->pointer && (json->pointer = init_object()) == NULL)
        return NULL;

    tmp = jsean_object_get(json, key);
    if (!tmp)
        return jsean_object_insert(json, key, val);

    jsean_free(tmp);
    memcpy(tmp, val, sizeof(*val));

    return tmp;
}

void jsean_object_remove(jsean *json, const char *key)
{
    struct object *obj;

    if (!json || json->type != JSEAN_TYPE_OBJECT || !key)
        return;

    obj = json->pointer;
    if (!obj || !obj->data)
        return;

    if (!obj->used)
        return;

    for (size_t j = obj->cap, i = hash_string(key) % j; j; i++, j--) {
        if (i >= obj->cap)
            i = 0;

        if (obj->data[i].key == NULL)
            break;
        if (obj->data[i].key == DEAD)
            continue;
        if (strcmp(key, obj->data[i].key) == 0) {
            free(obj->data[i].key);
            obj->data[i].key = DEAD;
            jsean_free(&obj->data[i].value);

            obj->used--;
            obj->dead++;
        }
    }
}

void jsean_object_clear(jsean *json)
{
    struct object *obj;

    if (!json || json->type != JSEAN_TYPE_OBJECT)
        return;

    obj = json->pointer;
    if (!obj || !obj->data)
        return;

    for (size_t i = 0; i < obj->cap && obj->used; i++) {
        if (obj->data[i].key == NULL || obj->data[i].key == DEAD) {
            continue;
        } else {
            free(obj->data[i].key);
            obj->data[i].key = NULL;
            jsean_free(&obj->data[i].value);

            obj->used--;
        }
    }
}

void free_object(jsean *json)
{
    struct object *obj = json->pointer;

    if (!obj || !obj->data)
        return;

    jsean_object_clear(json);
    free(obj->data);
    free(obj);
}
