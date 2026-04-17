#include "checker.h"

#include "exception.h"
#include "logger.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {
Checker::Checker(Types::CalculatorData& data) : ICalculator(data) {}

void Checker::check_arguments() {
  Logger::debug("Checker::check_arguments started.");

  if (data_.need_help != 0) {
    Logger::debug("Checker skipped because help is requested.");
    return;
  }

  if (data_.operation == Types::OP_NONE) {
    Logger::warn("Operation is missing.");
    throw CalculatorException(Types::ERR_MISSING_OPERATION, "Operation is missing.");
  }

  if (data_.has_first_number == 0) {
    Logger::warn("First number is missing.");
    throw CalculatorException(Types::ERR_MISSING_FIRST_NUMBER, "First number is missing.");
  }

  if (data_.operation == Types::OP_FACT) {
    if (data_.first_number < 0) {
      Logger::warn("Negative factorial argument received.");
      throw CalculatorException(Types::ERR_NEGATIVE_FACTORIAL, "Factorial argument must be non-negative.");
    }
    Logger::debug("Checker finished successfully for factorial.");
    return;
  }

  if (data_.has_second_number == 0) {
    Logger::warn("Second number is missing.");
    throw CalculatorException(Types::ERR_MISSING_SECOND_NUMBER, "Second number is missing.");
  }

  if (data_.operation == Types::OP_DIV && data_.second_number == 0) {
    Logger::warn("Division by zero detected before calculation.");
    throw CalculatorException(Types::ERR_DIVISION_BY_ZERO, "Division by zero.");
  }

  if (data_.operation == Types::OP_POW && data_.second_number < 0) {
    Logger::warn("Negative power is not supported.");
    throw CalculatorException(Types::ERR_NEGATIVE_POWER, "Negative power is not supported.");
  }

  Logger::debug("Checker::check_arguments finished successfully.");
}
} // namespace Calculator
