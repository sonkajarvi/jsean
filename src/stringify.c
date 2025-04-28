//
// Copyright (c) 2024-2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#include <stdio.h>

#include <jsean/jsean.h>
#include <jsean/string.h>

static void write_value(struct string *str, jsean_t *json, int indent, const char *indent_s);

static void write_indent(struct string *str, int indent, const char *indent_s)
{
    for (int i = 0; i < indent; i++)
        string_append_chars(str, indent_s);
}

static void write_array(struct string *str, jsean_t *json, int indent, const char *indent_s)
{
    string_append_char(str, '[');
    if (indent_s)
        string_append_char(str, '\n');

    const size_t len = jsean_array_size(json);
    for (size_t i = 0; i < len; i++) {
        if (i > 0) {
            string_append_char(str, ',');
            if (indent_s)
                string_append_char(str, '\n');
        }

        if (indent_s)
            write_indent(str, indent + 1, indent_s);
        write_value(str, jsean_array_at(json, i), indent + 1, indent_s);
    }

    if (indent_s) {
        string_append_char(str, '\n');
        write_indent(str, indent, indent_s);
    }

    string_append_char(str, ']');
}

static void write_object(struct string *str, jsean_t *json, int indent, const char *indent_s)
{
    struct jsean_object_ *obj = json->data._object;

    string_append_char(str, '{');
    if (indent_s)
        string_append_char(str, '\n');

    // struct jsean_object_entry *buffer = jsean_object_buffer(object);
    for (size_t i = 0, j = obj->count; j; i++) {
        if (!obj->data[i].key)
            continue;

        if (j != obj->count) {
            string_append_char(str, ',');
            if (indent_s)
                string_append_char(str, '\n');
        }

        if (indent_s)
            write_indent(str, indent + 1, indent_s);
        string_append_char(str, '"');
        string_append_chars(str, obj->data[i].key);
        string_append_chars(str, "\":");
        if (indent_s)
            string_append_char(str, ' ');
        write_value(str, &obj->data[i].value, indent + 1, indent_s);

        j--;
    }

    if (indent_s) {
        string_append_char(str, '\n');
        write_indent(str, indent, indent_s);
    }

    string_append_char(str, '}');
}

static void write_value(struct string *str, jsean_t *json, int indent, const char *indent_s)
{
    char buf[64];

    if (!json)
        return;

    switch (json->type) {
    case JSEAN_TYPE_NULL:
        string_append_chars(str, "null");
        break;

    case JSEAN_TYPE_BOOLEAN:
        if (json->data._boolean)
            string_append_chars(str, "true");
        else
            string_append_chars(str, "false");
        break;

    case JSEAN_TYPE_NUMBER:
        snprintf(buf, sizeof(buf), "%f", json->data._double);
        string_append_chars(str, buf);
        break;

    case JSEAN_TYPE_STRING:
        string_append_char(str, '"');
        if (json->data._string)
            string_append_chars(str, json->data._string);
        string_append_char(str, '"');
        break;

    case JSEAN_TYPE_ARRAY:
        write_array(str, json, indent, indent_s);
        break;

    case JSEAN_TYPE_OBJECT:
        write_object(str, json, indent, indent_s);
        break;

    default:
        break;
    }
}

struct string json_stringify(jsean_t *json, const char *indent_s)
{
    struct string str = {0};
    string_reserve(&str, 512);

    write_value(&str, json, 0, indent_s);
    return str;
}
