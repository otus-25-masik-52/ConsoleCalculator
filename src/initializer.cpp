#include "initializer.h"

#include "logger.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {
Initializer::Initializer(Types::CalculatorData& data) : ICalculator(data) {}

void Initializer::init_calculator_data() {
  data_.first_number = 0;
  data_.second_number = 0;
  data_.result = 0;
  data_.has_first_number = 0;
  data_.has_second_number = 0;
  data_.operation = Types::OP_NONE;
  data_.error_code = Types::ERR_NONE;
  data_.need_help = 0;

  Logger::debug("Calculator data initialized.");
}
} // namespace Calculator
