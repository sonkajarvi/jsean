//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "jsean/jsean.h"
#include "internal.h"

struct parser {
    struct strbuf buf;
    const char *ptr, *end;
};

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

static inline int peek(struct parser *p)
{
    if (p->ptr >= p->end)
        return -1;

    return *p->ptr;
}

static int read(struct parser *p)
{
    int c;

    if ((c = peek(p)) == -1)
        return -1;
    p->ptr++;

    return c;
}

static void skip_whitespace(struct parser *p)
{
    while (isws(peek(p)))
        read(p);
}

// Returns the number of bytes read or -1
static int is_valid_utf8(const char *ptr)
{
    int tmp;

#define A(i, m, x) ((ptr[i] & m) == x)
#define B(i, m, x) ((ptr[i] & m) << x)

    // 0xxx xxxx
    if (A(0, 0x80, 0x0))
        return 1;

    // 110xx xxx 10xx xxxx
    if (A(0, 0xe0, 0xc0) && A(1, 0xc0, 0x80)) {
        tmp = B(0, 0x1f, 6) | B(1, 0x3f, 0);
        if (tmp < 0x80)
            goto fail;

        return 2;
    }

    // 1110 xxxx 10xx xxxx 10xx xxxx
    if (A(0, 0xf0, 0xe0) && A(1, 0xc0, 0x80) && A(2, 0xc0, 0x80)) {
        tmp = B(0, 0xf, 12) | B(1, 0x3f, 6) | B(2, 0x3f, 0);
        if (tmp < 0x800 || (tmp >= 0xd800 && tmp <= 0xdfff))
            goto fail;

        return 3;
    }

    // 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
    if (A(0, 0xf8, 0xf0) && A(1, 0xc0, 0x80) && A(2, 0xc0, 0x80)  && A(3, 0xc0, 0x80)) {
        tmp = B(0, 0x7, 18) | B(1, 0x3f, 12) | B(2, 0x3f, 6) | B(3, 0x3f, 0);
        if (tmp < 0x10000 || tmp > 0x10ffff)
            goto fail;

        return 4;
    }

#undef A
#undef B

fail:
    return -1;
}

static bool parse_keyword(struct parser *p, const char *str, const size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (peek(p) != str[i])
            return false;
        read(p);
    }

    return true;
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

    jsean_set_object(json);

    read(p);

    skip_whitespace(p);
    if (peek(p) == '}')
        goto ret;

#define PARSE_MEMBER_HELPER()                                        \
    ret = parse_string(p, &name);                                    \
    if (ret != JSEAN_SUCCESS)                                        \
        goto err;                                                    \
                                                                     \
    skip_whitespace(p);                                              \
    if (read(p) != ':')                                              \
        goto err_name;                                               \
    skip_whitespace(p);                                              \
                                                                     \
    ret = parse_value(p, &value);                                    \
    if (ret != JSEAN_SUCCESS)                                        \
        goto err_name;                                               \
    if (!jsean_object_insert(json, jsean_get_string(&name), &value)) \
        goto err_value;                                              \
    jsean_free(&name);

    PARSE_MEMBER_HELPER();

    while (1) {
        skip_whitespace(p);
        if (peek(p) == '}')
            goto ret;

        if (read(p) != ',') {
            ret = JSEAN_EXPECTED_COMMA;
            goto err;
        }
        skip_whitespace(p);

        PARSE_MEMBER_HELPER();
    }

ret:
    read(p);
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

    jsean_set_array(json);

    read(p);

    skip_whitespace(p);
    if (peek(p) == ']')
        goto ret;

#define PARSE_VALUE_HELPER()               \
    ret = parse_value(p, &value);          \
    if (ret != JSEAN_SUCCESS)              \
        goto err;                          \
                                           \
    if (!jsean_array_push(json, &value)) { \
        ret = JSEAN_OUT_OF_MEMORY;         \
        goto err_value;                    \
    }

    PARSE_VALUE_HELPER();

    while (1) {
        skip_whitespace(p);
        if (peek(p) == ']')
            break;

        if (read(p) != ',') {
            ret = JSEAN_EXPECTED_COMMA;
            goto err;
        }
        skip_whitespace(p);

        PARSE_VALUE_HELPER();
    }

ret:
    read(p);
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
    const char *ptr;
    char *endptr;
    double num;

    ptr = p->ptr;
    if (peek(p) == '-')
        read(p);

    if (peek(p) == '0') {
        read(p);
    } else {
        if (!isnonzero(peek(p)))
            return JSEAN_EXPECTED_NONZERO_DIGIT;
        read(p);

        while (isdec(peek(p)))
            read(p);
    }

    if (ptr == p->ptr)
        return JSEAN_EXPECTED_MINUS_NONZERO_DIGIT;

    if (peek(p) == '.') {
        read(p);

        if (!isdec(peek(p)))
            return JSEAN_EXPECTED_DIGIT;
        read(p);

        while (isdec(peek(p)))
            read(p);
    }

    if (peek(p) == 'e' || peek(p) == 'E') {
        read(p);

        if (peek(p) == '-' || peek(p) == '+')
            read(p);

        if (!isdec(peek(p)))
            return JSEAN_EXPECTED_DIGIT;
        read(p);

        while (isdec(peek(p)))
            read(p);
    }

    num = strtod(ptr, &endptr);
    if (p->ptr != endptr)
        return JSEAN_CONVERSION_FAILED;
    jsean_set_number(json, num);

    return JSEAN_SUCCESS;
}

// Returns -1 for invalid sequences
static int parse_unicode_escape_sequence(struct parser *p)
{
    static const int powers[] = { 4096, 256, 16, 1 };
    int cp1, cp2;

    if (peek(p) != 'u')
        return -1;
    read(p);

    cp1 = 0;
    for (int i = 0; i < 4; i++) {
        if (!ishex(peek(p)))
            return -1;
        cp1 += hextoi(read(p)) * powers[i];
    }

    // Not a UTF-16 surrogate pair
    if (cp1 < 0xd800 || cp1 > 0xdfff)
        return cp1;

    // Surrogate pair not followed by another pair
    if (peek(p) != '\\')
        return -1;
    read(p);
    if (peek(p) != 'u')
        return -1;
    read(p);

    cp2 = 0;
    for (int i = 0; i < 4; i++) {
        if (!ishex(peek(p)))
            return -1;
        cp2 += hextoi(read(p)) * powers[i];
    }

    // Expected a surrogate pair
    if (cp2 < 0xd800 || cp2 > 0xdfff)
        return -1;

    // https://en.wikipedia.org/wiki/UTF-16#Examples
    return ((cp1 - 0xd800) * 0x400) + (cp2 - 0xdc00) + 0x10000;
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
    int tmp;

    strbuf_clear(&p->buf);

    if (peek(p) != '"')
        return JSEAN_EXPECTED_QUOTATION_MARK;
    read(p);

    for (;;) {
        switch (peek(p)) {
        case -1:
            return JSEAN_EXPECTED_QUOTATION_MARK;

        case '"':
            read(p);
            strbuf_add_byte(&p->buf, '\0');
            jsean_set_string(json, p->buf.data);
            return JSEAN_SUCCESS;

        // ASCII characters
        case 0x20 ... 0x21:
        case 0x23 ... 0x5b:
        case 0x5d ... 0x7f:
            if (!strbuf_add_byte(&p->buf, read(p)))
                return JSEAN_OUT_OF_MEMORY;
            break;

        // Escape characters
        case '\\':
            read(p);
            switch (peek(p)) {
            case '"':  tmp = '"';  read(p); break;
            case '\\': tmp = '\\'; read(p); break;
            case '/':  tmp = '/';  read(p); break;
            case 'b':  tmp = '\b'; read(p); break;
            case 'f':  tmp = '\f'; read(p); break;
            case 'n':  tmp = '\n'; read(p); break;
            case 'r':  tmp = '\r'; read(p); break;
            case 't':  tmp = '\t'; read(p); break;
            case 'u':
                tmp = parse_unicode_escape_sequence(p);
                if (tmp == -1)
                    return JSEAN_INVALID_UNICODE_ESCAPE_SEQUENCE;
                break;
            default:
                return JSEAN_INVALID_ESCAPE_SEQUENCE;
            }
            if (!strbuf_add_codepoint(&p->buf, tmp))
                return JSEAN_OUT_OF_MEMORY;
            break;

        // Non-ASCII characters
        default:
            tmp = is_valid_utf8(p->ptr);
            if (tmp == -1)
                return JSEAN_INVALID_UTF8;
            if (p->ptr + tmp >= p->end)
                return JSEAN_INVALID_UTF8;

            if (!strbuf_add_bytes(&p->buf, p->ptr, tmp))
                return JSEAN_OUT_OF_MEMORY;
            p->ptr += tmp;
            break;
        }
    }
}

// value = false / null / true / object / array / number / string
//
static int parse_value(struct parser *p, jsean *json)
{
    switch (peek(p)) {
    case 'f':
        if (!parse_keyword(p, "false", 5))
            return JSEAN_EXPECTED_FALSE;
        jsean_set_boolean(json, false);
        break;

    case 'n':
        if (!parse_keyword(p, "null", 4))
            return JSEAN_EXPECTED_NULL;
        jsean_set_null(json);
        break;

    case 't':
        if (!parse_keyword(p, "true", 4))
            return JSEAN_EXPECTED_TRUE;
        jsean_set_boolean(json, true);
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

int jsean_read(jsean *json, const char *bytes, const size_t len)
{
    struct parser p;
    int ret;

    if (!json || !bytes)
        return JSEAN_INVALID_ARGUMENTS;

    if (!strbuf_init(&p.buf))
        return JSEAN_OUT_OF_MEMORY;

    p.ptr = bytes;
    p.end = p.ptr + len;

    // JSON-text = ws value ws
    skip_whitespace(&p);
    ret = parse_value(&p, json);
    if (ret != JSEAN_SUCCESS)
        goto fail;

    skip_whitespace(&p);
    if (peek(&p) != -1) {
        ret = JSEAN_EXPECTED_WHITESPACE;
        goto fail;
    }

    ret = JSEAN_SUCCESS;
fail:
    strbuf_free(&p.buf);
    return ret;
}
