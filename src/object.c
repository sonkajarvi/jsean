//
// Copyright (c) 2024, sonkajarvi
//

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/json.h>

#define OBJECT_INITIAL_CAPACITY     16
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

static float load_factor(struct json_object *object)
{
    return (float)object->count / object->capacity;
}

static bool is_empty(struct json_object *object, const size_t i)
{
    return !json_object_buffer(object)[i].key;
}

static struct json_object *to_object(struct json *json)
{
    if (!json || json->type != JSON_TYPE_OBJECT)
        return NULL;

    return json->data._object;
}

static struct json_object *object_resize(struct json *json,
    struct json_object *object, size_t new_cap)
{
    if (new_cap < OBJECT_INITIAL_CAPACITY)
        new_cap = OBJECT_INITIAL_CAPACITY;

    const size_t old_cap = object ? object->capacity : 0;

    struct json_object *tmp;
    tmp = realloc(object, sizeof(struct json_object) + new_cap * sizeof(struct json_object_entry));
    if (!tmp)
        goto fail;

    if (new_cap > old_cap) {
        memset(json_object_buffer(tmp) + old_cap, 0,
            (new_cap - old_cap) * sizeof(struct json_object_entry));
    }

    tmp->capacity = new_cap;
    json->data._object = tmp;

fail:
    return tmp;
}

static bool object_rehash(struct json_object *object, const size_t new_cap)
{
    struct json_object_entry *buffer;
    const char *key;

    const size_t old_cap = new_cap >= object->capacity ? new_cap / 2 : new_cap;

    buffer = json_object_buffer(object);
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

int json_set_object(struct json *json)
{
    if (!json)
        return EFAULT;

    if (json->type != JSON_TYPE_OBJECT) {
        json_free(json);
        json->data._object = NULL;
        json->type = JSON_TYPE_OBJECT;

        struct json_object *object = to_object(json);
        if (!(object = object_resize(json, object, OBJECT_INITIAL_CAPACITY)))
            return ENOMEM;

        memset(json_object_buffer(object), 0,
            object->capacity * sizeof(struct json_object_entry));
        object->count = 0;
    }

    return 0;
}

size_t json_object_count(struct json *json)
{
    struct json_object *object;
    if (!(object = to_object(json)))
        return 0;

    return object->count;
}

struct json *json_object_get(struct json *json, const char *key)
{
    struct json_object *object;
    if (!(object = to_object(json)))
        return NULL;

    struct json_object_entry *buffer = json_object_buffer(object);
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

struct json *json_object_insert(struct json *json, const char *key, struct json *other)
{
    return json_object_insert_without_copy(json, strdup(key), other);
}

// Internal
struct json *json_object_insert_without_copy(struct json *json, char *key, struct json *other)
{
    struct json_object *object;
    if (!(object = to_object(json)) || !key || !other)
        return NULL;

    // Allocate more memory and rehash
    if (load_factor(object) > OBJECT_LOAD_FACTOR_MAX) {
        if (!(object = object_resize(json, object, 2 * object->capacity)))
            return NULL;
        if (!object_rehash(object, object->capacity))
            return NULL;
    }

    struct json_object_entry *buffer = json_object_buffer(object);
    size_t i = hash(key) % object->capacity;
    while (!is_empty(object, i)) {
        // Duplicate key
        if (strcmp(key, buffer[i].key) == 0)
            return NULL;

        i++;
        if (i >= object->capacity)
            i = 0;
    }

    buffer[i].key = key;
    json_move(&buffer[i].value, other);
    object->count++;

    return other;
}

void json_object_remove(struct json *json, const char *key)
{
    struct json_object *object;
    if (!(object = to_object(json)))
        return;

    // Rehash and halve the capacity
    if (load_factor(object) < OBJECT_LOAD_FACTOR_MIN) {
        if (!object_rehash(object, object->capacity / 2))
            return;

        object = object_resize(json, object, object->capacity / 2);
    }

    struct json_object_entry *buffer = json_object_buffer(object);
    size_t i = hash(key) % object->capacity;
    if (is_empty(object, i))
        return;

    free(buffer[i].key);
    buffer[i].key = NULL;
    json_free(&buffer[i].value);
}
