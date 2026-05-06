#pragma once

#include <stdexcept>
#include <string>

namespace Calculator {
class CalculatorException : public std::runtime_error {
 public:
  CalculatorException(int error_code, const std::string& message)
      : std::runtime_error(message), error_code_(error_code) {}

  [[nodiscard]] int error_code() const noexcept {
    return error_code_;
  }

 private:
  int error_code_;
};
} // namespace Calculator
