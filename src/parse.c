//
// Copyright (c) 2024, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsean/json.h>

#define SET_RETVAL_AND_GOTO(x, label) { retval = x; goto label; }

enum json_parse_result {
    JSON_PARSE_OK,
    JSON_PARSE_NOT_SUPPORTED,
    JSON_PARSE_MEMORY_ERROR,
    JSON_PARSE_EXPECTED_BEGIN_OBJECT,
    JSON_PARSE_EXPECTED_ARRAY_BEGIN,
    JSON_PARSE_EXPECTED_NAME_SEPARATOR,
    JSON_PARSE_EXPECTED_VALUE_SEPARATOR,
    JSON_PARSE_UNEXPECTED_END_OBJECT,
    JSON_PARSE_EXPECTED_QUOTATION_MARK,
    JSON_PARSE_INVALID_ESCAPED_UNICODE,
    JSON_PARSE_UNEXPECTED_EOF,
    JSON_PARSE_UNEXPECTED_CHARACTER,
    JSON_PARSE_EXPECTED_EOF,
    JSON_PARSE_EXPECTED_FALSE,
    JSON_PARSE_EXPECTED_NULL,
    JSON_PARSE_EXPECTED_TRUE,
    JSON_PARSE_UNEXPECTED_END_ARRAY,
    JSON_PARSE_EXPECTED_DIGIT,
};

const char *parse_result_to_string(int result)
{
    static const char *strings[] = {
        "OK",
        "Not supported",
        "Memory error",
        "Expected '{'",
        "Expected '['",
        "Expected ':'",
        "Expected ','",
        "Unexpected '}'",
        "Expected '\"'",
        "Invalid escaped unicode",
        "Unexpected EOF",
        "Unexpected character",
        "Expected EOF",
        "Expected \"false\"",
        "Expected \"null\"",
        "Expected \"true\"",
        "Unexpected ']'",
        "Expected int",
    };

    return strings[result];
}

struct reader
{
    const char *bytes;
    size_t index;
    size_t ln;
    size_t col;
};

static int read_string(struct reader *rd, char **out);

static int parse_value(struct reader *rd, struct json *out);
static int parse_object(struct reader *rd, struct json *out);
static int parse_array(struct reader *rd, struct json *out);
static int parse_number(struct reader *rd, struct json *out);
static int parse_string(struct reader *rd, struct json *out);

static inline char peek(struct reader *rd)
{
    return rd->bytes[rd->index];
}

static char read(struct reader *rd)
{
    char c = peek(rd);
    rd->index++;

    if (c == '\n') {
        rd->ln++;
        rd->col = 1;
    } else {
        rd->col++;
    }

    return c;
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
        return JSON_PARSE_EXPECTED_QUOTATION_MARK;

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
                        return JSON_PARSE_INVALID_ESCAPED_UNICODE;
                }
                break;

            default:
                return JSON_PARSE_UNEXPECTED_CHARACTER;
            }
            continue;
        }

        read(rd);
    }

    *out = malloc(rd->index - start + 1);
    if (*out == NULL)
        return JSON_PARSE_MEMORY_ERROR;

    memcpy(*out, rd->bytes + start, rd->index - start);
    (*out)[rd->index - start] = '\0';

    read(rd); // Skip '"'
    return JSON_PARSE_OK;
}

static int parse_value(struct reader *rd, struct json *out)
{
    switch (peek(rd)) {
    case 'f':
        if (strncmp(&rd->bytes[rd->index], "false", 5) != 0)
            return JSON_PARSE_EXPECTED_FALSE;
        rd->index += 5;
        json_init_boolean(out, false);
        break;

    case 'n':
        if (strncmp(&rd->bytes[rd->index], "null", 4) != 0)
            return JSON_PARSE_EXPECTED_NULL;
        rd->index += 4;
        json_init_null(out);
        break;

    case 't':
        if (strncmp(&rd->bytes[rd->index], "true", 4) != 0)
            return JSON_PARSE_EXPECTED_TRUE;
        rd->index += 4;
        json_init_boolean(out, true);
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
        return JSON_PARSE_UNEXPECTED_CHARACTER;
    }

    return JSON_PARSE_OK;
}

static int parse_object(struct reader *rd, struct json *out)
{
    struct json value = {0};
    char *key = NULL;
    int retval;

    skip_whitespace(rd);
    if (read(rd) != '{')
        return JSON_PARSE_EXPECTED_BEGIN_OBJECT;

    if ((retval = json_init_object(out, 0)) != 0)
        return retval;

    while (1) {
        skip_whitespace(rd);
        if (peek(rd) == '}')
            break;

        skip_whitespace(rd);
        if ((retval = read_string(rd, &key)) != JSON_PARSE_OK)
            goto fail;

        skip_whitespace(rd);
        if (read(rd) != ':')
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_NAME_SEPARATOR, fail);

        skip_whitespace(rd);
        value.type = JSON_TYPE_UNKNOWN;
        if ((retval = parse_value(rd, &value)) != JSON_PARSE_OK)
            goto fail;

        if ((retval = json_internal_object_insert(out, key, &value)) != 0)
            goto fail;
        key = NULL; // @out took ownership of @key, so let's not accidentally free it

        skip_whitespace(rd);
        if (peek(rd) == '}')
            break;
        if (read(rd) != ',')
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_VALUE_SEPARATOR, fail);

        skip_whitespace(rd);
        if (peek(rd) == '}')
            SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_END_OBJECT, fail);
    }

    read(rd); // Skip '}'
    skip_whitespace(rd);
    return retval;
fail:
    free(key);
    json_free(out);
    return retval;
}

static int parse_array(struct reader *rd, struct json *out)
{
    struct json value = {0};
    int retval;

    skip_whitespace(rd);
    if (read(rd) != '[')
        return JSON_PARSE_EXPECTED_ARRAY_BEGIN;

    if ((retval = json_init_array(out, 0)) != 0)
        return retval;

    while (1) {
        skip_whitespace(rd);
        if (peek(rd) == ']')
            break;

        skip_whitespace(rd);
        value.type = JSON_TYPE_UNKNOWN;
        if ((retval = parse_value(rd, &value)) != JSON_PARSE_OK)
            goto fail;

        if ((retval = json_array_push(out, &value)) != 0)
            goto fail;

        skip_whitespace(rd);
        if (peek(rd) == ']')
            break;
        if (read(rd) != ',')
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_VALUE_SEPARATOR, fail);

        skip_whitespace(rd);
        if (peek(rd) == ']')
            SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_END_ARRAY, fail);
    }

    read(rd); // Skip ']'
    skip_whitespace(rd);
    return retval;
fail:
    json_free(out);
    return retval;
}

static int parse_number(struct reader *rd, struct json *out)
{
    size_t start, tmp;
    bool has_minus = false;
    bool has_frac = false;

    start = rd->index;

    // Minus part (optional)
    if (peek(rd) == '-') {
        has_minus = true;
        read(rd);
    }

    // Integer part
    if (peek(rd) == '0') {
        read(rd);
    } else {
        while ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);
        if (start == rd->index)
            return JSON_PARSE_EXPECTED_DIGIT;
    }

    // Fraction part (optional)
    if (peek(rd) == '.') {
        has_frac = true;
        read(rd);

        tmp = rd->index;
        while ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);

        if (tmp == rd->index)
            return JSON_PARSE_EXPECTED_DIGIT;
    }

    if (!has_frac)
        goto integer;

    // Exponent part (requires fraction part)
    if (peek(rd) == 'e' || peek(rd) == 'E') {
        read(rd);

        if (peek(rd) == '-' || peek(rd) == '+')
            read(rd);

        tmp = rd->index;
        while ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);

        if (tmp == rd->index)
            return JSON_PARSE_EXPECTED_DIGIT;
    }

    json_set_double(out, strtod(&rd->bytes[start], NULL));
    return JSON_PARSE_OK;

integer:
    if (has_minus)
        json_set_signed(out, strtoll(&rd->bytes[start], NULL, 10));
    else
        json_set_unsigned(out, strtoull(&rd->bytes[start], NULL, 10));
    return JSON_PARSE_OK;
}

static int parse_string(struct reader *rd, struct json *out)
{
    char *string;
    int retval;

    if ((retval = read_string(rd, &string)) != JSON_PARSE_OK)
        return retval;

    *out = json_new_string_without_copy(string);
    return JSON_PARSE_OK;
}

int json_parse(struct json *json, const char *bytes)
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

    printf("Status: %s (Ln %zu, Col %zu)\n",
        parse_result_to_string(retval), rd.ln, rd.col);

    return retval;
}
