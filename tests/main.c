#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"

static struct test_context ctx = { 0 };

void test_add(const char *name, void (*callback)(struct test_result *))
{
    if (!ctx.tests || ctx.capacity >= ctx.size) {
        ctx.capacity += 256;
        if (!(ctx.tests = realloc(ctx.tests, ctx.capacity * sizeof(*ctx.tests))))
            exit(ENOMEM);
    }

    ctx.tests[ctx.size].name = name;
    ctx.tests[ctx.size].callback = callback;
    ctx.size++;
}

void test_print_passed(const char *name)
{
    printf("[ \033[92mPASS\033[0m ] %s\n", name);
}

void test_print_failed(const char *name, struct test_result *result)
{
    printf("[ \033[91mFAIL\033[0m ] %s\n", name);
    printf("  In file %s, line %u:\n", result->file, result->line);
    printf("    Got %ld from '%s'\n", result->lhs_val, result->lhs_expr);
    printf("    With %ld from '%s'\n", result->rhs_val, result->rhs_expr);
}

int main(void)
{
    struct test_result tmp;

    for (size_t i = 0; i < ctx.size; i++) {
        tmp.status = 0;
        ctx.tests[i].callback(&tmp);

        if (tmp.status == TEST_FAILED_)
            test_print_failed(ctx.tests[i].name, &tmp);
        else
            test_print_passed(ctx.tests[i].name);
    }

    free(ctx.tests);
}
