//
// Copyright (c) 2024, sonkajarvi
//

#include <stdio.h>
#include <stdlib.h>

#include <jsean/json.h>

#include "string.h"

#define STRINGIFY_INITIAL_CAPACITY 256

static void write_value(string *str, struct json *json, int indent, const char *indent_s);

static void write_indent(string *str, int indent, const char *indent_s)
{
    for (int i = 0; i < indent; i++)
        string_append_chars(str, indent_s);
}

static void write_array(string *str, struct json *json, int indent, const char *indent_s)
{
    string_append_char(str, '[');
    if (indent_s)
        string_append_char(str, '\n');

    const size_t len = json_array_length(json);
    for (size_t i = 0; i < len; i++) {
        if (i > 0) {
            string_append_char(str, ',');
            if (indent_s)
                string_append_char(str, '\n');
        }

        if (indent_s)
            write_indent(str, indent + 1, indent_s);
        write_value(str, json_array_at(json, i), indent + 1, indent_s);
    }

    if (indent_s) {
        string_append_char(str, '\n');
        write_indent(str, indent, indent_s);
    }

    string_append_char(str, ']');
}

static void write_object(string *str, struct json *json, int indent, const char *indent_s)
{
    struct json_object *object = json->data._object;

    string_append_char(str, '{');
    if (indent_s)
        string_append_char(str, '\n');

    struct json_object_entry *buffer = json_object_buffer(object);
    for (size_t i = 0, j = object->count; j; i++) {
        if (!buffer[i].key)
            continue;

        if (j != object->count) {
            string_append_char(str, ',');
            if (indent_s)
                string_append_char(str, '\n');
        }

        if (indent_s)
            write_indent(str, indent + 1, indent_s);
        string_append_char(str, '"');
        string_append_chars(str, buffer[i].key);
        string_append_chars(str, "\":");
        if (indent_s)
            string_append_char(str, ' ');
        write_value(str, &buffer[i].value, indent + 1, indent_s);

        j--;
    }

    if (indent_s) {
        string_append_char(str, '\n');
        write_indent(str, indent, indent_s);
    }

    string_append_char(str, '}');
}

static void write_value(string *str, struct json *json, int indent, const char *indent_s)
{
    char buf[64];

    if (!json)
        return;

    switch (json->type) {
    case JSON_TYPE_NULL:
        string_append_chars(str, "null");
        break;

    case JSON_TYPE_BOOLEAN:
        if (json->data._boolean)
            string_append_chars(str, "true");
        else
            string_append_chars(str, "false");
        break;

    case JSON_TYPE_NUMBER:
        if (json->number_type == JSON_NUMBER_SIGNED)
            snprintf(buf, sizeof(buf), "%ld", json->data._signed);
        else if (json->number_type == JSON_NUMBER_UNSIGNED)
            snprintf(buf, sizeof(buf), "%lu", json->data._unsigned);
        else
            snprintf(buf, sizeof(buf), "%f", json->data._double);
        string_append_chars(str, buf);
        break;

    case JSON_TYPE_STRING:
        string_append_char(str, '"');
        if (json->data._string)
            string_append_chars(str, json->data._string);
        string_append_char(str, '"');
        break;

    case JSON_TYPE_ARRAY:
        write_array(str, json, indent, indent_s);
        break;

    case JSON_TYPE_OBJECT:
        write_object(str, json, indent, indent_s);
        break;

    default:
        break;
    }
}

char *json_stringify(struct json *json, const char *indent_s)
{
    string str = {0};
    string_reserve(&str, STRINGIFY_INITIAL_CAPACITY);

    write_value(&str, json, 0, indent_s);
    str.data = realloc(str.data, str.length + 1);
    return str.data;
}
