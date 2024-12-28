#ifndef TEST_H
#define TEST_H

#define GREEN   "\033[1;92m"
#define RED     "\033[1;91m"
#define BLACK   "\033[1;90m"
#define RESET   "\033[0m"

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

#define __TEST_FUNCTION(x) __func_##x
#define __TEST_NAME(x) __name_##x

#define __PASS_FORMAT "%s:" GREEN " pass" RESET "\n"
#define __FAIL_FORMAT "%s:" RED " fail" BLACK " (assert '%s', in %s:%d)" RESET "\n"

#define __run(x) ({                                                \
    const struct result __r = x.function();                        \
    if (__r.status == TEST_SUCCESS) printf(__PASS_FORMAT, x.name); \
    else printf(__FAIL_FORMAT, x.name, __r.expr, __r.file, __r.line); })

#define test_declare(x) ({                                   \
    struct result __TEST_FUNCTION(x)(void);                  \
    struct test __TEST_NAME(x) = { __TEST_FUNCTION(x), #x }; \
    __run(__TEST_NAME(x)); })

#define test_case(x) struct result __TEST_FUNCTION(x)(void)

#define test_assert(expr) if (!(expr)) return (struct result){ TEST_FAILURE, __LINE__, #expr, __FILE__ }
#define test_success() return (struct result){ TEST_SUCCESS, 0, 0, 0 }

struct result
{
    int status, line;
    const char *expr, *file;
};

struct test
{
    struct result (*function)(void);
    const char *name;
};

#endif // TEST_H