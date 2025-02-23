//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

// https://datatracker.ietf.org/doc/html/rfc8259

#ifndef JSEAN_JSON_H
#define JSEAN_JSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <jsean/string.h>

#ifndef JSON_OBJECT_DEFAULT_CAPACITY
#define JSON_OBJECT_DEFAULT_CAPACITY 16
#endif

#ifndef JSON_ARRAY_DEFAULT_CAPACITY
#define JSON_ARRAY_DEFAULT_CAPACITY  16
#endif

// JSON types
enum json_type
{
    JSON_TYPE_UNKNOWN,
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
};

// JSON value
typedef struct
{
    uint8_t type;

    union {
        bool                _boolean;
        double              _double;
        char               *_string;
        struct json_array  *_array;
        struct JSON_object *_object;
    } data;
} JSON;

struct json_array
{
    size_t capacity;
    size_t length;
};

struct JSON_object_entry
{
    JSON value;
    char *key;
};

struct JSON_object
{
    size_t capacity;
    size_t count;
};

// Move JSON value from @rhs to @lhs
void json_move(void *lhs, void *rhs);

static inline JSON *json_array_buffer(struct json_array *array)
{
    return (void *)array + sizeof(*array);
}

static inline struct JSON_object_entry *JSON_object_buffer(struct JSON_object *object)
{
    return (void *)object + sizeof(*object);
}

// Internal
JSON json_new_string_without_copy(char *s);

// Parse / generate
int json_parse(JSON *json, const char *bytes);
struct string json_stringify(JSON *json, const char *indent);

// Get type of JSON value
enum json_type json_type(JSON *json);

// Free any JSON value
void json_free(JSON *json);

// JSON null
int json_init_null(JSON *json);

// JSON boolean
int json_init_boolean(JSON *json, bool b);
bool json_boolean(JSON *json);

// JSON object
int json_init_object(JSON *json, const size_t n);
size_t JSON_object_count(JSON *json);
size_t JSON_object_capacity(JSON *json);
void JSON_object_clear(JSON *json);
JSON *JSON_object_get(JSON *json, const char *key);
int JSON_object_insert(JSON *json, const char *key, JSON *value);
int JSON_object_overwrite(JSON *json, const char *key, JSON *value);
void JSON_object_remove(JSON *json, const char *key);
// Internal: Does not copy key
int json_internal_object_insert(JSON *json, char *key, JSON *other);

// JSON array
int json_init_array(JSON *json, const size_t n);
size_t json_array_length(JSON *json);
size_t json_array_capacity(JSON *json);
int json_array_reserve(JSON *json, const size_t n);
void json_array_clear(JSON *json);
JSON *json_array_at(JSON *json, size_t i);
int json_array_push(JSON *json, JSON *value);
void json_array_pop(JSON *json);
int json_array_insert(JSON *json, size_t i, JSON *value);
void json_array_remove(JSON *json, size_t i);

// JSON number
int json_set_number(JSON *json, double d);
double json_number(JSON *json);

// JSON string
int json_set_string(JSON *json, const char *s);
char *json_string(JSON *json);

#endif // JSEAN_JSON_H
