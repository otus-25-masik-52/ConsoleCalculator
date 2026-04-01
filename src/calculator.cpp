#include "calculator.h"

#include <calculator-math.h>

#include "types/error-code.h"
#include "types/operation-code.h"

namespace {
  int map_math_error(const int math_error) {
    switch (math_error) {
      case CalculatorMath::ERR_NONE:
        return Calculator::Types::ERR_NONE;
      case CalculatorMath::ERR_OVERFLOW:
        return Calculator::Types::ERR_OVERFLOW;
      case CalculatorMath::ERR_DIV_ZERO:
        return Calculator::Types::ERR_DIVISION_BY_ZERO;
      case CalculatorMath::ERR_INVALID_ARGUMENTS:
      default:
        return Calculator::Types::ERR_INVALID_OPERATION;
    }
  }
}

namespace Calculator {
  void calculate_result(Types::CalculatorData *data) {
    int math_status = CalculatorMath::ERR_NONE;

    if (data == nullptr) {
      return;
    }

    if (data->error_code != Types::ERR_NONE || data->need_help != 0) {
      return;
    }

    switch (data->operation) {
      case Types::OP_ADD:
        math_status = CalculatorMath::addition(data->first_number, data->second_number, &data->result);
        break;
      case Types::OP_SUB:
        math_status = CalculatorMath::subtraction(data->first_number, data->second_number, &data->result);
        break;
      case Types::OP_MUL:
        math_status = CalculatorMath::multiplication(data->first_number, data->second_number, &data->result);
        break;
      case Types::OP_DIV:
        math_status = CalculatorMath::division(data->first_number, data->second_number, &data->result);
        break;
      case Types::OP_POW:
        math_status = CalculatorMath::exponentiation(data->first_number, data->second_number, &data->result);
        break;
      case Types::OP_FACT:
        math_status = CalculatorMath::factorial(data->first_number, &data->result);
        break;
      default:
        data->error_code = Types::ERR_INVALID_OPERATION;
        return;
    }

    data->error_code = map_math_error(math_status);
  }
} // namespace Calculator
