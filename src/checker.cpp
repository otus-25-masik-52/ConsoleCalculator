#include "checker.h"

#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {
  void check_arguments(Types::CalculatorData *data) {
    if (data == nullptr) {
      return;
    }

    if (data->error_code != Types::ERR_NONE || data->need_help != 0) {
      return;
    }

    if (data->operation == Types::OP_NONE) {
      data->error_code = Types::ERR_MISSING_OPERATION;
      return;
    }

    if (data->has_first_number == 0) {
      data->error_code = Types::ERR_MISSING_FIRST_NUMBER;
      return;
    }

    if (data->operation == Types::OP_FACT) {
      if (data->first_number < 0) {
        data->error_code = Types::ERR_NEGATIVE_FACTORIAL;
        return;
      }
      return;
    }

    if (data->has_second_number == 0) {
      data->error_code = Types::ERR_MISSING_SECOND_NUMBER;
      return;
    }

    if (data->operation == Types::OP_DIV && data->second_number == 0) {
      data->error_code = Types::ERR_DIVISION_BY_ZERO;
      return;
    }

    if (data->operation == Types::OP_POW && data->second_number < 0) {
      data->error_code = Types::ERR_NEGATIVE_POWER;
    }
  }
} // namespace Calculator
