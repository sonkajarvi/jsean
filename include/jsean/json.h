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

// Opaque JSON value
typedef struct json json_t;

// Parse/generate
json_t *json_parse(const char *bytes);
json_t *json_parse_ext(const char *bytes, int *error);

// Get type of JSON value
enum json_type json_type(json_t *json);

// Free any JSON value
void json_free(json_t *json);

// JSON null
json_t *json_new_null(void);
void json_set_null(json_t *json);

// JSON boolean
json_t *json_new_boolean(bool b);
void json_set_boolean(json_t *json, bool b);
bool json_boolean(json_t *json);

// JSON object
json_t *json_new_object(void);
size_t json_object_count(json_t *json);
json_t *json_object_get(json_t *json, const char *key);
json_t *json_object_insert(json_t *json, const char *key, json_t *other);
void json_object_remove(json_t *json, const char *key);

// JSON array
json_t *json_new_array(void);
size_t json_array_length(json_t *json);
json_t *json_array_at(json_t *json, size_t i);
json_t *json_array_push(json_t *json, json_t *other);
void json_array_pop(json_t *json);

// JSON number
json_t *json_new_signed(int64_t i);
void json_set_signed(json_t *json, int64_t i);
int64_t json_signed(json_t *json);

json_t *json_new_unsigned(uint64_t u);
void json_set_unsigned(json_t *json, uint64_t u);
uint64_t json_unsigned(json_t *json);

json_t *json_new_double(double d);
void json_set_double(json_t *json, double d);
double json_double(json_t *json);

// JSON string
json_t *json_new_string(const char *s);
void json_set_string(json_t *json, const char *s);
char *json_string(json_t *json);

#endif // JSEAN_JSON_H
