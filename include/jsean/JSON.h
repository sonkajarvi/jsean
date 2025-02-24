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
enum JSON_type
{
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_UNKNOWN,
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
    JSON *data;
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

static inline struct JSON_object_entry *JSON_object_buffer(struct JSON_object *object)
{
    return (void *)object + sizeof(*object);
}

// Get type of JSON value
enum JSON_type JSON_type(const JSON *json);

// Move JSON value from @rhs to @lhs
void JSON_move(void *lhs, const void *rhs);

// Free any JSON value
void JSON_free(JSON *json);

// Parse / generate
int json_parse(JSON *json, const char *bytes);
struct string json_stringify(JSON *json, const char *indent);

// JSON null
int JSON_set_null(JSON *json);

// JSON boolean
int JSON_set_boolean(JSON *json, const bool b);
bool JSON_boolean(const JSON *json);

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
int JSON_set_array(JSON *json, size_t cap);
size_t JSON_array_size(const JSON *json);
size_t JSON_array_capacity(const JSON *json);
int JSON_array_reserve(JSON *json, const size_t cap);
void JSON_array_clear(JSON *json);
JSON *JSON_array_at(const JSON *json, const size_t idx);
int JSON_array_push(JSON *json, const JSON *val);
void JSON_array_pop(JSON *json);
int JSON_array_insert(JSON *json, const size_t idx, const JSON *val);
void JSON_array_remove(JSON *json, const size_t idx);

// JSON number
int JSON_set_number(JSON *json, const double d);
double JSON_number(const JSON *json);

// JSON string
int JSON_set_string(JSON *json, const char *s);
int JSON_move_string(JSON *json, char *s);
char *JSON_string(JSON *json);

#endif // JSEAN_JSON_H
