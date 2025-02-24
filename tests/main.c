#include <stdio.h> // IWYU pragma: keep

#include <test.h>

int main(void)
{
    // JSON
    test_declare(JSON_move);
    test_declare(JSON_type);
    test_declare(json_set_null);
    test_declare(json_set_and_get_boolean);
    test_declare(json_set_and_get_double);
    test_declare(json_set_and_get_string);

    test_declare(JSON_set_array);
    test_declare(JSON_array_size);
    test_declare(JSON_array_capacity);
    test_declare(JSON_array_reserve);
    test_declare(JSON_array_clear);
    test_declare(JSON_array_at);
    test_declare(JSON_array_push);
    test_declare(JSON_array_pop);
    test_declare(JSON_array_insert);
    test_declare(JSON_array_remove);

    test_declare(json_init_object);
    test_declare(JSON_object_count);
    test_declare(JSON_object_capacity);
    test_declare(JSON_object_clear);
    test_declare(JSON_object_get);
    test_declare(JSON_object_insert);
    test_declare(JSON_object_overwrite);
    test_declare(JSON_object_remove);

    // String
    test_declare(string_free);
    test_declare(string_append_char);
    test_declare(string_append_chars_n);
    test_declare(string_reserve);
    test_declare(string_ref);
    test_declare(string_release);
    test_declare(string_shrink);
}
