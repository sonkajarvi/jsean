//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#ifndef INTERNAL_H
#define INTERNAL_H

#include <stddef.h>

#include "jsean/jsean.h"

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

void free_object(jsean *json);
void free_array(jsean *json);

#endif // INTERNAL_H
