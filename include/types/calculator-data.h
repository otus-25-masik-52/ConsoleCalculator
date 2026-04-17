#pragma once

namespace Calculator::Types {
struct CalculatorData {
  int first_number;
  int second_number;
  int result;
  int has_first_number;
  int has_second_number;
  int operation;
  int error_code;
  int need_help;
};
} // namespace Calculator::Types