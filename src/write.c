//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>

#include "jsean/jsean.h"
#include "internal.h"

#define WRITE(w, b) ((w)->write((w), (b)))

struct writer {
    struct strbuf buf;
    const char *indent;

    bool (*write)(struct writer *, char);
};

static bool write_buffer(struct writer *wr, char byte)
{
    return strbuf_add_byte(&wr->buf, byte);
}

static bool write_literal(struct writer *wr, const char *str, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (!WRITE(wr, str[i]))
            return false;
    }

    return true;
}

static bool write_value(struct writer *wr, const jsean *json)
{
    switch (jsean_typeof(json)) {
    case JSEAN_TYPE_NULL:
        return write_literal(wr, "null", 4);

    case JSEAN_TYPE_BOOLEAN:
        if (jsean_get_bool(json))
            return write_literal(wr, "true", 4);
        else
            return write_literal(wr, "false", 5);

    default:
        return false;
    }
}

char *jsean_write(const jsean *json, size_t *len, const char *indent)
{
    struct writer wr;

    if (!json)
        return NULL;

    if (!strbuf_init(&wr.buf))
        return NULL;

    wr.indent = indent;
    wr.write = write_buffer;

    if (!write_value(&wr, json))
        return NULL;

    if (len)
        *len = wr.buf.len;

    if (!strbuf_add_byte(&wr.buf, '\0'))
        return false;

    return wr.buf.data;
}
