//
// Copyright (c) 2025, sonkajarvi
//
// SPDX-License-Identifier: MIT
//

#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdlib.h>

struct test_result
{
    int status;

    const char *file;
    int line;

    const char *lhs_expr;
    int64_t lhs_val;

    const char *rhs_expr;
    int64_t rhs_val;
};

struct test_case
{
    const char *name;
    void (*callback)(struct test_result *);
};

struct test_context
{
    struct test_case *tests;
    size_t capacity;
    size_t size;
};

void test_add(const char *name, void (*callback)(struct test_result *));

#define TEST_PASSED_ 0
#define TEST_FAILED_ 1
#define TEST_CASE_RESULT_PARAM_ result_

#define TEST_CASE_NAME_(name) TEST_CASE_##name##_
#define TEST_WRAPPER_NAME_(name) TEST_WRAPPER_##name##_

#define TEST(name)                                    \
    void TEST_CASE_NAME_(name)(struct test_result *); \
    __attribute__((constructor))                      \
    void TEST_WRAPPER_NAME_(name)(void) {             \
        test_add(#name, TEST_CASE_NAME_(name));       \
    }                                                 \
    void TEST_CASE_NAME_(name)(struct test_result *TEST_CASE_RESULT_PARAM_)

#define ASSERT_(lhs, rhs, x) ({                             \
        int64_t lhs_ = (int64_t)(lhs);                      \
        int64_t rhs_ = (int64_t)(rhs);                      \
        if (!(lhs_ x rhs_)) {                               \
            TEST_CASE_RESULT_PARAM_->status = TEST_FAILED_; \
            TEST_CASE_RESULT_PARAM_->file = __FILE__;       \
            TEST_CASE_RESULT_PARAM_->line = __LINE__;       \
            TEST_CASE_RESULT_PARAM_->lhs_expr = #lhs;       \
            TEST_CASE_RESULT_PARAM_->lhs_val = lhs_;        \
            TEST_CASE_RESULT_PARAM_->rhs_expr = #rhs;       \
            TEST_CASE_RESULT_PARAM_->rhs_val = rhs_;        \
            return;                                         \
        }                                                   \
    })

#define ASSERT_EQ(lhs, rhs) ASSERT_(lhs, rhs, ==)
#define ASSERT_NE(lhs, rhs) ASSERT_(lhs, rhs, !=)
#define ASSERT_GT(lhs, rhs) ASSERT_(lhs, rhs, >)
#define ASSERT_GE(lhs, rhs) ASSERT_(lhs, rhs, >=)
#define ASSERT_LT(lhs, rhs) ASSERT_(lhs, rhs, <)
#define ASSERT_LE(lhs, rhs) ASSERT_(lhs, rhs, <=)

#endif // TEST_H
