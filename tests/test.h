//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#ifndef TESTS_TEST_H_
#define TESTS_TEST_H_

#include <stdio.h>

#define TEST(name_)                                     \
    void __TEST_CASE_NAME(name_)(struct test_result *); \
    __attribute__((constructor(102)))                   \
    void __TEST_WRAPPER_NAME(name_)(void)               \
    {                                                   \
        struct test_case tmp = {                        \
            .func = __TEST_CASE_NAME(name_),            \
            .name = #name_,                             \
            .path = __FILE__,                           \
            .file = __FILE_NAME__                       \
        };                                              \
        __test_run(&tmp);                               \
    }                                                   \
    void __TEST_CASE_NAME(name_)(struct test_result *__TEST_RESULT)

#define ASSERT(expr_)                       \
    {                                       \
        if ((expr_)) {                      \
            __TEST_RESULT->status = 0;      \
        } else {                            \
            __TEST_RESULT->expr = #expr_;   \
            __TEST_RESULT->line = __LINE__; \
            __TEST_RESULT->status = 1;      \
            return;                         \
        }                                   \
    }

#define __TEST_RESULT __result
#define __TEST_CASE_NAME(name) __test_case_##name
#define __TEST_WRAPPER_NAME(name) __test_wrapper_##name

struct test_result {
    const char *expr;
    unsigned int line;
    int status;
};

struct test_case {
    void (*func)(struct test_result *);
    const char *name;
    const char *path;
    const char *file;
};

static inline void __test_run(struct test_case *test)
{
    struct test_result tmp;

    test->func(&tmp);

    if (tmp.status == 0) {
        printf("[ \033[1;92mPASS\033[0m ] %s\n", test->name);
    } else {
        printf("[ \033[1;91mFAIL\033[0m ] %s\n  In file %s, line %u:\n    Assertion failed: '%s'\n",
            test->name, test->path, tmp.line, tmp.expr);
    }
}

#endif // TESTS_TEST_H_
