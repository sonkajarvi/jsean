//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#ifndef INTERNAL_H
#define INTERNAL_H

#include <stddef.h>

#include "jsean/jsean.h"

#define STRING_LENGTH_MAX           4294967294 // 2^32-1
#define STRING_HASH_UNDEFINED       0

#define OBJECT_DEFAULT_CAPACITY     16
#define OBJECT_LOAD_FACTOR_MAX      0.67

enum internal_type {
    // Same as jsean_type
    INTERNAL_TYPE_NULL,
    INTERNAL_TYPE_BOOLEAN,
    INTERNAL_TYPE_OBJECT,
    INTERNAL_TYPE_ARRAY,
    INTERNAL_TYPE_NUMBER,
    INTERNAL_TYPE_STRING,

    // For object internals
    INTERNAL_TYPE_EMPTY,
    INTERNAL_TYPE_DEAD,

    __INTERNAL_TYPE_COUNT,

    INTERNAL_TYPE_UNKNOWN,
};

struct obj {
    struct obj_pair *pairs;
    unsigned int cap;
    unsigned int len;
    unsigned int dead;
};

struct obj_pair {
    jsean key;
    jsean val;
};

struct strbuf {
    char *data;
    size_t cap;
    size_t len;
};

// Grows approximately by a factor of 1.6
static inline size_t next_capacity(const size_t n)
{
    if (n == 1)
        return 2;

    return n + (n >> 1) + (n >> 3);
}

unsigned int get_internal_type(const jsean *json);

// These return false if they fail to allocate memory.
bool strbuf_init(struct strbuf *buf);
void strbuf_free(struct strbuf *buf);
bool strbuf_add_byte(struct strbuf *buf, char byte);
bool strbuf_add_bytes(struct strbuf *buf, const char *str, size_t len);
bool strbuf_add_codepoint(struct strbuf *buf, int cp);

static inline void strbuf_clear(struct strbuf *buf)
{
    buf->len = 0;
}

void obj_free(jsean *json);
void free_array(jsean *json);

bool str_cmp(const jsean *json, const jsean *other);
size_t str_hash(const jsean *json);
void str_free(jsean *json);

#endif // INTERNAL_H
