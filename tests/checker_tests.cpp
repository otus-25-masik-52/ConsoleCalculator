#include <gtest/gtest.h>

#include "checker.h"
#include "exception.h"
#include "types/calculator-data.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace {
Calculator::Types::CalculatorData make_data() {
  return Calculator::Types::CalculatorData{.first_number = 0,
                                           .second_number = 0,
                                           .result = 0,
                                           .has_first_number = 0,
                                           .has_second_number = 0,
                                           .operation = Calculator::Types::OperationCode::OP_NONE,
                                           .error_code = Calculator::Types::ErrorCode::ERR_NONE,
                                           .need_help = 0};
}
} // namespace

TEST(CheckerTest, ThrowsWhenOperationMissing) {
  auto data = make_data();
  data.first_number = 10;
  data.has_first_number = 1;

  Calculator::Checker checker(data);

  try {
    checker.check_arguments();
    FAIL();
  } catch (const Calculator::CalculatorException& exception) {
    EXPECT_EQ(exception.error_code(), static_cast<int>(Calculator::Types::ErrorCode::ERR_MISSING_OPERATION));
  }
}

TEST(CheckerTest, ThrowsForNegativeFactorial) {
  auto data = make_data();
  data.first_number = -1;
  data.has_first_number = 1;
  data.operation = Calculator::Types::OperationCode::OP_FACT;

  Calculator::Checker checker(data);

  try {
    checker.check_arguments();
    FAIL();
  } catch (const Calculator::CalculatorException& exception) {
    EXPECT_EQ(exception.error_code(), static_cast<int>(Calculator::Types::ErrorCode::ERR_NEGATIVE_FACTORIAL));
  }
}

TEST(CheckerTest, AcceptsValidBinaryOperation) {
  auto data = make_data();
  data.first_number = 4;
  data.second_number = 2;
  data.has_first_number = 1;
  data.has_second_number = 1;
  data.operation = Calculator::Types::OperationCode::OP_DIV;

  Calculator::Checker checker(data);

  EXPECT_NO_THROW(checker.check_arguments());
}
