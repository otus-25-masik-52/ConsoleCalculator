#pragma once

#include "types/calculator-data.h"

namespace Calculator {

class ICalculator {
 public:
  explicit ICalculator(Types::CalculatorData& data) : data_(data) {}
  virtual ~ICalculator() = default;

 protected:
  Types::CalculatorData& data_;
};

} // namespace Calculator
