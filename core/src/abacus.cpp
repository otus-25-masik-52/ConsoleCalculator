#include "abacus.h"

#include <calculator-math.h>

#include "exception.h"
#include "logger.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {

Abacus::Abacus(Types::CalculatorData& data) : ICalculator(data) {}

int Abacus::map_math_error(const int math_error) {
  switch (math_error) {
  case CalculatorMath::ERR_NONE:
    return static_cast<int>(Types::ErrorCode::ERR_NONE);
  case CalculatorMath::ERR_OVERFLOW:
    return static_cast<int>(Types::ErrorCode::ERR_OVERFLOW);
  case CalculatorMath::ERR_DIV_ZERO:
    return static_cast<int>(Types::ErrorCode::ERR_DIVISION_BY_ZERO);
  case CalculatorMath::ERR_INVALID_ARGUMENTS:
  default:
    return static_cast<int>(Types::ErrorCode::ERR_INVALID_OPERATION);
  }
}

void Abacus::calculate_result() {
  int math_status = CalculatorMath::ERR_NONE;

  Logger::debug("Abacus::calculate_result started.");

  if (data_.need_help != 0) {
    Logger::debug("Calculation skipped because help is requested.");
    return;
  }

  switch (data_.operation) {
  case Types::OperationCode::OP_ADD:
    math_status = CalculatorMath::addition(data_.first_number, data_.second_number, &data_.result);
    break;
  case Types::OperationCode::OP_SUB:
    math_status = CalculatorMath::subtraction(data_.first_number, data_.second_number, &data_.result);
    break;
  case Types::OperationCode::OP_MUL:
    math_status = CalculatorMath::multiplication(data_.first_number, data_.second_number, &data_.result);
    break;
  case Types::OperationCode::OP_DIV:
    math_status = CalculatorMath::division(data_.first_number, data_.second_number, &data_.result);
    break;
  case Types::OperationCode::OP_POW:
    math_status = CalculatorMath::exponentiation(data_.first_number, data_.second_number, &data_.result);
    break;
  case Types::OperationCode::OP_FACT:
    math_status = CalculatorMath::factorial(data_.first_number, &data_.result);
    break;
  default:
    Logger::error("Unknown operation code received by calculator.");
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_INVALID_OPERATION), "Unknown operation code.");
  }

  const int mapped_error = map_math_error(math_status);
  if (mapped_error != static_cast<int>(Types::ErrorCode::ERR_NONE)) {
    Logger::warn(std::string("Calculation finished with error code: ") + std::to_string(mapped_error));
    throw CalculatorException(mapped_error, "Calculation failed.");
  }

  Logger::info(std::string("Calculation completed. Result: ") + std::to_string(data_.result));
}
} // namespace Calculator
