//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "jsean.h"
#include "jsean_internal.h"

#define PEEK(p) ((p)->peek(p))
#define READ(p) ((p)->read(p))

struct parser {
    struct strbuf buf;
    union {
        struct {
            const char *ptr, *end;
        };
        struct {
            FILE *fp;
        };
    };

    // These return -1 for EOF
    int (*peek)(struct parser *);
    int (*read)(struct parser *);
};

static int peek_buffer(struct parser *p)
{
    if (p->ptr >= p->end)
        return -1;

    return *p->ptr;
}

static int read_buffer(struct parser *p)
{
    int c;

    if ((c = PEEK(p)) == -1)
        return -1;
    p->ptr++;

    return c;
}

static int peek_stream(struct parser *p)
{
    int c;

    c = fgetc(p->fp);
    if (c == EOF)
        return -1;
    ungetc(c, p->fp);

    return c;
}

static int read_stream(struct parser *p)
{
    int c;

    c = fgetc(p->fp);
    if (c == EOF)
        return -1;

    return c;
}

static int parse_value(struct parser *p, jsean *json);
static int parse_string(struct parser *p, jsean *json);

static inline bool isdec(const char c)
{
    return c >= '0' && c <= '9';
}

static inline bool ishex(const char c)
{
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

static inline int hextoi(const char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}

static inline bool isnonzero(const char c)
{
    return c >= '1' && c <= '9';
}

// ws = *(
//         %x20 /               ; Space
//         %x09 /               ; Horizontal tab
//         %x0A /               ; Line feed or New line
//         %x0D )               ; Carriage return
//
static inline bool isws(const int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static void skip_whitespace(struct parser *p)
{
    while (isws(PEEK(p)))
        READ(p);
}

// object = begin-object [ member *( value-separator member ) ]
//          end-object
// member = string name-separator value
//
// begin-object    = ws %x7B ws         ; {
// end-object      = ws %x7D ws         ; }
// name-separator  = ws %x3A ws         ; :
// value-separator = ws %x2C ws         ; ,
//
static int parse_object(struct parser *p, jsean *json)
{
    jsean name, value;
    int ret;

    jsean_set_obj(json);

    READ(p);

    skip_whitespace(p);
    if (PEEK(p) == '}')
        goto end;

#define PARSE_MEMBER_HELPER()                  \
    ret = parse_string(p, &name);              \
    if (ret != JSEAN_SUCCESS)                  \
        goto err;                              \
                                               \
    skip_whitespace(p);                        \
    if (READ(p) != ':')                        \
        goto err_name;                         \
    skip_whitespace(p);                        \
                                               \
    ret = parse_value(p, &value);              \
    if (ret != JSEAN_SUCCESS)                  \
        goto err_name;                         \
    if (!jsean_obj_set(json, &name, &value)) { \
        ret = JSEAN_OUT_OF_MEMORY;             \
        goto err_value;                        \
    }

    PARSE_MEMBER_HELPER();

    while (true) {
        skip_whitespace(p);
        if (PEEK(p) == '}')
            goto end;

        if (READ(p) != ',') {
            ret = JSEAN_EXPECTED_COMMA;
            goto err;
        }
        skip_whitespace(p);

        PARSE_MEMBER_HELPER();
    }

end:
    READ(p);
    return JSEAN_SUCCESS;

err_value:
    jsean_free(&value);
err_name:
    jsean_free(&name);
err:
    jsean_free(json);
    return ret;

#undef PARSE_MEMBER_HELPER
}

// array = begin-array [ value *( value-separator value ) ] end-array
//
// begin-array = ws %x5B ws             ; [
// end-array = ws %x5D ws               ; ]
// value-separator = ws %x2C ws         ; ,
//
static int parse_array(struct parser *p, jsean *json)
{
    jsean value;
    int ret;

    jsean_set_arr(json);

    READ(p);

    skip_whitespace(p);
    if (PEEK(p) == ']')
        goto end;

#define PARSE_VALUE_HELPER()             \
    ret = parse_value(p, &value);        \
    if (ret != JSEAN_SUCCESS)            \
        goto err;                        \
                                         \
    if (!jsean_arr_push(json, &value)) { \
        ret = JSEAN_OUT_OF_MEMORY;       \
        goto err_value;                  \
    }

    PARSE_VALUE_HELPER();

    while (true) {
        skip_whitespace(p);
        if (PEEK(p) == ']')
            break;

        if (READ(p) != ',') {
            ret = JSEAN_EXPECTED_COMMA;
            goto err;
        }
        skip_whitespace(p);

        PARSE_VALUE_HELPER();
    }

end:
    READ(p);
    return JSEAN_SUCCESS;

err_value:
    jsean_free(&value);
err:
    jsean_free(json);
    return ret;

#undef PARSE_VALUE_HELPER
}

// number = [ minus ] int [ frac ] [ exp ]
//
// decimal-point = %x2E                 ; .
// digit1-9 = %x31-39                   ; 1-9
// e = %x65 / %x45                      ; e E
// exp = e [ minus / plus ] 1*DIGIT
// frac = decimal-point 1*DIGIT
// int = zero / ( digit1-9 *DIGIT )
// minus = %x2D                         ; -
// plus = %x2B                          ; +
// zero = %x30                          ; 0
//
static int parse_number(struct parser *p, jsean *json)
{
    char *endptr;
    double num;

    strbuf_clear(&p->buf);

    if (PEEK(p) == '-')
        strbuf_add_byte(&p->buf, READ(p));

    if (PEEK(p) == '0') {
        strbuf_add_byte(&p->buf, READ(p));
    } else {
        if (!isnonzero(PEEK(p)))
            return JSEAN_EXPECTED_NONZERO_DIGIT;
        strbuf_add_byte(&p->buf, READ(p));

        while (isdec(PEEK(p)))
            strbuf_add_byte(&p->buf, READ(p));
    }

    if (p->buf.len == 0)
        return JSEAN_EXPECTED_MINUS_NONZERO_DIGIT;

    if (PEEK(p) == '.') {
        strbuf_add_byte(&p->buf, READ(p));

        if (!isdec(PEEK(p)))
            return JSEAN_EXPECTED_DIGIT;
        strbuf_add_byte(&p->buf, READ(p));

        while (isdec(PEEK(p)))
            strbuf_add_byte(&p->buf, READ(p));
    }

    if (PEEK(p) == 'e' || PEEK(p) == 'E') {
        strbuf_add_byte(&p->buf, READ(p));

        if (PEEK(p) == '-' || PEEK(p) == '+')
            strbuf_add_byte(&p->buf, READ(p));

        if (!isdec(PEEK(p)))
            return JSEAN_EXPECTED_DIGIT;
        strbuf_add_byte(&p->buf, READ(p));

        while (isdec(PEEK(p)))
            strbuf_add_byte(&p->buf, READ(p));
    }

    strbuf_add_byte(&p->buf, '\0');

    num = strtod(p->buf.data, &endptr);
    if (&p->buf.data[p->buf.len - 1] != endptr)
        return JSEAN_CONVERSION_FAILED;

    jsean_set_num(json, num);

    return JSEAN_SUCCESS;
}

// Returns the code point, or -1 for invalid sequences
static int parse_unicode_escape_sequence(struct parser *p)
{
    static const int powers[] = { 4096, 256, 16, 1 };
    int cp1, cp2;

    if (PEEK(p) != 'u')
        return -1;
    READ(p);

    cp1 = 0;
    for (int i = 0; i < 4; i++) {
        if (!ishex(PEEK(p)))
            return -1;
        cp1 += hextoi(READ(p)) * powers[i];
    }

    // Not a UTF-16 surrogate pair
    if (cp1 < 0xd800 || cp1 > 0xdfff)
        return cp1;

    // Surrogate pair not followed by another pair
    if (PEEK(p) != '\\')
        return -1;
    READ(p);
    if (PEEK(p) != 'u')
        return -1;
    READ(p);

    cp2 = 0;
    for (int i = 0; i < 4; i++) {
        if (!ishex(PEEK(p)))
            return -1;
        cp2 += hextoi(READ(p)) * powers[i];
    }

    // Expected a surrogate pair
    if (cp2 < 0xd800 || cp2 > 0xdfff)
        return -1;

    // https://en.wikipedia.org/wiki/UTF-16#Examples
    return ((cp1 - 0xd800) * 0x400) + (cp2 - 0xdc00) + 0x10000;
}

static int parse_utf8_sequence(struct parser *p)
{
    char buf[4];
    int tmp, len;

#define A(i, m, x) ((buf[i] & m) == x)
#define B(i, m, x) ((buf[i] & m) << x)

    len = 0;
    buf[len++] = READ(p);

    // 0xxx xxxx
    if (A(0, 0x80, 0x0))
        goto end;

    buf[len++] = READ(p);

    // 110xx xxx 10xx xxxx
    if (A(0, 0xe0, 0xc0) && A(1, 0xc0, 0x80)) {
        tmp = B(0, 0x1f, 6) | B(1, 0x3f, 0);
        if (tmp < 0x80)
            return JSEAN_INVALID_UTF8_SEQUENCE;

        goto end;
    }

    buf[len++] = READ(p);

    // 1110 xxxx 10xx xxxx 10xx xxxx
    if (A(0, 0xf0, 0xe0) && A(1, 0xc0, 0x80) && A(2, 0xc0, 0x80)) {
        tmp = B(0, 0xf, 12) | B(1, 0x3f, 6) | B(2, 0x3f, 0);
        if (tmp < 0x800 || (tmp >= 0xd800 && tmp <= 0xdfff))
            return JSEAN_INVALID_UTF8_SEQUENCE;

        goto end;
    }

    buf[len++] = READ(p);

    // 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
    if (A(0, 0xf8, 0xf0) && A(1, 0xc0, 0x80) && A(2, 0xc0, 0x80)  && A(3, 0xc0, 0x80)) {
        tmp = B(0, 0x7, 18) | B(1, 0x3f, 12) | B(2, 0x3f, 6) | B(3, 0x3f, 0);
        if (tmp < 0x10000 || tmp > 0x10ffff)
            return JSEAN_INVALID_UTF8_SEQUENCE;
    }

#undef A
#undef B

end:
    if (!strbuf_add_bytes(&p->buf, buf, len))
        return JSEAN_OUT_OF_MEMORY;

    return JSEAN_SUCCESS;
}

// string = quotation-mark *char quotation-mark
//
// char = unescaped /
//     escape (
//         %x22 /               ; "    quotation mark  U+0022
//         %x5C /               ; \    reverse solidus U+005C
//         %x2F /               ; /    solidus         U+002F
//         %x62 /               ; b    backspace       U+0008
//         %x66 /               ; f    form feed       U+000C
//         %x6E /               ; n    line feed       U+000A
//         %x72 /               ; r    carriage return U+000D
//         %x74 /               ; t    tab             U+0009
//         %x75 4HEXDIG )       ; uXXXX                U+XXXX
//
// escape = %x5C                ; \     :)
// quotation-mark = %x22        ; "
// unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
//
static int parse_string(struct parser *p, jsean *json)
{
    char *ptr;
    int ret;

    strbuf_clear(&p->buf);

    if (PEEK(p) != '"')
        return JSEAN_EXPECTED_QUOTATION_MARK;
    READ(p);

    for (;;) {
        switch (PEEK(p)) {
        case -1:
            return JSEAN_EXPECTED_QUOTATION_MARK;

        case '"':
            READ(p);

            ptr = malloc(p->buf.len);
            if (!ptr)
                return JSEAN_OUT_OF_MEMORY;

            memcpy(ptr, p->buf.data, p->buf.len);
            jsean_set_str(json, ptr, p->buf.len, free);

            return JSEAN_SUCCESS;

        // ASCII characters
        case 0x20 ... 0x21:
        case 0x23 ... 0x5b:
        case 0x5d ... 0x7f:
            if (!strbuf_add_byte(&p->buf, READ(p)))
                return JSEAN_OUT_OF_MEMORY;
            break;

        // Escape characters
        case '\\':
            READ(p);
            switch (PEEK(p)) {
            case '"':  ret = '"';  READ(p); break;
            case '\\': ret = '\\'; READ(p); break;
            case '/':  ret = '/';  READ(p); break;
            case 'b':  ret = '\b'; READ(p); break;
            case 'f':  ret = '\f'; READ(p); break;
            case 'n':  ret = '\n'; READ(p); break;
            case 'r':  ret = '\r'; READ(p); break;
            case 't':  ret = '\t'; READ(p); break;
            case 'u':
                ret = parse_unicode_escape_sequence(p);
                if (ret == -1)
                    return JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE;
                break;

            default:
                return JSEAN_INVALID_ESCAPE_SEQUENCE;
            }

            if (!strbuf_add_codepoint(&p->buf, ret))
                return JSEAN_OUT_OF_MEMORY;
            break;

        // Non-ASCII characters
        default:
            ret = parse_utf8_sequence(p);
            if (ret != JSEAN_SUCCESS)
                return ret;

            break;
        }
    }
}

static bool parse_literal(struct parser *p, const char *str, const size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (PEEK(p) != str[i])
            return false;
        READ(p);
    }

    return true;
}

// value = false / null / true / object / array / number / string
//
static int parse_value(struct parser *p, jsean *json)
{
    switch (PEEK(p)) {
    case 'f':
        if (!parse_literal(p, "false", 5))
            return JSEAN_EXPECTED_FALSE;
        jsean_set_bool(json, false);
        break;

    case 'n':
        if (!parse_literal(p, "null", 4))
            return JSEAN_EXPECTED_NULL;
        jsean_set_null(json);
        break;

    case 't':
        if (!parse_literal(p, "true", 4))
            return JSEAN_EXPECTED_TRUE;
        jsean_set_bool(json, true);
        break;

    case '{':
        return parse_object(p, json);

    case '[':
        return parse_array(p, json);

    case '-':
    case '0' ... '9':
        return parse_number(p, json);

    case '"':
        return parse_string(p, json);

    default:
        return JSEAN_EXPECTED_VALUE;
    }

    return JSEAN_SUCCESS;
}

// JSON-text = ws value ws
//
static int parse_text(struct parser *p, jsean *json)
{
    int ret;

    skip_whitespace(p);
    ret = parse_value(p, json);
    if (ret != JSEAN_SUCCESS)
        return ret;

    skip_whitespace(p);
    if (PEEK(p) != -1)
        return JSEAN_EXPECTED_WHITESPACE;

    return JSEAN_SUCCESS;
}

int jsean_read(jsean *json, jsean *src)
{
    struct parser p;
    int ret;

    if (!json || jsean_get_type(src) != JSEAN_TYPE_STRING)
        return JSEAN_INVALID_ARGUMENTS;

    if (!strbuf_init(&p.buf))
        return JSEAN_OUT_OF_MEMORY;

    p.ptr = jsean_get_str(src);
    p.end = p.ptr + jsean_str_len(src);
    p.peek = peek_buffer;
    p.read = read_buffer;

    ret = parse_text(&p, json);

    strbuf_free(&p.buf);
    return ret;
}

int jsean_read_stream(jsean *json, FILE *fp)
{
    struct parser p;
    int ret;

    if (!json || !fp)
        return JSEAN_INVALID_ARGUMENTS;

    if (!strbuf_init(&p.buf))
        return JSEAN_OUT_OF_MEMORY;

    p.fp = fp;
    p.peek = peek_stream;
    p.read = read_stream;

    ret = parse_text(&p, json);

    strbuf_free(&p.buf);
    return ret;
}
