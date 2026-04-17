#pragma once

#include "impl/icalculator.h"

namespace Calculator {
class Checker : public ICalculator {
 public:
  explicit Checker(Types::CalculatorData& data);

  void check_arguments();
};
} // namespace Calculator