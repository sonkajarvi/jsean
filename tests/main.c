#include <stdio.h> // IWYU pragma: keep

#include "test.h"

int main(void)
{
    // JSON
    test_declare(json_get_type);
    test_declare(json_set_null);
    test_declare(json_set_and_get_boolean);
    test_declare(json_set_and_get_signed);
    test_declare(json_set_and_get_unsigned);
    test_declare(json_set_and_get_double);
    test_declare(json_set_and_get_string);

    // String
    test_declare(string_free);
    test_declare(string_append_char);
    test_declare(string_append_chars_n);
    test_declare(string_reserve);
}
