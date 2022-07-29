// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <platform/types.h>

#include <gtest/gtest.h>

#include <climits>

#define TEST_PRINT_S128(Expected, Value)                                                                                       \
  do {                                                                                                                         \
    std::ostringstream ss;                                                                                                     \
    ss << static_cast<s128>(Value);                                                                                            \
    EXPECT_EQ(Expected, ss.str());                                                                                             \
  } while (false)

TEST(types_test, print_s128)
{
  TEST_PRINT_S128("0", 0);
  TEST_PRINT_S128("1", 1);
  TEST_PRINT_S128("-1", -1);
  TEST_PRINT_S128("10", 10);
  TEST_PRINT_S128("-10", -10);
  TEST_PRINT_S128("255", 255);
  TEST_PRINT_S128("-255", -255);
  TEST_PRINT_S128(
      "170141183460469231731687303715884105727",
      static_cast<s128>(static_cast<u128>(~static_cast<u128>(0)) >> static_cast<u128>(1)));
  TEST_PRINT_S128(
      "-170141183460469231731687303715884105728", static_cast<s128>(static_cast<u128>(1) << static_cast<u128>(127)));
}

#define TEST_PRINT_U128(Expected, Value)                                                                                       \
  do {                                                                                                                         \
    std::ostringstream ss;                                                                                                     \
    ss << static_cast<u128>(Value);                                                                                            \
    EXPECT_EQ(Expected, ss.str());                                                                                             \
  } while (false)

TEST(types_test, print_u128)
{
  TEST_PRINT_U128("0", 0);
  TEST_PRINT_U128("1", 1);
  TEST_PRINT_U128("10", 10);
  TEST_PRINT_U128("255", 255);
  TEST_PRINT_U128(
      "170141183460469231731687303715884105727", (static_cast<u128>(~static_cast<u128>(0)) >> static_cast<u128>(1)));
  TEST_PRINT_U128("340282366920938463463374607431768211455", static_cast<u128>(~static_cast<u128>(0)));
}
