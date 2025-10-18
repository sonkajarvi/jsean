//
// Copyright (c) 2025, sonkajarvi
//
// Licensed under the BSD 2-Clause License. See LICENSE.txt
//

#ifndef TESTS_TEST_H_
#define TESTS_TEST_H_

#include <stdio.h>

#ifdef __linux__
# include <time.h>
#endif

#define TEST(test_suite, test_name)                                     \
    void __TEST_CASE_NAME(test_suite, test_name)(struct test_result *); \
    __attribute__((constructor(102)))                                   \
    void __TEST_WRAPPER_NAME(test_suite, test_name)(void)               \
    {                                                                   \
        struct test_case tmp = {                                        \
            .func = __TEST_CASE_NAME(test_suite, test_name),            \
            .suite = #test_suite,                                       \
            .name = #test_name,                                         \
            .path = __FILE__,                                           \
            .file = __FILE_NAME__                                       \
        };                                                              \
        __test_run(&tmp);                                               \
    }                                                                   \
    void __TEST_CASE_NAME(test_suite, test_name)(struct test_result *__TEST_RESULT)

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
#define __TEST_CASE_NAME(suite, name) __test_case_##suite##_##name
#define __TEST_WRAPPER_NAME(suite, name) __test_wrapper_##suite##_##name

struct test_result {
    const char *expr;
    unsigned int line;
    int status;
};

struct test_case {
    void (*func)(struct test_result *);
    const char *suite;
    const char *name;
    const char *path;
    const char *file;
};

extern unsigned long __passed_tests;
extern unsigned long __failed_tests;

static inline void __test_run(struct test_case *test)
{
#ifdef __linux__
    struct timespec start, end;
#endif
    struct test_result result;
    unsigned long time;

#ifdef __linux__
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
#endif

    test->func(&result);

#ifdef __linux__
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
#else
    time = 0;
#endif

    if (result.status == 0) {
        printf("\033[32m[  pass  ]\033[0m %s::%s (%lu ns)\n",
            test->suite, test->name, time);

        __passed_tests++;
    } else {
        printf("\033[31m[  fail  ]\033[0m %s::%s (%lu ns)\n  In file %s, line %u:\n    Assertion failed: '%s'\n",
            test->suite, test->name, time, test->path, result.line, result.expr);

        __failed_tests++;
    }
}

#endif // TESTS_TEST_H_
