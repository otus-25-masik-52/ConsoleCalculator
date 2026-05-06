#pragma once

#include "../../core/include/interfaces/icalculator.h"

namespace Calculator {
class Printer : public ICalculator {
 public:
  explicit Printer(Types::CalculatorData& data);
  ~Printer() override = default;

  static void print_help();
  static void print_error(int error_code);

  void print_result();
};

} // namespace Calculator
