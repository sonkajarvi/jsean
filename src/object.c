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

#define OBJECT_LOAD_FACTOR_MAX      0.8
#define OBJECT_LOAD_FACTOR_MIN      0.2

// djb2
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
    return (float)object->count / object->capacity;
}

static bool is_empty(struct JSON_object *object, const size_t i)
{
    return !JSON_object_buffer(object)[i].key;
}

static struct JSON_object *to_object(JSON *json)
{
    if (!json || json->type != JSON_TYPE_OBJECT)
        return NULL;

    return json->data._object;
}

static size_t JSON_object_index_of(JSON *json, const char *key)
{
    struct JSON_object *object;
    if (!(object = to_object(json)))
        return -1;

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    size_t j = object->capacity, i = hash(key) % j;
    do {
        if (i >= object->capacity)
            i = 0;

        if (is_empty(object, i))
            continue;

        if (strcmp(key, buffer[i].key) == 0)
            return i;
    } while ((i++, j--));

    return -1;
}

static struct JSON_object *object_resize(JSON *json,
    struct JSON_object *object, size_t new_cap)
{
    const size_t old_cap = object ? object->capacity : 0;

    struct JSON_object *tmp;
    tmp = realloc(object, sizeof(struct JSON_object) + new_cap * sizeof(struct JSON_object_entry));
    if (!tmp)
        goto fail;

    if (new_cap > old_cap) {
        memset(JSON_object_buffer(tmp) + old_cap, 0,
            (new_cap - old_cap) * sizeof(struct JSON_object_entry));
    }

    tmp->capacity = new_cap;
    json->data._object = tmp;

fail:
    return tmp;
}

static bool object_rehash(struct JSON_object *object, const size_t new_cap)
{
    struct JSON_object_entry *buffer;
    const char *key;

    const size_t old_cap = new_cap >= object->capacity ? new_cap / 2 : new_cap;

    buffer = JSON_object_buffer(object);
    for (size_t i = 0, j; i < old_cap; i++) {
        if (!(key = buffer[i].key))
            continue;

        j = hash(key) % new_cap;
        do {
            if (++j >= new_cap)
                j = 0;
        } while (!is_empty(object, j));

        buffer[j].key = buffer[i].key;
        json_move(&buffer[j].value, &buffer[i].value);
        buffer[i].key = NULL;
    }

    return true;
}

int json_init_object(JSON *json, const size_t n)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_OBJECT) {
        json_free(json);
        json->data._object = NULL;
        json->type = JSON_TYPE_OBJECT;

        struct JSON_object *object = to_object(json);
        if (!(object = object_resize(json, object, n ?: JSON_OBJECT_DEFAULT_CAPACITY)))
            return ENOMEM;

        memset(JSON_object_buffer(object), 0,
            object->capacity * sizeof(struct JSON_object_entry));
        object->count = 0;
    }

    return 0;
}

size_t JSON_object_count(JSON *json)
{
    struct JSON_object *object;
    if (!(object = to_object(json)))
        return 0;

    return object->count;
}

size_t JSON_object_capacity(JSON *json)
{
    struct JSON_object *object;
    if (!(object = to_object(json)))
        return 0;

    return object->capacity;
}

void JSON_object_clear(JSON *json)
{
    struct JSON_object *object;
    if (!(object = to_object(json)))
        return;

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    for (size_t i = 0; i < object->capacity; i++) {
        if (!is_empty(object, i)) {
            free(buffer[i].key);
            json_free(&buffer[i].value);
            buffer[i].key = NULL;
        }
    }

    object->count = 0;
}

JSON *JSON_object_get(JSON *json, const char *key)
{
    struct JSON_object *object;
    if (!(object = to_object(json)))
        return NULL;

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    size_t j = object->capacity, i = hash(key) % j;
    do {
        if (i >= object->capacity)
            i = 0;

        if (!buffer[i].key)
            continue;

        if (strcmp(key, buffer[i].key) == 0)
            return &buffer[i].value;
    } while ((i++, j--));

    return NULL;
}

int JSON_object_insert(JSON *json, const char *key, JSON *value)
{
    char *copy;
    int retval;

    if (!key)
        return EFAULT;
    if (!(copy = strdup(key)))
        return ENOMEM;
    if ((retval = json_internal_object_insert(json, copy, value)) != 0)
        free(copy);
    return retval;
}

// Internal
int json_internal_object_insert(JSON *json, char *key, JSON *value)
{
    struct JSON_object *object;
    if (!json || !key || !value)
        return EFAULT;

    if (!(object = to_object(json)) || json_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    // Allocate more memory and rehash
    if (load_factor(object) > OBJECT_LOAD_FACTOR_MAX) {
        if (!(object = object_resize(json, object, 2 * object->capacity)))
            return ENOMEM;
        object_rehash(object, object->capacity);
    }

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    size_t i = hash(key) % object->capacity;
    while (!is_empty(object, i)) {
        // Duplicate key
        if (strcmp(key, buffer[i].key) == 0)
            return EINVAL;

        i++;
        if (i >= object->capacity)
            i = 0;
    }

    buffer[i].key = key;
    json_move(&buffer[i].value, value);
    object->count++;

    return 0;
}

int JSON_object_overwrite(JSON *json, const char *key, JSON *value)
{
    struct JSON_object *object;
    JSON *tmp;

    if (!json || !key || !value)
        return EFAULT;

    if (!(object = to_object(json)) || json_type(value) == JSON_TYPE_UNKNOWN)
        return EINVAL;

    if ((tmp  = JSON_object_get(json, key)) == NULL)
        return JSON_object_insert(json, key, value);

    json_move(tmp, value);
    return 0;
}

void JSON_object_remove(JSON *json, const char *key)
{
    struct JSON_object *object;
    size_t index;

    if (!(object = to_object(json)))
        return;

    if ((index = JSON_object_index_of(json, key)) == (size_t)-1)
        return;

    struct JSON_object_entry *buffer = JSON_object_buffer(object);
    free(buffer[index].key);
    buffer[index].key = NULL;
    json_free(&buffer[index].value);
    object->count--;

    // Rehash and halve the capacity if needed
    if (load_factor(object) < OBJECT_LOAD_FACTOR_MIN) {
        if (!object_rehash(object, object->capacity / 2))
            return;

        object = object_resize(json, object, object->capacity / 2);
    }
}
