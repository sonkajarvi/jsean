//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/jsean.h>

#define SET_RETVAL_AND_GOTO(x, label) { retval = x; goto label; }

const char *jsean_parse_result_to_string(int result)
{
    static const char *s[] = {
#define F(_, s) s,
        JSEAN_STATUS_LIST(F)
#undef F
    };

    if (result < 0 || result >= JSEAN_STATUS_COUNT - 1)
        result = JSEAN_UNKNOWN_ERROR;

    return s[result];
}

struct reader
{
    const char *bytes;
    const size_t size;
    size_t index;
};

static int read_string(struct reader *rd, char **out);

static int parse_value(struct reader *rd, jsean_t *out);
static int parse_object(struct reader *rd, jsean_t *out);
static int parse_array(struct reader *rd, jsean_t *out);
static int parse_number(struct reader *rd, jsean_t *out);
static int parse_string(struct reader *rd, jsean_t *out);

static inline int peek_offset(struct reader *rd, const size_t offset)
{
    return rd->index + offset < rd->size
        ? rd->bytes[rd->index + offset]
        : -1;
}

static inline int peek(struct reader *rd)
{
    return peek_offset(rd, 0);
}

static inline int read(struct reader *rd)
{
    int c = peek(rd);
    rd->index++;
    return c;
}

static inline const char *curr(struct reader *rd)
{
    return &rd->bytes[rd->index];
}

static inline bool iseof(struct reader *rd)
{
    return rd->index >= rd->size;
}

static void skip_whitespace(struct reader *rd)
{
    char c;
    while ((c = peek(rd)) == ' ' || c == '\t' || c == '\n' || c == '\r')
        read(rd);
}

static inline bool is_hex(const char c)
{
    return ('0' <= c && c <= '9') ||
           ('a' <= c && c <= 'f') ||
           ('A' <= c && c <= 'F');
}

static int read_string(struct reader *rd, char **out)
{
    size_t start;
    char c;

    if (read(rd) != '"')
        return JSEAN_EXPECTED_QUOTATION_MARK;

    start = rd->index;
    while ((c = peek(rd)) != '"') {
        if (c == '\\') {
            read(rd); // Skip '\'
            switch (peek(rd)) {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                read(rd);
                break;

            case 'u':
                read(rd); // Skip 'u'
                for (int i = 0; i < 4; i++) {
                    if (!is_hex(read(rd)))
                        return JSEAN_INVALID_UNICODE_ESCAPE;
                }
                break;

            default:
                return JSEAN_UNEXPECTED_CHARACTER;
            }
            continue;
        }

        read(rd);
    }

    *out = malloc(rd->index - start + 1);
    if (*out == NULL)
        return JSEAN_MEMORY_ERROR;

    memcpy(*out, rd->bytes + start, rd->index - start);
    (*out)[rd->index - start] = '\0';

    read(rd); // Skip '"'
    return JSEAN_OK;
}

static int parse_value(struct reader *rd, jsean_t *out)
{
    switch (peek(rd)) {
    case 'f':
        if (strncmp(&rd->bytes[rd->index], "false", 5) != 0)
            return JSEAN_EXPECTED_FALSE;
        rd->index += 5;
        jsean_set_boolean(out, false);
        break;

    case 'n':
        if (strncmp(&rd->bytes[rd->index], "null", 4) != 0)
            return JSEAN_EXPECTED_NULL;
        rd->index += 4;
        jsean_set_null(out);
        break;

    case 't':
        if (strncmp(&rd->bytes[rd->index], "true", 4) != 0)
            return JSEAN_EXPECTED_TRUE;
        rd->index += 4;
        jsean_set_boolean(out, true);
        break;

    case '{':
        return parse_object(rd, out);

    case '[':
        return parse_array(rd, out);

    case '-':
    case '0' ... '9':
        return parse_number(rd, out);

    case '"':
        return parse_string(rd, out);

    default:
        return JSEAN_UNEXPECTED_CHARACTER;
    }

    return JSEAN_OK;
}

static int parse_object(struct reader *rd, jsean_t *out)
{
    jsean_t value = {0};
    char *key = NULL;
    int retval;

    skip_whitespace(rd);
    if (read(rd) != '{')
        return JSEAN_EXPECTED_BEGIN_OBJECT;

    if ((retval = jsean_set_object(out)) != 0)
        return retval;

    while (1) {
        skip_whitespace(rd);
        if (peek(rd) == '}')
            break;

        skip_whitespace(rd);
        if ((retval = read_string(rd, &key)) != JSEAN_OK)
            goto fail;

        skip_whitespace(rd);
        if (read(rd) != ':')
            SET_RETVAL_AND_GOTO(JSEAN_EXPECTED_NAME_SEPARATOR, fail);

        skip_whitespace(rd);
        value.type = JSEAN_TYPE_UNKNOWN;
        if ((retval = parse_value(rd, &value)) != JSEAN_OK)
            goto fail;

        if ((retval = jsean_internal_object_overwrite(out, key, &value)) != 0)
            goto fail;

        key = NULL;

        skip_whitespace(rd);
        if (peek(rd) == '}')
            break;
        if (read(rd) != ',')
            SET_RETVAL_AND_GOTO(JSEAN_EXPECTED_VALUE_SEPARATOR, fail);

        skip_whitespace(rd);
        if (peek(rd) == '}')
            SET_RETVAL_AND_GOTO(JSEAN_UNEXPECTED_END_OBJECT, fail);
    }

    read(rd); // Skip '}'
    skip_whitespace(rd);
    return retval;

fail:
    free(key);
    jsean_free(out);
    return retval;
}

static int parse_array(struct reader *rd, jsean_t *out)
{
    jsean_t value = {0};
    int retval;

    skip_whitespace(rd);
    if (read(rd) != '[')
        return JSEAN_EXPECTED_ARRAY_BEGIN;

    if ((retval = jsean_set_array(out, 0)) != 0)
        return retval;

    while (1) {
        skip_whitespace(rd);
        if (peek(rd) == ']')
            break;

        skip_whitespace(rd);
        value.type = JSEAN_TYPE_UNKNOWN;
        if ((retval = parse_value(rd, &value)) != JSEAN_OK)
            goto fail;

        if ((retval = jsean_array_push(out, &value)) != 0)
            goto fail;

        skip_whitespace(rd);
        if (peek(rd) == ']')
            break;
        if (read(rd) != ',')
            SET_RETVAL_AND_GOTO(JSEAN_EXPECTED_VALUE_SEPARATOR, fail);

        skip_whitespace(rd);
        if (peek(rd) == ']')
            SET_RETVAL_AND_GOTO(JSEAN_UNEXPECTED_END_ARRAY, fail);
    }

    read(rd); // Skip ']'
    skip_whitespace(rd);
    return retval;
fail:
    jsean_free(out);
    return retval;
}

static int parse_number(struct reader *rd, jsean_t *out)
{
    size_t start, tmp;

    start = rd->index;

    // Minus part (optional)
    if (peek(rd) == '-') {
        read(rd);
    }

    // Integer part
    if (peek(rd) == '0') {
        read(rd);
    } else {
        while ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);
        if (start == rd->index)
            return JSEAN_EXPECTED_DIGIT;
    }

    // Fraction part (optional)
    if (peek(rd) == '.') {
        read(rd);

        tmp = rd->index;
        while ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);

        if (tmp == rd->index)
            return JSEAN_EXPECTED_DIGIT;
    }

    // Exponent part (optional)
    if (peek(rd) == 'e' || peek(rd) == 'E') {
        read(rd);

        if (peek(rd) == '-' || peek(rd) == '+')
            read(rd);

        tmp = rd->index;
        while (peek(rd) >= '0' && peek(rd) <= '9')
            read(rd);

        if (tmp == rd->index)
            return JSEAN_EXPECTED_DIGIT;
    }

    jsean_set_number(out, strtod(&rd->bytes[start], NULL));
    return JSEAN_OK;
}

static int parse_string(struct reader *rd, jsean_t *out)
{
    char *string;
    int retval;

    if ((retval = read_string(rd, &string)) != JSEAN_OK)
        return retval;

    jsean_move_string(out, string);
    return JSEAN_OK;
}

int json_parse(jsean_t *json, const char *bytes)
{
    int retval;

    struct reader rd = {
        .bytes = bytes,
        .index = 0,
        .ln = 1,
        .col = 1
    };

    skip_whitespace(&rd);
    retval = parse_value(&rd, json);

    // printf("%s (Ln %zu, Col %zu)\n", parse_result_to_string(retval), rd.ln, rd.col);

    return retval;
}
