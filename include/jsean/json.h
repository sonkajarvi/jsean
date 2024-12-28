//
// Copyright (c) 2024, sonkajarvi
//

// https://datatracker.ietf.org/doc/html/rfc8259

#ifndef JSEAN_JSON_H
#define JSEAN_JSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

// Internal: JSON number type
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

static inline struct json *json_array_buffer(struct json_array *array)
{
    return (void *)array + sizeof(*array);
}

static inline struct json_object_entry *json_object_buffer(struct json_object *object)
{
    return (void *)object + sizeof(*object);
}

struct json json_new_string_without_copy(char *s);
struct json *json_object_insert_without_copy(struct json *json, char *key, struct json *other);

// Parse / generate
struct json json_parse(const char *bytes);
struct json json_parse_ext(const char *bytes, int *error);
char *json_stringify(struct json *json, const char *indent_s);

// Get type of JSON value
enum json_type json_type(struct json *json);

// Free any JSON value
void json_free(struct json *json);

// JSON null
struct json json_new_null(void);
void json_set_null(struct json *json);

// JSON boolean
struct json json_new_boolean(bool b);
void json_set_boolean(struct json *json, bool b);
bool json_boolean(struct json *json);

// JSON object
struct json json_new_object(void);
size_t json_object_count(struct json *json);
struct json *json_object_get(struct json *json, const char *key);
struct json *json_object_insert(struct json *json, const char *key, struct json *other);
void json_object_remove(struct json *json, const char *key);

// JSON array
struct json json_new_array(void);
size_t json_array_length(struct json *json);
struct json *json_array_at(struct json *json, size_t i);
struct json *json_array_push(struct json *json, struct json *other);
void json_array_pop(struct json *json);

// JSON number
struct json json_new_signed(int64_t i);
void json_set_signed(struct json *json, int64_t i);
int64_t json_signed(struct json *json);

struct json json_new_unsigned(uint64_t u);
void json_set_unsigned(struct json *json, uint64_t u);
uint64_t json_unsigned(struct json *json);

struct json json_new_double(double d);
void json_set_double(struct json *json, double d);
double json_double(struct json *json);

// JSON string
struct json json_new_string(const char *s);
void json_set_string(struct json *json, const char *s);
char *json_string(struct json *json);

#endif // JSEAN_JSON_H
