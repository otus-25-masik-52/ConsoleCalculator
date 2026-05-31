#pragma once

#include "types/operation-code.h"
#include "types/error-code.h"

namespace Calculator::Types {
struct CalculatorData {
  int first_number = 0;
  int second_number = 0;
  int result = 0;
  int has_first_number = 0;
  int has_second_number = 0;
  OperationCode operation = OperationCode::OPERATION_NONE;
  ErrorCode error_code = ErrorCode::ERROR_NONE;
  int need_help = 0;
};
} // namespace Calculator::Types