//
// Copyright (c) 2024-2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdlib.h>

#include "jsean.h"
#include "jsean_internal.h"

static const char *__type_strings[] = {
#define X(type_, str_) str_,
    __JSEAN_TYPE_LIST(X)
#undef X
};

static const char *__status_strings[] = {
#define X(status_, str_) str_,
    __JSEAN_STATUS_LIST(X)
#undef X
};

unsigned int get_internal_type(const jsean *json)
{
    if (!json || json->type >= __INTERNAL_TYPE_COUNT)
        return INTERNAL_TYPE_UNKNOWN;

    return json->type;
}

unsigned int jsean_get_type(const jsean *json)
{
    if (!json || json->type >= __JSEAN_TYPE_COUNT)
        return JSEAN_TYPE_UNKNOWN;

    return json->type;
}

const char *jsean_type_to_str(unsigned int type)
{
    if (type > __JSEAN_TYPE_COUNT)
        return NULL;

    return __type_strings[type];
}

const char *jsean_status_to_str(int status)
{
    if (status > __JSEAN_STATUS_COUNT)
        return NULL;

    return __status_strings[status];
}

void jsean_free(jsean *json)
{
    switch (jsean_get_type(json)) {
    case JSEAN_TYPE_OBJECT:
        obj_free(json);
        break;

    case JSEAN_TYPE_ARRAY:
        arr_free(json);
        break;

    case JSEAN_TYPE_STRING:
        str_free(json);
        break;

    default:
        break;
    }
}

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
