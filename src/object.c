//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/jsean.h>

#define OBJECT_LOAD_FACTOR_MAX 0.8
#define OBJECT_LOAD_FACTOR_MIN 0.2

// http://www.cse.yorku.ca/~oz/hash.html
static size_t hash(const char *s)
{
    size_t h = 0;
    int c;

    while ((c = *s++))
        h = ((h << 5) + h) + c;
    return h;
}

static float load_factor(struct jsean_object *object)
{
    return (float)object->count / object->cap;
}

static struct jsean_object *jsean_get_object(const jsean_t *json)
{
    if (!json || json->type != JSEAN_TYPE_OBJECT)
        return NULL;

    return json->data._object;
}

static jsean_t *jsean_object_at(const jsean_t *json, const size_t idx)
{
    struct jsean_object *obj;

    if ((obj = jsean_get_object(json)) == NULL || idx >= obj->cap)
        return NULL;

    if (obj->data[idx].key == NULL)
        return NULL;

    return &obj->data[idx].value;
}

static size_t jsean_object_index_of(jsean_t *json, const char *key)
{
    struct jsean_object *obj;

    if (!(obj = jsean_get_object(json)))
        return -1;

    size_t j = obj->cap, i = hash(key) % j;
    do {
        if (i >= obj->cap)
            i = 0;

        if (jsean_object_at(json, i) == NULL)
            continue;

        if (strcmp(key, obj->data[i].key) == 0)
            return i;
    } while ((i++, j--));

    return -1;
}

static int jsean_object_resize(jsean_t *json, size_t new_cap)
{
    struct jsean_object *obj;
    struct jsean_object_entry *tmp;

    if ((obj = jsean_get_object(json)) == NULL)
        return EINVAL;

    if ((tmp = realloc(obj->data, sizeof(*obj->data) * new_cap)) == NULL)
        return ENOMEM;

    obj->data = tmp;
    obj->cap = new_cap;

    return 0;
}

static bool jsean_object_rehash(jsean_t *json, const size_t old_cap)
{
    struct jsean_object *obj;
    const char *key;

    if ((obj = jsean_get_object(json)) == NULL)
        return false;

    for (size_t i = 0, j; i < old_cap; i++) {
        if (!(key = obj->data[i].key))
            continue;

        j = hash(key) % obj->cap;
        do {
            if (++j >= obj->cap)
                j = 0;
        } while (jsean_object_at(json, j) != NULL);

        obj->data[j].key = obj->data[i].key;
        jsean_move(&obj->data[j].value, &obj->data[i].value);
        obj->data[i].key = NULL;
    }

    return true;
}

int jsean_set_object(jsean_t *json)
{
    struct jsean_object *obj;

    if (!json)
        return EINVAL;

    if ((obj = malloc(sizeof(*obj))) == NULL)
        return ENOMEM;

    if ((obj->data = malloc(sizeof(*obj->data) * JSEAN_OBJECT_DEFAULT_CAPACITY)) == NULL)
        return ENOMEM;

    obj->cap = JSEAN_OBJECT_DEFAULT_CAPACITY;
    memset(obj->data, 0, sizeof(*obj->data) * obj->cap);
    obj->count = 0;

    json->data._object = obj;
    json->type = JSEAN_TYPE_OBJECT;

    return 0;
}

size_t jsean_object_count(const jsean_t *json)
{
    struct jsean_object *obj;

    if (!(obj = jsean_get_object(json)))
        return 0;

    return obj->count;
}

size_t jsean_object_capacity(const jsean_t *json)
{
    struct jsean_object *obj;

    if (!(obj = jsean_get_object(json)))
        return 0;

    return obj->cap;
}

void jsean_object_clear(jsean_t *json)
{
    struct jsean_object *obj;

    if (!(obj = jsean_get_object(json)))
        return;

    for (size_t i = 0; i < obj->cap; i++) {
        if (obj->data[i].key != NULL) {
            jsean_free(&obj->data[i].value);
            free(obj->data[i].key);
            obj->data[i].key = NULL;
        }
    }

    obj->count = 0;
}

jsean_t *jsean_object_get(const jsean_t *json, const char *key)
{
    struct jsean_object *obj;

    if ((obj = jsean_get_object(json)) == NULL || !key)
        return NULL;

    size_t j = obj->cap, i = hash(key) % j;
    do {
        if (i >= obj->cap)
            i = 0;

        if (!obj->data[i].key)
            continue;

        if (strcmp(key, obj->data[i].key) == 0)
            return &obj->data[i].value;
    } while ((i++, j--));

    return NULL;
}

int jsean_object_insert(jsean_t *json, const char *key, jsean_t *value)
{
    char *copy;
    int retval;

    if (!json || !key)
        return EINVAL;

    if ((copy = strdup(key)) == NULL)
        return ENOMEM;

    if ((retval = jsean_internal_object_insert(json, copy, value)) != 0)
        free(copy);

    return retval;
}

// Internal
int jsean_internal_object_insert(jsean_t *json, char *key, jsean_t *value)
{
    struct jsean_object *obj;

    if (!json || !key || !value)
        return EINVAL;

    if ((obj = jsean_get_object(json)) == NULL)
        return EINVAL;

    // Allocate more memory and rehash when needed
    if (load_factor(obj) > OBJECT_LOAD_FACTOR_MAX) {
        if (jsean_object_resize(json, 2 * obj->cap) != 0)
            return ENOMEM;

        obj = jsean_get_object(json);
        jsean_object_rehash(json, obj->cap / 2);
    }

    size_t i = hash(key) % obj->cap;
    while (obj->data[i].key) {
        if (i >= obj->cap) {
            i = 0;
            continue;
        }

        // Duplicate key
        if (strcmp(key, obj->data[i].key) == 0)
           return EINVAL;

        i++;
    }

    obj->data[i].key = key;
    jsean_move(&obj->data[i].value, value);
    obj->count++;

    return 0;
}

int jsean_object_overwrite(jsean_t *json, const char *key, jsean_t *value)
{
    struct jsean_object *object;
    jsean_t *tmp;

    if (!json || !key || !value)
        return EINVAL;

    if ((object = jsean_get_object(json)) == NULL)
        return EINVAL;

    if ((tmp = jsean_object_get(json, key)) == NULL)
        return jsean_object_insert(json, key, value);

    jsean_free(tmp);
    jsean_move(tmp, value);

    return 0;
}

int jsean_internal_object_overwrite(jsean_t *json, char *key, jsean_t *value)
{
    struct jsean_object *obj;
    jsean_t *tmp;

    if (!json || !key || !value)
        return EINVAL;

    if ((obj = jsean_get_object(json)) == NULL)
        return EINVAL;

    if ((tmp = jsean_object_get(json, key)) == NULL)
        return jsean_internal_object_insert(json, key, value);

    free(key);
    jsean_free(tmp);
    jsean_move(tmp, value);

    return 0;
}

void jsean_object_remove(jsean_t *json, const char *key)
{
    struct jsean_object *obj;
    size_t index;

    if ((obj = jsean_get_object(json)) == NULL)
        return;

    if ((index = jsean_object_index_of(json, key)) == (size_t)-1)
        return;

    free(obj->data[index].key);
    obj->data[index].key = NULL;
    jsean_free(&obj->data[index].value);
    obj->count--;

    // Rehash and halve the capacity when needed
    if (load_factor(obj) < OBJECT_LOAD_FACTOR_MIN) {
        if (!jsean_object_rehash(json, obj->cap / 2))
            return;

        jsean_object_resize(json, obj->cap / 2);
    }
}
