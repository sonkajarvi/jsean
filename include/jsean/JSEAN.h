//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#ifndef JSEAN_JSEAN_H
#define JSEAN_JSEAN_H

#include <stdbool.h>
#include <stddef.h>

typedef enum : unsigned char {
    // JSEAN_Unknown is set to zero, so we can do !JSEAN_TypeOf()
    JSEAN_Unknown = 0,

    JSEAN_Null,
    JSEAN_Boolean,
    JSEAN_Object,
    JSEAN_Array,
    JSEAN_Number,
    JSEAN_String,

    JSEAN_TypeCount
} JSEAN_Type;

typedef struct {
    JSEAN_Type type;
    union {
        bool boolean;
        double number;
        void *pointer;
    };
} JSEAN;

// Get the type of a JSON value
JSEAN_Type JSEAN_TypeOf(const JSEAN *json);

// Free a JSON value
void JSEAN_Free(JSEAN *json);

// Read and write JSON data
int JSEAN_Read(const char *bytes, const size_t len, JSEAN *json);
size_t JSEAN_Write(const JSEAN *json, char *bytes, const size_t len);

// JSEAN_SetNull - Set a JSON value to null.
//   @json: A pointer to the JSON value.
//
void JSEAN_SetNull(JSEAN *json);

// JSEAN_SetBoolean - Set a JSON value to a boolean.
//   @json: A pointer to the JSON value.
//   @b: The boolean value to use.
//
void JSEAN_SetBoolean(JSEAN *json, bool b);

// JSEAN_GetBoolean - Get a boolean value from a JSON value.
//   @json: A pointer to the JSON value.
//
bool JSEAN_GetBoolean(const JSEAN *json);

void JSEAN_SetObject(JSEAN *json);
size_t JSEAN_ObjectCount(const JSEAN *json);
JSEAN *JSEAN_ObjectGet(const JSEAN *json, const char *key);
int JSEAN_ObjectInsert(JSEAN *json, const char *key, JSEAN *val);
int JSEAN_ObjectWrite(JSEAN *json, const char *key, JSEAN *val);
void JSEAN_ObjectRemove(JSEAN *json, const char *key);
void JSEAN_ObjectClear(JSEAN *json);

// JSEAN_SetArray - Set a JSON value to an array.
//   @json: A pointer to the JSON value.
//
// Arrays are lazy-initialized, meaning that they do not allocate memory
// before JSEAN_ArrayInsert() is called.
//
void JSEAN_SetArray(JSEAN *json);

// JSEAN_ArrayLength - Get the length of an array.
//   @json: A pointer to the JSON value.
//
size_t JSEAN_ArrayLength(const JSEAN *json);

// JSEAN_ArrayAt - Get a JSON value from an array at the given index.
//   @json: A pointer to the JSON array.
//   @index: The index to use.
//
// Checks for out of bounds indices.
//
JSEAN *JSEAN_ArrayAt(const JSEAN *json, const size_t index);

// JSEAN_ArrayInsert - Insert a value to a JSON array.
//   @json: A pointer to the JSON array.
//   @index: The index to use.
//   @val: The value to insert.
//
JSEAN *JSEAN_ArrayInsert(JSEAN *json, const size_t index, const JSEAN *val);

// JSEAN_ArrayPush - Push a value to a JSON array.
//   @json: A pointer to the JSON array.
//   @val: The value to push.
//
static inline JSEAN *JSEAN_ArrayPush(JSEAN *json, const JSEAN *val)
{
    return JSEAN_ArrayInsert(json, JSEAN_ArrayLength(json), val);
}

// JSEAN_ArrayRemove - Remove a value from a JSON array at the given index.
//   @json: A pointer to the JSON array.
//   @index: The index to use.
//   @out: A pointer to an optional output value. If left null, the popped
//         value is also freed.
//
void JSEAN_ArrayRemove(JSEAN *json, const size_t index, JSEAN *out);

// JSEAN_ArrayPop - Remove the last element from a JSON array.
//   @json: A pointer to the JSON array.
//   @out: A pointer to an optional output value. If left null, the popped
//         value is also freed.
//
static inline void JSEAN_ArrayPop(JSEAN *json, JSEAN *out)
{
    JSEAN_ArrayRemove(json, JSEAN_ArrayLength(json) - 1, out);
}

// JSEAN_ArrayClear - Clear the contents of a JSON array.
//   @json: A pointer to the JSON array.
//
void JSEAN_ArrayClear(JSEAN *json);

// JSEAN_SetNumber - Set a JSON value to a number.
//   @json: A pointer to the JSON value.
//   @num: The number to use.
//
// Because JSON does not permit values like NaN or Infinity, the value is set
// to null instead. This works similarly to JavaScript's JSON.stringify().
// Note: See ECMA-262, section 25.5.2.2.
//
void JSEAN_SetNumber(JSEAN *json, double num);

// JSEAN_GetNumber - Get a number from a JSON value.
//   @json: A pointer to the JSON number.
//
// Returns zero on failure.
//
double JSEAN_GetNumber(const JSEAN *json);

// JSEAN_SetString - Set a JSON value to a string.
//   @json: A pointer to the JSON value.
//   @str: The string to use.
//
void JSEAN_SetString(JSEAN *json, const char *str);

// JSEAN_GetString - Get a string from a JSON value.
//   @json: A pointer to the JSON value.
//
const char *JSEAN_GetString(JSEAN *json);

#endif // JSEAN_JSEAN_H
