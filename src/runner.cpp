#include "runner.h"

#include "abacus.h"
#include "checker.h"
#include "exception.h"
#include "initializer.h"
#include "logger.h"
#include "parser.h"
#include "printer.h"
#include "types/error-code.h"

namespace Calculator {
int Runner::run(int argc, char** argv) {
  Types::CalculatorData data;

  Logger::init();
  Logger::info("Application started.");

  Abacus abacus(data);
  Checker checker(data);
  Initializer initializer(data);
  Parser parser(data);
  Printer printer(data);

  try {
    initializer.init_calculator_data();
    parser.parse_arguments(argc, argv);

    if (data.need_help != 0) {
      printer.print_result();
      Logger::info("Application finished after help output.");
      Logger::shutdown();
      return Types::ErrorCode::ERR_NONE;
    }

    checker.check_arguments();
    abacus.calculate_result();
    printer.print_result();

    Logger::info("Application finished successfully.");
    Logger::shutdown();
    return Types::ERR_NONE;
  } catch (const CalculatorException& exception) {
    data.error_code = exception.error_code();
    Logger::error(std::string("Application failed: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return data.error_code;
  } catch (const std::exception& exception) {
    data.error_code = Types::ERR_UNKNOWN_OPTION;
    Logger::critical(std::string("Unhandled std::exception: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return data.error_code;
  } catch (...) {
    data.error_code = Types::ERR_UNKNOWN_OPTION;
    Logger::critical("Unhandled unknown exception.");
    printer.print_result();
    Logger::shutdown();
    return data.error_code;
  }
}
} // namespace Calculator
