#include "runner.h"

#include "abacus.h"
#include "checker.h"
#include "exception.h"
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
  Parser parser(data);
  Printer printer(data);

  try {
    parser.parse_arguments(argc, argv);

    if (data.need_help != 0) {
      printer.print_result();
      Logger::info("Application finished after help output.");
      Logger::shutdown();
      return static_cast<int>(Types::ErrorCode::ERR_NONE);
    }

    checker.check_arguments();
    abacus.calculate_result();
    printer.print_result();

    Logger::info("Application finished successfully.");
    Logger::shutdown();
    return static_cast<int>(Types::ErrorCode::ERR_NONE);
  } catch (const CalculatorException& exception) {
    data.error_code = static_cast<Types::ErrorCode>(exception.error_code());
    Logger::error(std::string("Application failed: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  } catch (const std::exception& exception) {
    data.error_code = Types::ErrorCode::ERR_UNKNOWN_OPTION;
    Logger::critical(std::string("Unhandled std::exception: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  } catch (...) {
    data.error_code = Types::ErrorCode::ERR_UNKNOWN_OPTION;
    Logger::critical("Unhandled unknown exception.");
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  }
}
} // namespace Calculator
