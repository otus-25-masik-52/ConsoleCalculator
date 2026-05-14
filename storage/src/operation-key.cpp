#include "operation-key.h"

#include <algorithm>

#include "exception.h"
#include "types/error-code.h"

namespace Calculator::Storage {
bool OperationKey::is_commutative(const std::string& operation) {
  return operation == "add" || operation == "+" || operation == "mul" || operation == "*";
}

bool OperationKey::is_factorial(const std::string& operation) {
  return operation == "fact" || operation == "factorial" || operation == "!";
}

std::string OperationKey::symbol_for(const std::string& operation) {
  if (operation == "add" || operation == "+") {
    return "+";
  }
  if (operation == "sub" || operation == "-") {
    return "-";
  }
  if (operation == "mul" || operation == "*") {
    return "*";
  }
  if (operation == "div" || operation == "/") {
    return "/";
  }
  if (operation == "pow" || operation == "^") {
    return "^";
  }
  if (is_factorial(operation)) {
    return "fact";
  }

  throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_INVALID_OPERATION),
                            "Unsupported operation for storage key: " + operation);
}

std::string OperationKey::make(int first_number, std::optional<int> second_number, const std::string& operation) {
  if (is_factorial(operation)) {
    return "fact(" + std::to_string(first_number) + ")";
  }

  if (!second_number.has_value()) {
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_MISSING_SECOND_NUMBER),
                              "Second number is required for binary operation storage key.");
  }

  int left = first_number;
  int right = *second_number;

  if (is_commutative(operation) && left > right) {
    std::swap(left, right);
  }

  return std::to_string(left) + symbol_for(operation) + std::to_string(right);
}
} // namespace Calculator::Storage
