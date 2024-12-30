//
// Copyright (c) 2024, sonkajarvi
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

// Internal: JSON number types
enum json_number_type
{
    JSON_NUMBER_SIGNED,
    JSON_NUMBER_UNSIGNED,
    JSON_NUMBER_DOUBLE,
};

// JSON value
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

    uint8_t type; 
    uint8_t number_type;
};

struct json_array
{
    size_t capacity;
    size_t length;
};

struct json_object_entry
{
    struct json value;
    char *key;
};

struct json_object
{
    size_t capacity;
    size_t count;
};

// Move JSON value from @rhs to @lhs
void json_move(void *lhs, void *rhs);

static inline struct json *json_array_buffer(struct json_array *array)
{
    return (void *)array + sizeof(*array);
}

static inline struct json_object_entry *json_object_buffer(struct json_object *object)
{
    return (void *)object + sizeof(*object);
}

// Internal
struct json json_new_string_without_copy(char *s);

// Parse / generate
int json_parse(struct json *json, const char *bytes);
struct string json_stringify(struct json *json, const char *indent);

// Get type of JSON value
enum json_type json_type(struct json *json);

// Free any JSON value
void json_free(struct json *json);

// JSON null
int json_init_null(struct json *json);

// JSON boolean
int json_init_boolean(struct json *json, bool b);
bool json_boolean(struct json *json);

// JSON object
int json_init_object(struct json *json, const size_t n);
size_t json_object_count(struct json *json);
size_t json_object_capacity(struct json *json);
void json_object_clear(struct json *json);
struct json *json_object_get(struct json *json, const char *key);
int json_object_insert(struct json *json, const char *key, struct json *value);
int json_object_overwrite(struct json *json, const char *key, struct json *value);
void json_object_remove(struct json *json, const char *key);
// Internal: Does not copy key
int json_internal_object_insert(struct json *json, char *key, struct json *other);

// JSON array
int json_init_array(struct json *json, const size_t n);
size_t json_array_length(struct json *json);
size_t json_array_capacity(struct json *json);
int json_array_reserve(struct json *json, const size_t n);
void json_array_clear(struct json *json);
struct json *json_array_at(struct json *json, size_t i);
int json_array_push(struct json *json, struct json *value);
void json_array_pop(struct json *json);
int json_array_insert(struct json *json, size_t i, struct json *value);
void json_array_remove(struct json *json, size_t i);

// JSON number
int json_set_signed(struct json *json, int64_t i);
int64_t json_signed(struct json *json);

int json_set_unsigned(struct json *json, uint64_t u);
uint64_t json_unsigned(struct json *json);

int json_set_double(struct json *json, double d);
double json_double(struct json *json);

// JSON string
int json_set_string(struct json *json, const char *s);
char *json_string(struct json *json);

#endif // JSEAN_JSON_H
