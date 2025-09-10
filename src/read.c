//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#include <stdbool.h>
#include <stdlib.h>

#include "jsean/jsean.h"

struct parser {
    const char *ptr, *end;
};

static inline bool isdec(const char c)
{
    return c >= '0' && c <= '9';
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

static int parse_keyword_helper(
    struct parser *p, const char *str, const size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (peek(p) != str[i])
            return JSEAN_UNEXPECTED_CHARACTER;
        read(p);
    }

    return JSEAN_SUCCESS;
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

// value = false / null / true / object / array / number / string
//
static int parse_value(struct parser *p, jsean *json)
{
    int retval;

    switch (peek(p)) {
    case 'f':
        if ((retval = parse_keyword_helper(p, "false", 5)) != JSEAN_SUCCESS)
            return JSEAN_EXPECTED_FALSE;
        jsean_set_boolean(json, false);
        break;

    case 'n':
        if ((retval = parse_keyword_helper(p, "null", 4)) != JSEAN_SUCCESS)
            return JSEAN_EXPECTED_NULL;
        jsean_set_null(json);
        break;

    case 't':
        if ((retval = parse_keyword_helper(p, "true", 4)) != JSEAN_SUCCESS)
            return JSEAN_EXPECTED_TRUE;
        jsean_set_boolean(json, true);
        break;

    case '-':
    case '0' ... '9':
        return parse_number(p, json);

    default:
        return JSEAN_UNEXPECTED_CHARACTER;
    }

    return JSEAN_SUCCESS;
}

enum jsean_status jsean_read(jsean *json, const char *bytes, const size_t len)
{
    struct parser p;
    int retval;

    if (!json || !bytes || !len)
        return JSEAN_INVALID_ARGUMENTS;

    p.ptr = bytes;
    p.end = p.ptr + len;

    // JSON-text = ws value ws
    //
    skip_whitespace(&p);
    if ((retval = parse_value(&p, json)) != JSEAN_SUCCESS)
        return retval;
    skip_whitespace(&p);
    if (peek(&p) != -1)
        return JSEAN_UNEXPECTED_CHARACTER;

    return JSEAN_SUCCESS;
}
