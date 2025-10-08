//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#ifndef JSEAN_JSEAN_H
#define JSEAN_JSEAN_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

enum jsean_type : unsigned char {
    // This is set to zero, so we can do !jsean_typeof()
    JSEAN_UNKNOWN = 0,

    JSEAN_NULL,
    JSEAN_BOOLEAN,
    JSEAN_OBJECT,
    JSEAN_ARRAY,
    JSEAN_NUMBER,
    JSEAN_STRING,

    JSEAN_TYPE_COUNT
};

enum jsean_status {
    JSEAN_SUCCESS = 0,

    JSEAN_CONVERSION_FAILED,
    JSEAN_EXPECTED_DIGIT,
    JSEAN_EXPECTED_FALSE,
    JSEAN_EXPECTED_MINUS_NONZERO_DIGIT,
    JSEAN_EXPECTED_NONZERO_DIGIT,
    JSEAN_EXPECTED_NULL,
    JSEAN_EXPECTED_QUOTATION_MARK,
    JSEAN_EXPECTED_TRUE,
    JSEAN_INVALID_ARGUMENTS,
    JSEAN_INVALID_ESCAPE_SEQUENCE,
    JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE,
    JSEAN_INVALID_UTF8,
    JSEAN_OUT_OF_MEMORY,
    JSEAN_UNEXPECTED_CHARACTER,

    JSEAN_STATUS_COUNT
};

typedef struct {
    enum jsean_type type;
    union {
        bool boolean;
        double number;
        void *pointer;
    };
} jsean;

// Get the type of a JSON value
enum jsean_type jsean_typeof(const jsean *json);

// Free a JSON value
void jsean_free(jsean *json);

// Read and write JSON data
enum jsean_status jsean_read(jsean *json, const char *bytes, const size_t len);

static inline enum jsean_status jsean_reads(jsean *json, const char *str)
{
    return jsean_read(json, str, strlen(str));
}

size_t jsean_write(const jsean *json, char *bytes, const size_t len);


// JSON null
// ---------

// jsean_set_null - Set a JSON value to null.
// @json: A pointer to the JSON value.
//
void jsean_set_null(jsean *json);


// JSON boolean
// ------------

// jsean_set_boolean - Set a JSON value to a boolean.
// @json: A pointer to the JSON value.
// @b: The boolean value to use.
//
void jsean_set_boolean(jsean *json, bool b);

// jsean_get_boolean - Get a boolean value from a JSON value.
// @json: A pointer to the JSON value.
//
bool jsean_get_boolean(const jsean *json);


// JSON object
// -----------

void jsean_object_print(jsean *json);

// jsean_set_object - Set a JSON value to an object.
// @json: A pointer to the JSON value.
//
void jsean_set_object(jsean *json);

// jsean_object_count - Get the number of elements in an object.
// @json: A pointer to the JSON object.
//
size_t jsean_object_count(const jsean *json);

// jsean_object_get - Get a value from an object.
// @json: A pointer to the JSON object.
// @key: The key to use.
//
jsean *jsean_object_get(const jsean *json, const char *key);

// jsean_object_insert - Insert a value to an object.
// @json: A pointer to the JSON object.
// @key: The key to use.
// @val: The JSON value to insert.
//
// If the key already exists, then no values are inserted and NULL is returned.
// Use jsean_object_replace for overwriting values.
//
jsean *jsean_object_insert(jsean *json, const char *key, jsean *val);

// jsean_object_replace - Replace a value in an object.
// @json: A pointer to the JSON object.
// @key: The key to use.
// @val: The JSON value to insert.
//
jsean *jsean_object_replace(jsean *json, const char *key, jsean *val);

// jsean_object_remove - Remove a value from an object.
// @json: A pointer to the JSON object.
// @key. The key to remove.
//
void jsean_object_remove(jsean *json, const char *key);

// jsean_object_clear - Remove all elements from an object.
// @json: A pointer to the JSON object.
void jsean_object_clear(jsean *json);


// JSON array
// ----------

// jsean_set_array - Set a JSON value to an array.
// @json: A pointer to the JSON value.
//
// Arrays are lazy-initialized, meaning that they do not allocate memory
// before JSEAN_ArrayInsert() is called.
//
void jsean_set_array(jsean *json);

// jsean_array_length - Get the length of an array.
// @json: A pointer to the JSON value.
//
size_t jsean_array_length(const jsean *json);

// jsean_array_at - Get a JSON value from an array at the given index.
// @json: A pointer to the JSON array.
// @index: The index to use.
//
// Checks for out of bounds indices.
//
jsean *jsean_array_at(const jsean *json, const size_t index);

// jsean_array_insert - Insert a value to a JSON array.
// @json: A pointer to the JSON array.
// @index: The index to use.
// @val: The value to insert.
//
jsean *jsean_array_insert(jsean *json, const size_t index, const jsean *val);

// jsean_array_push - Push a value to a JSON array.
// @json: A pointer to the JSON array.
// @val: The value to push.
//
static inline jsean *jsean_array_push(jsean *json, const jsean *val)
{
    return jsean_array_insert(json, jsean_array_length(json), val);
}

// jsean_array_remove - Remove a value from a JSON array at the given index.
// @json: A pointer to the JSON array.
// @index: The index to use.
// @out: A pointer to an optional output value. If left null, the popped
//         value is also freed.
//
void jsean_array_remove(jsean *json, const size_t index, jsean *out);

// jsean_array_pop - Remove the last element from a JSON array.
// @json: A pointer to the JSON array.
// @out: A pointer to an optional output value. If left null, the popped
//         value is also freed.
//
static inline void jsean_array_pop(jsean *json, jsean *out)
{
    jsean_array_remove(json, jsean_array_length(json) - 1, out);
}

// jsean_array_clear - Clear the contents of a JSON array.
// @json: A pointer to the JSON array.
//
void jsean_array_clear(jsean *json);


// JSON number
// -----------

// jsean_set_number - Set a JSON value to a number.
// @json: A pointer to the JSON value.
// @num: The number to use.
//
// Because JSON does not permit values like NaN or Infinity, the value is set
// to null instead. This works similarly to JavaScript's JSON.stringify().
// Note: See ECMA-262, section 25.5.2.2.
//
void jsean_set_number(jsean *json, double num);

// jsean_get_number - Get a number from a JSON value.
// @json: A pointer to the JSON number.
//
// Returns zero on failure.
//
double jsean_get_number(const jsean *json);


// JSON string
// -----------

// jsean_set_string - Set a JSON value to a string.
// @json: A pointer to the JSON value.
// @str: The string to use.
//
void jsean_set_string(jsean *json, const char *str);

// jsean_get_string - Get a string from a JSON value.
// @json: A pointer to the JSON value.
//
const char *jsean_get_string(jsean *json);

#endif // JSEAN_JSEAN_H
