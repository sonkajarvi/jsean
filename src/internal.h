//
// Copyright (c) 2024, sonkajarvi
//

#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <jsean/json.h>

// Internal JSON type extensions
enum json_type_ext
{
    JSON_TYPE_EXT_INT,
    JSON_TYPE_EXT_UINT,
    JSON_TYPE_EXT_DOUBLE,
};

struct json
{
    union {
        bool                _boolean;
        int64_t             _signed;
        uint64_t            _unsigned;
        double              _double;
        char               *_string;
        struct json_array  *_array;
        struct json_object *_object;
    } data;

    enum json_type type;
    enum json_type_ext type_ext;
};

struct json_array
{
    size_t capacity;
    size_t length;
};

struct json_object_node
{
    char *key;
    struct json *value;
};

struct json_object
{
    size_t capacity;
    size_t count;
};

// Free the underlying data of a JSON value
void json_free_underlying(json_t *json);

json_t *json_new_string_without_copy(char *s);
json_t *json_object_insert_without_copy(json_t *json, char *key, json_t *other);

static inline json_t **json_array_buffer(struct json_array *array)
{
    return (void *)array + sizeof(*array);
}

static inline struct json_object_node *json_object_buffer(struct json_object *object)
{
    return (void *)object + sizeof(*object);
}

#endif // INTERNAL_H
