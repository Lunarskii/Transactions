#ifndef TRANSACTIONS_INCLUDE_TESTS_TEST_CORE_H_
#define TRANSACTIONS_INCLUDE_TESTS_TEST_CORE_H_

#include <gtest/gtest.h>
#include "common/storage_struct.h"

namespace Test
{

using namespace s21;

static const Value value1 = {"last_name", "first_name", 2000, "city", 183};
static const Value value2 = {"abcdef", "defabc", 1900, "Moscow", 99};

} // namespace Test

#endif // TRANSACTIONS_INCLUDE_TESTS_TEST_CORE_H_
