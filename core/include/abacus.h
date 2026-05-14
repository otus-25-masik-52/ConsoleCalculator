#pragma once

#include "interfaces/icalculator.h"

namespace Calculator {
class Abacus : public ICalculator {
 public:
  explicit Abacus(Types::CalculatorData& data);
  ~Abacus() override = default;

  void calculate_result();

 private:
  static int map_math_error(int math_error);
};
} // namespace Calculator