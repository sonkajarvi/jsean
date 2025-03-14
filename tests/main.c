#include <test/test.h>

int main(void)
{
    // JSON
    test_declare(jsean_type);
    test_declare(jsean_move);
    test_declare(jsean_free);
    test_declare(jsean_set_null);
    test_declare(jsean_set_and_get_boolean);
    test_declare(jsean_set_and_get_double);
    test_declare(jsean_set_and_get_string);

    test_declare(jsean_set_array);
    test_declare(jsean_array_size);
    test_declare(jsean_array_capacity);
    test_declare(jsean_array_reserve);
    test_declare(jsean_array_clear);
    test_declare(jsean_array_at);
    test_declare(jsean_array_push);
    test_declare(jsean_array_pop);
    test_declare(jsean_array_insert);
    test_declare(jsean_array_remove);

    test_declare(jsean_set_object);
    test_declare(jsean_object_count);
    test_declare(jsean_object_capacity);
    test_declare(jsean_object_clear);
    test_declare(jsean_object_get);
    test_declare(jsean_object_insert);
    test_declare(jsean_object_overwrite);
    test_declare(jsean_object_remove);

    // String
    test_declare(string_free);
    test_declare(string_append_char);
    test_declare(string_append_chars_n);
    test_declare(string_reserve);
    test_declare(string_ref);
    test_declare(string_release);
    test_declare(string_shrink);
}
