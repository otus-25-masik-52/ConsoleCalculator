#pragma once

#include "types/calculator-data.h"

namespace Calculator {
  void parse_arguments(int argc, char **argv, Types::CalculatorData *data);

  int parse_int(const char *text, int *value);

  int parse_operation(const char *text, int *operation);
} // namespace Calculator
