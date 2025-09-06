//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#ifndef JSEAN_JSEAN_H_
#define JSEAN_JSEAN_H_

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

// JSON null
void JSEAN_SetNull(JSEAN *json);

// JSON boolean
void JSEAN_SetBoolean(JSEAN *json, bool b);
bool JSEAN_GetBoolean(const JSEAN *json);

// JSON object
void JSEAN_SetObject(JSEAN *json);

size_t JSEAN_ObjectCount(const JSEAN *json);
JSEAN *JSEAN_ObjectGet(const JSEAN *json, const char *key);

int JSEAN_ObjectInsert(JSEAN *json, const char *key, JSEAN *val);
int JSEAN_ObjectWrite(JSEAN *json, const char *key, JSEAN *val);
void JSEAN_ObjectRemove(JSEAN *json, const char *key);
void JSEAN_ObjectClear(JSEAN *json);

// JSON array
void JSEAN_SetArray(JSEAN *json);

size_t JSEAN_ArrayLength(const JSEAN *json);
JSEAN *JSEAN_ArrayAt(const JSEAN *json, const size_t index);

int JSEAN_ArrayPush(JSEAN *json, const JSEAN *val);
int JSEAN_ArrayInsert(JSEAN *json, const size_t index, const JSEAN *val);
void JSEAN_ArrayPop(JSEAN *json);
void JSEAN_ArrayRemove(JSEAN *json, const size_t index);
void JSEAN_ArrayClear(JSEAN *json);

// JSON number
void JSEAN_SetNumber(JSEAN *json, double d);
double JSEAN_GetNumber(const JSEAN *json);

// JSON string
void JSEAN_SetString(JSEAN *json, const char *s);
const char *JSEAN_GetString(JSEAN *json);

#endif // JSEAN_JSEAN_H_
