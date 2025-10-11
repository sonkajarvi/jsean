//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define STRBUF_DEFAULT_CAPACITY 16

bool strbuf_init(struct strbuf *buf)
{
    char *data;

    buf->cap = STRBUF_DEFAULT_CAPACITY;

    data = malloc(sizeof(*data) * buf->cap);
    if (!data)
        return false;

    buf->data = data;
    buf->len = 0;

    return true;
}

void strbuf_free(struct strbuf *buf)
{
    if (buf && buf->data)
        free(buf->data);
}

// Assumes both @buf and @byte are valid
bool strbuf_add_byte(struct strbuf *buf, char byte)
{
    char *data;
    size_t cap;

    if (buf->len >= buf->cap) {
        cap = next_capacity(buf->cap);

        data = realloc(buf->data, sizeof(*data) * cap);
        if (!data)
            return false;

        buf->data = data;
        buf->cap = cap;
    }

    buf->data[buf->len++] = byte;

    return true;
}

bool strbuf_add_bytes(struct strbuf *buf, const char *bytes, size_t len)
{
    char *data;
    size_t cap;

    if (buf->len + len >= buf->cap) {
        cap = next_capacity(buf->cap);
        while (buf->len + len >= cap)
            cap = next_capacity(cap);

        data = realloc(buf->data, sizeof(*data) * cap);
        if (!data)
            return false;

        buf->data = data;
        buf->cap = cap;
    }

    memcpy(&buf->data[buf->len], bytes, len);
    buf->len += len;

    return true;
}

bool strbuf_add_codepoint(struct strbuf *buf, int cp)
{
    char *data;
    size_t cap;

    if (buf->len + 4 >= buf->cap) {
        cap = next_capacity(buf->cap);
        while (buf->len + 4 >= cap)
            cap = next_capacity(cap);

        data = realloc(buf->data, sizeof(*data) * cap);
        if (!data)
            return false;

        buf->data = data;
        buf->cap = cap;
    }

    // https://en.wikipedia.org/wiki/UTF-8#Description
    if (cp <= 0x7f) {
        strbuf_add_byte(buf,   cp        & 0b01111111);
    } else if (cp <= 0x7ff) {
        strbuf_add_byte(buf, ( cp >> 6)                | 0b11000000);
        strbuf_add_byte(buf, ( cp        & 0b00111111) | 0b10000000);
    } else if (cp <= 0xffff) {
        strbuf_add_byte(buf, ( cp >> 12)               | 0b11100000);
        strbuf_add_byte(buf, ((cp >>  6) & 0b00111111) | 0b10000000);
        strbuf_add_byte(buf, ( cp        & 0b00111111) | 0b10000000);
    } else if (cp <= 0x10ffff) {
        strbuf_add_byte(buf, ( cp >> 18)               | 0b11110000);
        strbuf_add_byte(buf, ((cp >> 12) & 0b00111111) | 0b10000000);
        strbuf_add_byte(buf, ((cp >>  6) & 0b00111111) | 0b10000000);
        strbuf_add_byte(buf, ( cp        & 0b00111111) | 0b10000000);
    }

    return true;
}
