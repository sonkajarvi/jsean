//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <string.h>

#include "jsean/jsean.h"
#include "internal.h"

#define NUM_BUF_LEN 64

#define TRY_WRITE(writer, byte)                  \
    do {                                         \
        bool ret;                                \
        ret = (writer)->write((writer), (byte)); \
        if (!ret)                                \
            return false;                        \
    } while (0)

#define TRY_WRITE_LITERAL(writer, str)           \
    do {                                         \
        for (size_t i = 0; i < strlen(str); i++) \
            TRY_WRITE(writer, (str)[i]);         \
    } while (0)

struct writer {
    char n_buf[NUM_BUF_LEN];
    struct strbuf buf;
    const char *indent;

    bool (*write)(struct writer *, char);
};

static bool __write(struct writer *wr, char byte)
{
    return strbuf_add_byte(&wr->buf, byte);
}

static bool write_string(struct writer *wr, const char *str, size_t len);
static bool write_value(struct writer *wr, const jsean *json);

static bool write_object(struct writer *wr, const jsean *json)
{
    struct obj *obj;
    struct obj_pair *ptr;
    size_t len;

    TRY_WRITE(wr, '{');

    if (jsean_obj_len(json) > 0) {
        obj = json->ao_ptr;
        len = obj->len;

        for (ptr = obj->ptr; len > 1; ptr++) {
            if (jsean_get_type(&ptr->key) != JSEAN_TYPE_STRING)
                continue;

            if (wr->indent) {
                TRY_WRITE(wr, '\n');
                TRY_WRITE_LITERAL(wr, wr->indent);
            }

            write_value(wr, &ptr->key);
            TRY_WRITE(wr, ':');
            if (wr->indent)
                TRY_WRITE(wr, ' ');

            write_value(wr, &ptr->val);
            TRY_WRITE(wr, ',');

            len--;
        }

        if (wr->indent) {
            TRY_WRITE(wr, '\n');
            TRY_WRITE_LITERAL(wr, wr->indent);
        }

        while (jsean_get_type(&ptr->key) != JSEAN_TYPE_STRING)
            ptr++;

        write_value(wr, &ptr->key);
        TRY_WRITE(wr, ':');
        if (wr->indent)
            TRY_WRITE(wr, ' ');

        write_value(wr, &ptr->val);

        if (wr->indent)
            TRY_WRITE(wr, '\n');
    }

    TRY_WRITE(wr, '}');

    return true;
}

static bool write_array(struct writer *wr, const jsean *json)
{
    TRY_WRITE(wr, '[');

    if (jsean_arr_len(json) > 0) {
        for (size_t i = 0; i < jsean_arr_len(json) - 1; i++) {
            if (wr->indent) {
                TRY_WRITE(wr, '\n');
                TRY_WRITE_LITERAL(wr, wr->indent);
            }

            write_value(wr, jsean_arr_at(json, i));
            TRY_WRITE(wr, ',');
        }

        if (wr->indent) {
            TRY_WRITE(wr, '\n');
            TRY_WRITE_LITERAL(wr, wr->indent);
        }

        write_value(wr, jsean_arr_at(json, jsean_arr_len(json) - 1));

        if (wr->indent)
            TRY_WRITE(wr, '\n');
    }

    TRY_WRITE(wr, ']');

    return true;
}

// Trims trailing zeroes, and uses exponential notation for large numbers.
static bool write_number(struct writer *wr, double num)
{
    int idx, idx2, len;

    if (num != 0.0 && (num > 1e21 || num < 1e-7))
        idx = snprintf(wr->n_buf, NUM_BUF_LEN, "%e", num);
    else
        idx = snprintf(wr->n_buf, NUM_BUF_LEN, "%f", num);

    if (num != 0.0 && (num > 1e21 || num < 1e-7)) {
        idx2 = idx;

        while (wr->n_buf[idx - 1] != 'e')
            idx--;
        idx--;

        len = idx2 - idx;
        idx2 = idx;

        while (wr->n_buf[idx - 1] == '0')
            idx--;
        if (wr->n_buf[idx - 1] == '.')
            idx--;

        memcpy(&wr->n_buf[idx], &wr->n_buf[idx2], len);
        wr->n_buf[idx + len] = '\0';
    } else {
        while (wr->n_buf[idx - 1] == '0')
            idx--;
        if (wr->n_buf[idx - 1] == '.')
            idx--;

        wr->n_buf[idx] = '\0';
    }

    TRY_WRITE_LITERAL(wr, wr->n_buf);

    return true;
}

static bool write_string(struct writer *wr, const char *str, size_t len)
{
    TRY_WRITE(wr, '\"');

    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
        case 0x00: TRY_WRITE_LITERAL(wr, "\\u0000"); break;
        case 0x01: TRY_WRITE_LITERAL(wr, "\\u0001"); break;
        case 0x02: TRY_WRITE_LITERAL(wr, "\\u0002"); break;
        case 0x03: TRY_WRITE_LITERAL(wr, "\\u0003"); break;
        case 0x04: TRY_WRITE_LITERAL(wr, "\\u0004"); break;
        case 0x05: TRY_WRITE_LITERAL(wr, "\\u0005"); break;
        case 0x06: TRY_WRITE_LITERAL(wr, "\\u0006"); break;
        case 0x07: TRY_WRITE_LITERAL(wr, "\\u0007"); break;
        case 0x08: TRY_WRITE_LITERAL(wr, "\\b"); break;
        case 0x09: TRY_WRITE_LITERAL(wr, "\\t"); break;
        case 0x0a: TRY_WRITE_LITERAL(wr, "\\n"); break;
        case 0x0b: TRY_WRITE_LITERAL(wr, "\\u000b"); break;
        case 0x0c: TRY_WRITE_LITERAL(wr, "\\f"); break;
        case 0x0d: TRY_WRITE_LITERAL(wr, "\\r"); break;
        case 0x0e: TRY_WRITE_LITERAL(wr, "\\u000e"); break;
        case 0x0f: TRY_WRITE_LITERAL(wr, "\\u000f"); break;

        case 0x10: TRY_WRITE_LITERAL(wr, "\\u0010"); break;
        case 0x11: TRY_WRITE_LITERAL(wr, "\\u0011"); break;
        case 0x12: TRY_WRITE_LITERAL(wr, "\\u0012"); break;
        case 0x13: TRY_WRITE_LITERAL(wr, "\\u0013"); break;
        case 0x14: TRY_WRITE_LITERAL(wr, "\\u0014"); break;
        case 0x15: TRY_WRITE_LITERAL(wr, "\\u0015"); break;
        case 0x16: TRY_WRITE_LITERAL(wr, "\\u0016"); break;
        case 0x17: TRY_WRITE_LITERAL(wr, "\\u0017"); break;
        case 0x18: TRY_WRITE_LITERAL(wr, "\\u0018"); break;
        case 0x19: TRY_WRITE_LITERAL(wr, "\\u0019"); break;
        case 0x1a: TRY_WRITE_LITERAL(wr, "\\u001a"); break;
        case 0x1b: TRY_WRITE_LITERAL(wr, "\\u001b"); break;
        case 0x1c: TRY_WRITE_LITERAL(wr, "\\u001c"); break;
        case 0x1d: TRY_WRITE_LITERAL(wr, "\\u001d"); break;
        case 0x1e: TRY_WRITE_LITERAL(wr, "\\u001e"); break;
        case 0x1f: TRY_WRITE_LITERAL(wr, "\\u001f"); break;

        case 0x22: TRY_WRITE_LITERAL(wr, "\\\""); break;

        case 0x5c: TRY_WRITE_LITERAL(wr, "\\\\"); break;

        default:
            TRY_WRITE(wr, str[i]);
        }

    }

    TRY_WRITE(wr, '\"');

    return true;
}

static bool write_value(struct writer *wr, const jsean *json)
{
    switch (jsean_get_type(json)) {
    case JSEAN_TYPE_NULL:
        TRY_WRITE_LITERAL(wr, "null");
        return true;

    case JSEAN_TYPE_BOOLEAN:
        if (jsean_get_bool(json))
            TRY_WRITE_LITERAL(wr, "true");
        else
            TRY_WRITE_LITERAL(wr, "false");
        return true;

    case JSEAN_TYPE_OBJECT:
        return write_object(wr, json);

    case JSEAN_TYPE_ARRAY:
        return write_array(wr, json);

    case JSEAN_TYPE_NUMBER:
        return write_number(wr, jsean_get_num(json));

    case JSEAN_TYPE_STRING:
        return write_string(wr, jsean_get_str(json), jsean_str_len(json));

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
    wr.write = __write;

    if (!write_value(&wr, json))
        return NULL;

    if (len)
        *len = wr.buf.len;

    if (!strbuf_add_byte(&wr.buf, '\0'))
        return false;

    return wr.buf.data;
}
