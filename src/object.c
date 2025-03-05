//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/JSON.h>

#define OBJECT_LOAD_FACTOR_MAX 0.8
#define OBJECT_LOAD_FACTOR_MIN 0.2

// djb2: http://www.cse.yorku.ca/~oz/hash.html
static size_t hash(const char *s)
{
    size_t h = 0;
    int c;

    while ((c = *s++))
        h = ((h << 5) + h) + c;
    return h;
}

static float load_factor(struct JSON_object *object)
{
    return (float)object->count / object->cap;
}

static struct JSON_object *JSON_object(const JSON *json)
{
    if (!json || json->type != JSON_TYPE_OBJECT)
    return NULL;

return json->data._object;
}

static JSON *JSON_object_at(const JSON *json, const size_t idx)
{
    struct JSON_object *obj;

    if ((obj = JSON_object(json)) == NULL || idx >= obj->cap)
        return NULL;

    if (obj->data[idx].key == NULL)
        return NULL;

    return &obj->data[idx].value;
}

static size_t JSON_object_index_of(JSON *json, const char *key)
{
    struct JSON_object *obj;

    if (!(obj = JSON_object(json)))
        return -1;

    size_t j = obj->cap, i = hash(key) % j;
    do {
        if (i >= obj->cap)
            i = 0;

        if (JSON_object_at(json, i) == NULL)
            continue;

        if (strcmp(key, obj->data[i].key) == 0)
            return i;
    } while ((i++, j--));

    return -1;
}

static int JSON_object_resize(JSON *json, size_t new_cap)
{
    struct JSON_object *obj;
    struct JSON_object_entry *tmp;

    if ((obj = JSON_object(json)) == NULL)
        return EINVAL;

    if ((tmp = realloc(obj->data, sizeof(*obj->data) * new_cap)) == NULL)
        return ENOMEM;

    obj->data = tmp;
    obj->cap = new_cap;

    return 0;
}

static bool JSON_object_rehash(JSON *json, const size_t old_cap)
{
    struct JSON_object *obj;
    const char *key;

    if ((obj = JSON_object(json)) == NULL)
        return false;

    for (size_t i = 0, j; i < old_cap; i++) {
        if (!(key = obj->data[i].key))
            continue;

        j = hash(key) % obj->cap;
        do {
            if (++j >= obj->cap)
                j = 0;
        } while (JSON_object_at(json, j) != NULL);

        obj->data[j].key = obj->data[i].key;
        JSON_move(&obj->data[j].value, &obj->data[i].value);
        obj->data[i].key = NULL;
    }

    return true;
}

int JSON_set_object(JSON *json)
{
    struct JSON_object *obj;

    if (!json)
        return EINVAL;

    if ((obj = malloc(sizeof(*obj))) == NULL)
        return ENOMEM;

    if ((obj->data = malloc(sizeof(*obj->data) * JSON_OBJECT_DEFAULT_CAPACITY)) == NULL)
        return ENOMEM;

    obj->cap = JSON_OBJECT_DEFAULT_CAPACITY;
    memset(obj->data, 0, sizeof(*obj->data) * obj->cap);
    obj->count = 0;

    json->data._object = obj;
    json->type = JSON_TYPE_OBJECT;

    return 0;
}

size_t JSON_object_count(const JSON *json)
{
    struct JSON_object *obj;

    if (!(obj = JSON_object(json)))
        return 0;

    return obj->count;
}

size_t JSON_object_capacity(const JSON *json)
{
    struct JSON_object *obj;

    if (!(obj = JSON_object(json)))
        return 0;

    return obj->cap;
}

void JSON_object_clear(JSON *json)
{
    struct JSON_object *obj;

    if (!(obj = JSON_object(json)))
        return;

    for (size_t i = 0; i < obj->cap; i++) {
        if (obj->data[i].key != NULL) {
            JSON_free(&obj->data[i].value);
            free(obj->data[i].key);
            obj->data[i].key = NULL;
        }
    }

    obj->count = 0;
}

JSON *JSON_object_get(const JSON *json, const char *key)
{
    struct JSON_object *obj;

    if ((obj = JSON_object(json)) == NULL || !key)
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

int JSON_object_insert(JSON *json, const char *key, JSON *value)
{
    char *copy;
    int retval;

    if (!json || !key)
        return EINVAL;

    if ((copy = strdup(key)) == NULL)
        return ENOMEM;

    if ((retval = json_internal_object_insert(json, copy, value)) != 0)
        free(copy);

    return retval;
}

// Internal
int json_internal_object_insert(JSON *json, char *key, JSON *value)
{
    struct JSON_object *obj;

    if (!json || !key || !value)
        return EINVAL;

    if ((obj = JSON_object(json)) == NULL)
        return EINVAL;

    // Allocate more memory and rehash when needed
    if (load_factor(obj) > OBJECT_LOAD_FACTOR_MAX) {
        if (JSON_object_resize(json, 2 * obj->cap) != 0)
            return ENOMEM;

        obj = JSON_object(json);
        JSON_object_rehash(json, obj->cap / 2);
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
    JSON_move(&obj->data[i].value, value);
    obj->count++;

    return 0;
}

int JSON_object_overwrite(JSON *json, const char *key, JSON *value)
{
    struct JSON_object *object;
    JSON *tmp;

    if (!json || !key || !value)
        return EINVAL;

    if ((object = JSON_object(json)) == NULL || JSON_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    if ((tmp = JSON_object_get(json, key)) == NULL)
        return JSON_object_insert(json, key, value);

    JSON_move(tmp, value);
    return 0;
}

void JSON_object_remove(JSON *json, const char *key)
{
    struct JSON_object *obj;
    size_t index;

    if ((obj = JSON_object(json)) == NULL)
        return;

    if ((index = JSON_object_index_of(json, key)) == (size_t)-1)
        return;

    free(obj->data[index].key);
    obj->data[index].key = NULL;
    JSON_free(&obj->data[index].value);
    obj->count--;

    // Rehash and halve the capacity when needed
    if (load_factor(obj) < OBJECT_LOAD_FACTOR_MIN) {
        if (!JSON_object_rehash(json, obj->cap / 2))
            return;

        JSON_object_resize(json, obj->cap / 2);
    }
}
