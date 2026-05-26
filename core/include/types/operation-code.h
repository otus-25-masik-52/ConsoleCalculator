#pragma once

#include <cstdint>

namespace Calculator::Types {

enum class OperationCode : std::uint8_t {
  OPERATION_NONE = 0,
  OPERATION_ADDITION,
  OPERATION_SUBTRACTION,
  OPERATION_MULTIPLICATION,
  OPERATION_DIVISION,
  OPERATION_EXPONENTIATION,
  OPERATION_FACTORIAL,
};

} // namespace Calculator::Types