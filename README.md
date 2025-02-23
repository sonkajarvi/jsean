## Jsean

### Usage

```c
#include <stdio.h>

#include <jsean/JSON.h>
#include <jsean/string.h>

int main(void)
{
    // Parse string to JSON value
    const char *in = "{\"version\":89}";
    json_t json = {0};
    json_parse(&json, in);

    // Modify JSON value
    json_t *value = JSON_object_get(&json, "version");
    json_set_unsigned(value, json_unsigned(value) - 78);

    // Stringify JSON value to string
    struct string out = json_stringify(&json, NULL);
    printf("%s\n", string_ref(&out)); // {"version":11}

    // Remember to free any allocated memory :)
    json_free(&json);
    string_free(&out);

    return 0;
}
```
