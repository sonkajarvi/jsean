//
// Copyright (c) 2024, sonkajarvi
//

//
// JSON-text = ws value ws
//
// begin-array     = ws %x5B ws  ; [ left square bracket
// begin-object    = ws %x7B ws  ; { left curly bracket
// end-array       = ws %x5D ws  ; ] right square bracket
// end-object      = ws %x7D ws  ; } right curly bracket
// name-separator  = ws %x3A ws  ; : colon
// value-separator = ws %x2C ws  ; , comma
//
// ws = *(
//         %x20 /              ; Space
//         %x09 /              ; Horizontal tab
//         %x0A /              ; Line feed or New line
//         %x0D )              ; Carriage return
//
// value = false / null / true / object / array / number / string
//
// false = %x66.61.6c.73.65   ; false
// null  = %x6e.75.6c.6c      ; null
// true  = %x74.72.75.65      ; true
//
// object = begin-object [ member *( value-separator member ) ]
//          end-object
//
// member = string name-separator value
//
// array = begin-array [ value *( value-separator value ) ] end-array
//
// number = [ minus ] int [ frac ] [ exp ]
//
// decimal-point = %x2E       ; .
// digit1-9 = %x31-39         ; 1-9
// e = %x65 / %x45            ; e E
// exp = e [ minus / plus ] 1*DIGIT
// frac = decimal-point 1*DIGIT
// int = zero / ( digit1-9 *DIGIT )
// minus = %x2D               ; -
// plus = %x2B                ; +
// zero = %x30                ; 0
//
// string = quotation-mark *char quotation-mark
//
// char = unescaped /
//     escape (
//         %x22 /          ; "    quotation mark  U+0022
//         %x5C /          ; \    reverse solidus U+005C
//         %x2F /          ; /    solidus         U+002F
//         %x62 /          ; b    backspace       U+0008
//         %x66 /          ; f    form feed       U+000C
//         %x6E /          ; n    line feed       U+000A
//         %x72 /          ; r    carriage return U+000D
//         %x74 /          ; t    tab             U+0009
//         %x75 4HEXDIG )  ; uXXXX                U+XXXX
//
// escape = %x5C              ; '\'
// quotation-mark = %x22      ; "
// unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "string.h"

#define PARSE_OK 0
#define PARSE_MEMORY_ERROR 1
#define PARSE_SYNTAX_ERROR 2
#define PARSE_SSCANF_ERROR 3

#define ERROR (1 + __COUNTER__)

#define STRING_INITIAL_CAPACITY 16

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
    JSON_PARSE_INVALID_UNICODE,
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
        "Invalid Unicode",
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

static int read_string(struct reader *rd, string *out);

// Also parses false, null and true
static int parse_value(struct reader *rd, json_t **out);

static int parse_object(struct reader *rd, json_t **out);
static int parse_array(struct reader *rd, json_t **out);
static int parse_number(struct reader *rd, json_t **out);
static int parse_string(struct reader *rd, json_t **out);

static inline int peek(struct reader *rd)
{
    return rd->bytes[rd->index];
}

static int read(struct reader *rd)
{
    int c = peek(rd);
    rd->index += !!c;

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
    int c;
    while ((c = peek(rd)) == 0x20 || c == 0x09 || c == 0x0a || c == 0x0d)
        read(rd);
}

static int byte_count(const int c)
{
    if ((c & 0x80) == 0x0)
        return 1;
    else if ((c & 0xe0) == 0xc0)
        return 2;
    else if ((c & 0xf0) == 0xe0)
        return 3;
    else if ((c & 0xf8) == 0xf0)
        return 4;
    return 0;
}

static int read_string(struct reader *rd, string *out)
{
    char *tmp;
    int c, bc, retval = JSON_PARSE_OK;

    if (read(rd) != '"')
        return JSON_PARSE_EXPECTED_QUOTATION_MARK;

    while ((c = peek(rd) & 0xff) != '"') {
        switch (c) {
        case 0x20 ... 0x21:
        case 0x23 ... 0x5b:
        case 0x5d ... 0xff:
            if ((bc = byte_count(c)) == 0)
                SET_RETVAL_AND_GOTO(JSON_PARSE_INVALID_UNICODE, end);
            while (bc--) {
                if ((c = read(rd)) == '\0')
                    SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_EOF, end);
                if (!string_append_char(out, c))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
            }
            break;

        case '\\':
            read(rd); // Skip '\\'

            switch (peek(rd)) {
            case '"':
                if (!string_append_char(out, '"'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case '\\':
                if (!string_append_char(out, '\\'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case '/':
                if (!string_append_char(out, '/'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case 'b':
                if (!string_append_char(out, '\b'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case 'f':
                if (!string_append_char(out, '\f'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case 'n':
                if (!string_append_char(out, '\n'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case 'r':
                if (!string_append_char(out, '\r'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;
            case 't':
                if (!string_append_char(out, '\t'))
                    SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
                break;

            case 'u':
                    SET_RETVAL_AND_GOTO(JSON_PARSE_NOT_SUPPORTED, end);

            default:
                retval = JSON_PARSE_UNEXPECTED_CHARACTER;
                goto end;
            }

            read(rd);
            break;

        default:
            printf("%x\n", c);
            retval = JSON_PARSE_UNEXPECTED_CHARACTER;
            goto end;
        }
    }

    read(rd); // Skip '"'

end:
    if (retval != JSON_PARSE_OK) {
        string_free(out);
    } else if (out->length < out->capacity) {
        if ((tmp = realloc(out->data, out->length + 1)) == NULL) {
            string_free(out);
            retval = JSON_PARSE_MEMORY_ERROR;
        } else {
            out->data = tmp;
        }
    }

    return retval;
}

static int parse_string(struct reader *rd, json_t **out)
{
    json_t *value;
    string string = {0};
    int retval = JSON_PARSE_OK;

    if ((retval = read_string(rd, &string)) != JSON_PARSE_OK)
        goto end;
    if ((value = json_new_string_without_copy(string.data)) == NULL)
        SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
    *out = value;
end:
    return retval;
}

static int parse_array(struct reader *rd, json_t **array)
{
    json_t *value;
    int retval = JSON_PARSE_OK;

    skip_whitespace(rd);
    if (read(rd) != '[')
        SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_ARRAY_BEGIN, fail);

    if ((*array = json_new_array()) == NULL)
        SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);

    for (;;) {
        skip_whitespace(rd);
        if (peek(rd) == ']')
            break;

        skip_whitespace(rd);
        if ((retval = parse_value(rd, &value)) != JSON_PARSE_OK)
            goto fail;

        if (json_array_push(*array, value) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);

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
    json_free(*array);
    return retval;
}

static int parse_number(struct reader *rd, json_t **number)
{
    union {
        uint64_t u;
        int64_t i;
        double d;
    } data;
    size_t start;
    bool has_minus = false;
    bool has_frac = false;
    bool has_exp = false;
    int c, retval = JSON_PARSE_OK;

    // Minus part (optional)
    if (peek(rd) == '-') {
        has_minus = true;
        read(rd);
    }

    // Zero
    start = rd->index;
    if (peek(rd) == '0') {
        read(rd);
        goto frac;
    }

    // Integer part
    if ('1' <= peek(rd) && peek(rd) <= '9') {
        read(rd);

        while ((c = peek(rd))) {
            if ('0' <= c && c <= '9')
                read(rd);
            else
                break;
        }
    }
    if (start == rd->index)
        SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_DIGIT, fail);

frac:
    // Fraction part (optional)
    if (peek(rd) == '.') {
        read(rd);

        while ((c = peek(rd))) {
            if ('0' <= c && c <= '9') {
                has_frac = true;
                read(rd);
            } else {
                break;
            }
        }
    }
    if (!has_frac)
        goto number;

    // Exponent part (requires fraction part)
    if ((c = peek(rd)) == 'e' || c == 'E') {
        has_exp = true;
        read(rd);

        if ((c = peek(rd)) == '-' || c == '+') {
            read(rd);
        }

        if ('0' <= peek(rd) && peek(rd) <= '9')
            read(rd);
        else
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_DIGIT, fail);

        while ((c = peek(rd))) {
            if ('0' <= c && c <= '9')
                read(rd);
            else
                break;
        }
    }

number:
    if (has_exp) {
        if (!sscanf(&rd->bytes[start], "%le", &data.d))
            SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_CHARACTER, fail);
        if ((*number = json_new_double(data.d)) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);
    } else if (has_frac) {
        if (!sscanf(&rd->bytes[start], "%lf", &data.d))
            SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_CHARACTER, fail);
        if ((*number = json_new_double(data.d)) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);
    } else {
        if (has_minus) {
            if (!sscanf(&rd->bytes[start], "%ld", &data.i))
                SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_CHARACTER, fail);
            if ((*number = json_new_signed(data.i)) == NULL)
                SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);
        } else {
            if (!sscanf(&rd->bytes[start], "%lu", &data.u))
                SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_CHARACTER, fail);
            if ((*number = json_new_unsigned(data.u)) == NULL)
                SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);
        }
    }

fail:
    return retval;
}

static int parse_value(struct reader *rd, json_t **out)
{
    int c, retval = JSON_PARSE_OK;

    switch ((c = peek(rd))) {
    case 'f':
        if (strncmp(&rd->bytes[rd->index], "false", 5) != 0)
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_FALSE, end);
        if ((*out = json_new_boolean(false)) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
        rd->index += 5;
        break;

    case 'n':
        if (strncmp(&rd->bytes[rd->index], "null", 4) != 0)
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_NULL, end);
        if ((*out = json_new_null()) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
        rd->index += 4;
        break;

    case 't':
        if (strncmp(&rd->bytes[rd->index], "true", 4) != 0)
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_TRUE, end);
        if ((*out = json_new_boolean(true)) == NULL)
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, end);
        rd->index += 4;
        break;

    case '{':
        if ((retval = parse_object(rd, out)) != PARSE_OK)
            goto end;
        break;

    case '[':
        if ((retval = parse_array(rd, out)) != PARSE_OK)
            goto end;
        break;

    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        if ((retval = parse_number(rd, out)) != PARSE_OK)
            goto end;
        break;

    case '"':
        if ((retval = parse_string(rd, out)) != PARSE_OK)
            goto end;
        break;

    default:
        SET_RETVAL_AND_GOTO(JSON_PARSE_UNEXPECTED_CHARACTER, end);
    }

end:
    return retval;
}

static int parse_object(struct reader *rd, json_t **object)
{
    json_t *value;
    string string;
    int retval = JSON_PARSE_OK;

    skip_whitespace(rd);
    if (read(rd) != '{')
        SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_BEGIN_OBJECT, fail);

    if ((*object = json_new_object()) == NULL)
        SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);

    for (;;) {
        skip_whitespace(rd);
        if (peek(rd) == '}')
            break;

        skip_whitespace(rd);
        memset(&string, 0, sizeof(string));
        if ((retval = read_string(rd, &string)) != JSON_PARSE_OK)
            goto fail;

        skip_whitespace(rd);
        if (read(rd) != ':')
            SET_RETVAL_AND_GOTO(JSON_PARSE_EXPECTED_NAME_SEPARATOR, fail);

        skip_whitespace(rd);
        if ((retval = parse_value(rd, &value)) != JSON_PARSE_OK)
            goto fail;

        if (!json_object_insert_without_copy(*object, string.data, value))
            SET_RETVAL_AND_GOTO(JSON_PARSE_MEMORY_ERROR, fail);

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
    json_free(*object);
    return retval;
}

json_t *json_parse(const char *bytes)
{
    return json_parse_ext(bytes, NULL);
}

json_t *json_parse_ext(const char *bytes, int *error)
{
    json_t *json;
    int result;

    struct reader rd = {
        .bytes = bytes,
        .index = 0,
        .ln = 1,
        .col = 1
    };

    skip_whitespace(&rd);
    if ((result = parse_value(&rd, &json)) != JSON_PARSE_OK)
        json = NULL;

    // skip_whitespace(&rd);
    // if (peek(&rd) != 0) {
    //     json_free(json);
    //     json = NULL;
    //     result = JSON_PARSE_EXPECTED_EOF;
    // }

    if (error)
        *error = result;

    printf("Status: %s (Ln %zu, Col %zu)\n", parse_result_to_string(result), rd.ln, rd.col);

    return json;
}
