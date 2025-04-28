//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

// An implementation of JSON in C, as specified in RFC 8259:
//
//     https://datatracker.ietf.org/doc/html/rfc8259

#ifndef JSEAN_JSEAN_H_
#define JSEAN_JSEAN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef JSEAN_OBJECT_DEFAULT_CAPACITY
# define JSEAN_OBJECT_DEFAULT_CAPACITY 16
#endif

#ifndef JSEAN_ARRAY_DEFAULT_CAPACITY
# define JSEAN_ARRAY_DEFAULT_CAPACITY 16
#endif

#define JSEAN_STATUS_LIST_(F)                                 \
    F(JSEAN_OK, "OK")                                         \
    \
    F(JSEAN_NOT_SUPPORTED, "Not supported")                   \
    F(JSEAN_MEMORY_ERROR, "Memory error")                     \
    F(JSEAN_UNKNOWN_ERROR, "Unknown error")                   \
    \
    F(JSEAN_EXPECTED_END_OF_FILE, "Expected end-of-file")     \
    F(JSEAN_UNEXPECTED_END_OF_FILE, "Unexpected end-of-file") \
    \
    F(JSEAN_UNEXPECTED_END_OF_ARRAY, "Unexpected ']'")        \
    F(JSEAN_UNEXPECTED_END_OF_OBJECT, "Unexpected '}'")       \
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
    JSEAN_STATUS_LIST_(F)
#undef F
    JSEAN_STATUS_COUNT_
};

// JSON types
enum jsean_type
{
    JSEAN_TYPE_UNKNOWN_,

    JSEAN_TYPE_NULL,
    JSEAN_TYPE_BOOLEAN,
    JSEAN_TYPE_OBJECT,
    JSEAN_TYPE_ARRAY,
    JSEAN_TYPE_NUMBER,
    JSEAN_TYPE_STRING,

    JSEAN_TYPE_COUNT_
};

// JSON value
typedef struct
{
    uint8_t type;
    union {
        bool                  _boolean;
        double                _double;
        char                 *_string;
        struct jsean_array_  *_array;
        struct jsean_object_ *_object;
    } data;
} jsean_t;

struct jsean_array_
{
    jsean_t *data;
    size_t cap;
    size_t size;
};

struct jsean_object_entry_
{
    jsean_t value;
    char *key;
};

struct jsean_object_
{
    struct jsean_object_entry_ *data;
    size_t cap;
    size_t count;
};

// Get type of JSON value
enum jsean_type jsean_type(const jsean_t *json);

// Move JSON value
void jsean_move_(jsean_t *to, const jsean_t *from);

// Free JSON value
void jsean_free(jsean_t *json);

// JSON read/write
int jsean_read(const char *bytes, const size_t size, jsean_t *json);
size_t jsean_write(const jsean_t *json, char *bytes, const size_t size);

// JSON null
int jsean_set_null(jsean_t *json);

// JSON boolean
int jsean_set_boolean(jsean_t *json, const bool b);
bool jsean_get_boolean(const jsean_t *json);

// JSON object
int jsean_set_object(jsean_t *json);

size_t jsean_object_count(const jsean_t *json);
size_t jsean_object_capacity(const jsean_t *json);
jsean_t *jsean_object_get(const jsean_t *json, const char *key);

int jsean_object_insert(jsean_t *json, const char *key, jsean_t *val);
int jsean_object_overwrite(jsean_t *json, const char *key, jsean_t *val);
void jsean_object_remove(jsean_t *json, const char *key);
void jsean_object_clear(jsean_t *json);

// Internal: Does not copy key
int jsean_internal_object_insert_(jsean_t *json, char *key, jsean_t *val);
int jsean_internal_object_overwrite_(jsean_t *json, char *key, jsean_t *val);

// JSON array
int jsean_set_array(jsean_t *json, size_t cap);

size_t jsean_array_size(const jsean_t *json);
size_t jsean_array_capacity(const jsean_t *json);
jsean_t *jsean_array_at(const jsean_t *json, const size_t idx);

int jsean_array_push(jsean_t *json, const jsean_t *val);
int jsean_array_insert(jsean_t *json, const size_t idx, const jsean_t *val);
void jsean_array_pop(jsean_t *json);
void jsean_array_remove(jsean_t *json, const size_t idx);
int jsean_array_reserve(jsean_t *json, size_t cap);
void jsean_array_clear(jsean_t *json);

// JSON number
int jsean_set_number(jsean_t *json, const double d);
double jsean_get_number(const jsean_t *json);

// JSON string
int jsean_set_string(jsean_t *json, const char *s);
int jsean_move_string(jsean_t *json, char *s);
char *jsean_get_string(jsean_t *json);

#endif // JSEAN_JSEAN_H_
