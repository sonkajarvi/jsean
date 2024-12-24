#include <stdio.h> // IWYU pragma: keep

#include "test.h"

int main(void)
{
    // String
    test_declare(string_free);
    test_declare(string_append_char);
    test_declare(string_append_chars_n);
    test_declare(string_reserve);
}
