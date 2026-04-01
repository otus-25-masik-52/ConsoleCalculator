#pragma once

#include "types/calculator-data.h"

namespace Calculator {
  void print_result(const Types::CalculatorData *data);

  void print_help();

  void print_error(int error_code);
} // namespace Calculator
