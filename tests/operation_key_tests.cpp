#include <gtest/gtest.h>

#include "operation-key.h"

TEST(OperationKeyTest, NormalizesAdditionOperands) {
  EXPECT_EQ(Calculator::Storage::OperationKey::make(1, 2, "add"), "1+2");
  EXPECT_EQ(Calculator::Storage::OperationKey::make(2, 1, "add"), "1+2");
}

TEST(OperationKeyTest, NormalizesMultiplicationOperands) {
  EXPECT_EQ(Calculator::Storage::OperationKey::make(3, 7, "mul"), "3*7");
  EXPECT_EQ(Calculator::Storage::OperationKey::make(7, 3, "mul"), "3*7");
}

TEST(OperationKeyTest, DoesNotNormalizeSubtractionOperands) {
  EXPECT_EQ(Calculator::Storage::OperationKey::make(1, 2, "sub"), "1-2");
  EXPECT_EQ(Calculator::Storage::OperationKey::make(2, 1, "sub"), "2-1");
}

TEST(OperationKeyTest, DoesNotNormalizeDivisionOperands) {
  EXPECT_EQ(Calculator::Storage::OperationKey::make(1, 2, "div"), "1/2");
  EXPECT_EQ(Calculator::Storage::OperationKey::make(2, 1, "div"), "2/1");
}

TEST(OperationKeyTest, CreatesFactorialKey) {
  EXPECT_EQ(Calculator::Storage::OperationKey::make(5, std::nullopt, "fact"), "fact(5)");
}
