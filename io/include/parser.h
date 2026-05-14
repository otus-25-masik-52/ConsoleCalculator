#pragma once

#include <string>

#include "../../core/include/interfaces/icalculator.h"

namespace Calculator {
class Parser : public ICalculator {
 public:
  explicit Parser(Types::CalculatorData& data);
  ~Parser() override = default;

  static int parse_int(const char* text, int* value);
  static int parse_operation(const char* text, int* operation);
  void parse_json(const char* text_or_path);

  void parse_arguments(int argc, char** argv);

 private:
  static std::string read_json_payload(const char* text_or_path);
};

} // namespace Calculator
