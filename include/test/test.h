#ifndef TEST_TEST_H
#define TEST_TEST_H

#include <stdio.h>

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

#define TEST_RESULT_ID(name) TEST_RESULT_##name
#define TEST_CASE_ID(name) TEST_CASE_##name

#define TEST_STRINGIFY_HELPER(x) #x
#define TEST_STRINGIFY(x) TEST_STRINGIFY_HELPER(x)

#define test_declare(name) ({                                              \
    struct test_result TEST_RESULT_ID(name)(void);                         \
    struct test_case TEST_CASE_ID(name) = { TEST_RESULT_ID(name), #name }; \
    test_run(TEST_CASE_ID(name)); })

#define TEST_RESULT_INIT_PASSED \
    (struct test_result) { 0, 0, TEST_SUCCESS, 0 }

#define TEST_RESULT_INIT_FAILED(expr, file, line) \
    (struct test_result) { \
        expr, \
        file, \
        expr ", in " file ":" TEST_STRINGIFY(line), \
        TEST_FAILURE, \
        line };

#define test_case(name) \
    struct test_result TEST_RESULT_ID(name)(void)

#define test_assert(expr) \
    if (!(expr))          \
        return TEST_RESULT_INIT_FAILED(#expr, __FILE__, __LINE__)

#define test_success() \
    return TEST_RESULT_INIT_PASSED

struct test_result
{
    const char *expr;
    const char *file;
    const char *extra;
    int status;
    int line;
};

struct test_case
{
    struct test_result (*function)(void);
    const char *name;
};

static inline void test_print_pass(const char *name)
{
    printf("[ \033[92mpass\033[0m ] %s\n", name);
}

static inline void test_print_fail(const char *name, const char *extra)
{
    printf("[ \033[1;31mfail\033[0m ] %s \033[90m(%s)\033[0m\n", name, extra);
}

static inline void test_run(struct test_case test)
{
    struct test_result result = test.function();

    if (result.status == TEST_SUCCESS)
        test_print_pass(test.name);
    else
        test_print_fail(test.name, result.extra);
}

#endif // TEST_TEST_H
