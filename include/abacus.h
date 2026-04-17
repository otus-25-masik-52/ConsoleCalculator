#pragma once

#include "impl/icalculator.h"

namespace Calculator {
class Abacus : public ICalculator {
 public:
  explicit Abacus(Types::CalculatorData& data);

  void calculate_result();

 private:
  static int map_math_error(int math_error);
};
} // namespace Calculator