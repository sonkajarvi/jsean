#include <stdbool.h>
#include <stdlib.h>

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

    if (buf->len >= buf->cap) {
        data = realloc(buf->data, sizeof(*data) * next_capacity(buf->cap));
        if (!data)
            return false;

        buf->data = data;
    }

    buf->data[buf->len++] = byte;
    return true;
}

bool strbuf_add_bytes(struct strbuf *buf, const char *bytes, size_t len)
{
    char *data;

    while (buf->len + len >= buf->cap) {
        data = realloc(buf->data, sizeof(*data) * next_capacity(buf->cap));
        if (!data)
            return false;

        buf->data = data;
    }

    memcpy(&buf->data[buf->len], bytes, len);
    buf->len += len;
    return true;
}

bool strbuf_add_codepoint(struct strbuf *buf, int cp)
{
    char *data;

    while (buf->len + 4 >= buf->cap) {
        data = realloc(buf->data, sizeof(*data) * next_capacity(buf->cap));
        if (!data)
            return false;

        buf->data = data;
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
