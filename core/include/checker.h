#pragma once

#include "interfaces/icalculator.h"

namespace Calculator {
class Checker : public ICalculator {
 public:
  explicit Checker(Types::CalculatorData& data);
  ~Checker() override = default;

  void check_arguments();
};
} // namespace Calculator