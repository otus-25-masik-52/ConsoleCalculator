#include "runner.h"

#include "calculator.h"
#include "types/calculator-data.h"
#include "checker.h"
#include "init.h"
#include "parser.h"
#include "printer.h"

namespace Calculator {
  int run(int argc, char **argv) {
    Types::CalculatorData data;

    init_calculator_data(&data);
    parse_arguments(argc, argv, &data);

    if (data.need_help != 0 || data.error_code != 0) {
      print_result(&data);
      return data.error_code;
    }

    check_arguments(&data);
    if (data.error_code != 0) {
      print_result(&data);
      return data.error_code;
    }

    calculate_result(&data);
    print_result(&data);

    return data.error_code;
  }
} // namespace Calculator
