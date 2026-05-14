#include "printer.h"

#include <cstdio>

#include "logger.h"
#include "types/error-code.h"

namespace Calculator {

Printer::Printer(Types::CalculatorData& data) : ICalculator(data) {}

void Printer::print_help() {
  printf("Usage:\n");
  printf("  calculator -f <number> -s <number> -o <operation>\n");
  printf("  calculator -f <number> -o fact\n");
  printf("  calculator --json '<json>'\n");
  printf("  calculator --json path/to/input.json\n");
  printf("  calculator --help\n\n");
  printf("Options:\n");
  printf("  -f, --first        first integer\n");
  printf("  -s, --second       second integer\n");
  printf("  -o, --operation    add | sub | mul | div | pow | fact\n");
  printf("  -j, --json         raw JSON string or path to JSON file\n");
  printf("  -h, --help         show help\n\n");
  printf("Examples:\n");
  printf("  calculator -f 10 -s 5 -o add\n");
  printf("  calculator --first 7 --second 3 --operation mul\n");
  printf("  calculator -f 5 -o fact\n");
  printf("  calculator --json '{\"first\":10,\"second\":5,\"operation\":\"div\"}'\n");
}

void Printer::print_error(const int error_code) {
  switch (error_code) {
  case static_cast<int>(Types::ErrorCode::ERR_UNKNOWN_OPTION):
    printf("Error: unknown option, unexpected argument, or invalid JSON input.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_INVALID_NUMBER):
    printf("Error: invalid integer value.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_MISSING_FIRST_NUMBER):
    printf("Error: first number is missing.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_MISSING_SECOND_NUMBER):
    printf("Error: second number is missing.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_MISSING_OPERATION):
    printf("Error: operation is missing.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_INVALID_OPERATION):
    printf("Error: invalid operation. Use add, sub, mul, div, pow or fact.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_DIVISION_BY_ZERO):
    printf("Error: division by zero.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_NEGATIVE_FACTORIAL):
    printf("Error: factorial is defined only for non-negative integers.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_NEGATIVE_POWER):
    printf("Error: negative power is not supported for integer result.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_OVERFLOW):
    printf("Error: integer overflow detected.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_INVALID_JSON):
    printf("Error: invalid JSON input.\n");
    break;
  case static_cast<int>(Types::ErrorCode::ERR_STORAGE):
    printf("Error: storage subsystem failed.\n");
    break;
  default:
    printf("Error: unknown error.\n");
    break;
  }
}

void Printer::print_result() {
  if (data_.need_help != 0) {
    Logger::info("Printing help.");
    print_help();
    return;
  }

  if (data_.error_code != Types::ErrorCode::ERR_NONE) {
    Logger::warn(std::string("Printing error with code: ") + std::to_string(static_cast<int>(data_.error_code)));
    print_error(static_cast<int>(data_.error_code));
    return;
  }

  Logger::info(std::string("Printing result to console: ") + std::to_string(data_.result));
  printf("%d\n", data_.result);
}
} // namespace Calculator
