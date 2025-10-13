//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

static inline float get_load_factor(struct obj *obj)
{
    return (float)(obj->len + obj->dead) / obj->cap;
}

static struct obj *obj_init(void)
{
    struct obj *obj;
    struct obj_pair *ptr, *end;

    obj = malloc(sizeof(*obj));
    if (!obj)
        return NULL;

    obj->cap = OBJECT_DEFAULT_CAPACITY;
    obj->len = 0;
    obj->dead = 0;

    ptr = malloc(sizeof(*ptr) * obj->cap);
    if (!ptr)
        return NULL;

    obj->pairs = ptr;

    for (end = ptr + obj->cap; ptr != end; ptr++)
        ptr->key.type = INTERNAL_TYPE_EMPTY;

    return obj;
}

static void obj_rehash(struct obj *obj, size_t old_cap)
{
    struct obj_pair *ptr, *end;
    size_t index;

    for (ptr = obj->pairs; obj->dead > 0; ptr++) {
        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_DEAD) {
            ptr->key.type = INTERNAL_TYPE_EMPTY;
            obj->dead--;
        }
    }

    for (ptr = obj->pairs, end = ptr + old_cap; ptr != end; ptr++) {
        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_EMPTY)
            continue;

        index = str_hash(&ptr->key) % obj->cap;
        while (get_internal_type(&ptr->key) != INTERNAL_TYPE_EMPTY) {
            index++;

            if (index >= obj->cap)
                index = 0;
        }

        memcpy(&obj->pairs[index], ptr, sizeof(*ptr));
        ptr->key.type = INTERNAL_TYPE_EMPTY;
    }
}

bool jsean_set_obj(jsean *json)
{
    if (!json)
        return false;

    json->ao_ptr = NULL;
    json->type = JSEAN_TYPE_OBJECT;

    return true;
}

size_t jsean_obj_len(const jsean *json)
{
    const struct obj *obj;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return 0;

    obj = json->ao_ptr;

    return obj ? obj->len : 0;
}

jsean *jsean_obj_at(const jsean *json, const jsean *key)
{
    struct obj *obj;
    struct obj_pair *ptr, *end;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return NULL;

    if (jsean_get_type(key) != JSEAN_TYPE_STRING)
        return NULL;

    obj = json->ao_ptr;
    if (!obj)
        return NULL;

    ptr = obj->pairs + (str_hash(key) % obj->cap);
    end = ptr + obj->cap;

    for (;; ptr++) {
        if (ptr == end)
            ptr = obj->pairs;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_EMPTY)
            return NULL;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_DEAD)
            continue;

        if (str_cmp(&ptr->key, key))
            return &ptr->val;
    }

    return NULL;
}

jsean *jsean_obj_add(jsean *json, jsean *key, jsean *val)
{
    struct obj *obj;
    struct obj_pair *ptr, *end;
    size_t old_cap;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return NULL;

    if (jsean_get_type(key) != JSEAN_TYPE_STRING)
        return NULL;

    if (jsean_get_type(val) == JSEAN_TYPE_UNKNOWN)
        return NULL;

    if (!json->ao_ptr && (json->ao_ptr = obj_init()) == NULL)
        return NULL;

    obj = json->ao_ptr;

    if (get_load_factor(obj) > OBJECT_LOAD_FACTOR_MAX) {
        old_cap = obj->cap;

        ptr = realloc(obj->pairs, sizeof(*ptr) * next_capacity(old_cap));
        if (!ptr)
            return NULL;

        obj->cap = next_capacity(old_cap);

        for (end = ptr + obj->cap; ptr != end; ptr++)
            ptr->key.type = INTERNAL_TYPE_EMPTY;

        obj->pairs = ptr;
        obj_rehash(obj, old_cap);
    }

    ptr = obj->pairs + (str_hash(key) % obj->cap);
    end = ptr + obj->cap;

    for (;; ptr++) {
        if (ptr == end)
            ptr = obj->pairs;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_EMPTY)
            break;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_DEAD)
            continue;

        if (str_cmp(&ptr->key, key))
            return NULL;
    }

    memcpy(&ptr->key, key, sizeof(*key));
    memcpy(&ptr->val, val, sizeof(*val));
    obj->len++;

    return &ptr->val;
}

jsean *jsean_obj_set(jsean *json, jsean *key, jsean *val)
{
    jsean *tmp;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return NULL;

    if (jsean_get_type(key) != JSEAN_TYPE_STRING)
        return NULL;

    if (jsean_get_type(val) == JSEAN_TYPE_UNKNOWN)
        return NULL;

    if (!json->ao_ptr && (json->ao_ptr = obj_init()) == NULL)
        return NULL;

    tmp = jsean_obj_at(json, key);
    if (!tmp)
        return jsean_obj_add(json, key, val);

    jsean_free(tmp);
    memcpy(tmp, val, sizeof(*val));

    jsean_set_null(key);

    return tmp;
}

void jsean_obj_del(jsean *json, const jsean *key)
{
    struct obj *obj;
    struct obj_pair *ptr, *end;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return;

    if (jsean_get_type(key) != JSEAN_TYPE_STRING)
        return;

    obj = json->ao_ptr;
    if (!obj || obj->len == 0)
        return;

    ptr = obj->pairs + (str_hash(key) % obj->cap);
    end = ptr + obj->cap;

    for (;; ptr++) {
        if (ptr == end)
            ptr = obj->pairs;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_EMPTY)
            break;

        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_DEAD)
            continue;

        if (str_cmp(&ptr->key, key)) {
            str_free(&ptr->key);
            jsean_free(&ptr->val);

            ptr->key.type = INTERNAL_TYPE_DEAD;

            obj->len--;
            obj->dead++;
        }
    }
}

void jsean_object_clear(jsean *json)
{
    struct obj *obj;
    struct obj_pair *ptr, *end;

    if (jsean_get_type(json) != JSEAN_TYPE_OBJECT)
        return;

    obj = json->ao_ptr;
    if (!obj || obj->len == 0)
        return;

    for (ptr = obj->pairs, end = ptr + obj->cap; ptr != end; ptr++) {
        if (get_internal_type(&ptr->key) == INTERNAL_TYPE_EMPTY)
            continue;
        else if (get_internal_type(&ptr->key) == INTERNAL_TYPE_DEAD)
            continue;
        else {
            str_free(&ptr->key);
            jsean_free(&ptr->val);

            ptr->key.type = INTERNAL_TYPE_EMPTY;
        }
    }

    obj->len = 0;
    obj->dead = 0;
}

void obj_free(jsean *json)
{
    struct obj *obj = json->ao_ptr;

    if (!obj)
        return;

    jsean_object_clear(json);

    free(obj->pairs);
    free(obj);
}
