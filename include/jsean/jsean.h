//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

// https://datatracker.ietf.org/doc/html/rfc8259

#ifndef JSEAN_JSEAN_H
#define JSEAN_JSEAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <jsean/string.h>

#ifndef JSEAN_OBJECT_DEFAULT_CAPACITY
# define JSEAN_OBJECT_DEFAULT_CAPACITY 16
#endif

#ifndef JSEAN_ARRAY_DEFAULT_CAPACITY
# define JSEAN_ARRAY_DEFAULT_CAPACITY  16
#endif

#define JSEAN_STATUS_LIST(F)                                  \
    F(JSEAN_OK, "OK")                                         \
    \
    F(JSEAN_NOT_SUPPORTED, "Not supported")                   \
    F(JSEAN_MEMORY_ERROR, "Memory error")                     \
    F(JSEAN_UNKNOWN_ERROR, "Unknown error")                   \
    \
    F(JSEAN_EXPECTED_EOF, "Expected end-of-file")             \
    F(JSEAN_UNEXPECTED_EOF, "Unexpected end-of-file")         \
    \
    F(JSEAN_EXPECTED_BEGIN_OBJECT, "Expected '{'")            \
    F(JSEAN_UNEXPECTED_END_OBJECT, "Unexpected '}'")          \
    \
    F(JSEAN_EXPECTED_ARRAY_BEGIN, "Expected '['")             \
    F(JSEAN_UNEXPECTED_END_ARRAY, "Unexpected ']'")           \
    \
    F(JSEAN_EXPECTED_NAME_SEPARATOR, "Expected ':'")          \
    F(JSEAN_EXPECTED_VALUE_SEPARATOR, "Expected ','")         \
    \
    F(JSEAN_EXPECTED_QUOTATION_MARK, "Expected '\"'")         \
    F(JSEAN_UNEXPECTED_CHARACTER, "Unexpected character")     \
    F(JSEAN_INVALID_UNICODE_ESCAPE, "Invalid Unicode escape") \
    \
    F(JSEAN_EXPECTED_FALSE, "Expected \"false\"")             \
    F(JSEAN_EXPECTED_NULL, "Expected \"null\"")               \
    F(JSEAN_EXPECTED_TRUE, "Expected \"true\"")               \
    \
    F(JSEAN_EXPECTED_DIGIT, "Expected digit")

enum jsean_status
{
#define F(name, _) name,
    JSEAN_STATUS_LIST(F)
#undef F
    JSEAN_STATUS_COUNT
};

// JSON types
enum jsean_type
{
    JSEAN_TYPE_NULL,
    JSEAN_TYPE_BOOLEAN,
    JSEAN_TYPE_OBJECT,
    JSEAN_TYPE_ARRAY,
    JSEAN_TYPE_NUMBER,
    JSEAN_TYPE_STRING,

    JSEAN_TYPE_UNKNOWN,
};

// JSON value
typedef struct
{
    uint8_t type;
    union {
        bool                 _boolean;
        double               _double;
        char                *_string;
        struct jsean_array  *_array;
        struct jsean_object *_object;
    } data;
} jsean_t;

struct jsean_array
{
    jsean_t *data;
    size_t cap;
    size_t size;
};

struct jsean_object_entry
{
    jsean_t value;
    char *key;
};

struct jsean_object
{
    struct jsean_object_entry *data;
    size_t cap;
    size_t count;
};

// Get the type of JSON value
enum jsean_type jsean_type(const jsean_t *json);

// Do a shallow copy on a JSON value
void jsean_copy(jsean_t *lhs, const jsean_t *rhs);

// Free any JSON value
void jsean_free(jsean_t *json);

// Parse / generate
int json_parse(jsean_t *json, const char *bytes);
struct string json_stringify(jsean_t *json, const char *indent);

const char *jsean_parse_result_to_string(int result);

// JSON null
int jsean_set_null(jsean_t *json);

// JSON boolean
int jsean_set_boolean(jsean_t *json, const bool b);
bool jsean_get_boolean(const jsean_t *json);

// JSON object
int jsean_set_object(jsean_t *json);
size_t jsean_object_count(const jsean_t *json);
size_t jsean_object_capacity(const jsean_t *json);
void jsean_object_clear(jsean_t *json);
jsean_t *jsean_object_get(const jsean_t *json, const char *key);
int jsean_object_insert(jsean_t *json, const char *key, jsean_t *val);
int jsean_object_overwrite(jsean_t *json, const char *key, jsean_t *val);
void jsean_object_remove(jsean_t *json, const char *key);

// Internal: Does not copy key
int jsean_internal_object_insert(jsean_t *json, char *key, jsean_t *other);
int jsean_internal_object_overwrite(jsean_t *json, char *key, jsean_t *value);

// JSON array
int jsean_set_array(jsean_t *json, size_t cap);
size_t jsean_array_size(const jsean_t *json);
size_t jsean_array_capacity(const jsean_t *json);
int jsean_array_reserve(jsean_t *json, size_t cap);
void jsean_array_clear(jsean_t *json);
jsean_t *jsean_array_at(const jsean_t *json, const size_t idx);
int jsean_array_push(jsean_t *json, const jsean_t *val);
void jsean_array_pop(jsean_t *json);
int jsean_array_insert(jsean_t *json, const size_t idx, const jsean_t *val);
void jsean_array_remove(jsean_t *json, const size_t idx);

// JSON number
int jsean_set_number(jsean_t *json, const double d);
double jsean_get_number(const jsean_t *json);

// JSON string
int jsean_set_string(jsean_t *json, const char *s);
int jsean_move_string(jsean_t *json, char *s);
char *jsean_get_string(jsean_t *json);

#endif // JSEAN_JSEAN_H
