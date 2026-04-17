#pragma once

#include "impl/icalculator.h"

namespace Calculator {
class Initializer : public ICalculator {
 public:
  explicit Initializer(Types::CalculatorData& data);

  void init_calculator_data();
};
} // namespace Calculator
