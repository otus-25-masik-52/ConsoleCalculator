#include "init.h"

#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {
  void init_calculator_data(Types::CalculatorData *data) {
    if (data == nullptr) {
      return;
    }

    data->first_number = 0;
    data->second_number = 0;
    data->result = 0;
    data->has_first_number = 0;
    data->has_second_number = 0;
    data->operation = Types::OP_NONE;
    data->error_code = Types::ERR_NONE;
    data->need_help = 0;
  }
} // namespace Calculator
