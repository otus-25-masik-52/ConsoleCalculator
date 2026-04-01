#include "printer.h"

#include <cstdio>

#include "types/error-code.h"

namespace Calculator {
  void print_help() {
    printf("Usage:\n");
    printf("  calculator -f <number> -s <number> -o <operation>\n");
    printf("  calculator -f <number> -o fact\n");
    printf("  calculator --help\n\n");
    printf("Options:\n");
    printf("  -f, --first        first integer\n");
    printf("  -s, --second       second integer\n");
    printf("  -o, --operation    add | sub | mul | div | pow | fact\n");
    printf("  -h, --help         show help\n\n");
    printf("Examples:\n");
    printf("  calculator -f 10 -s 5 -o add\n");
    printf("  calculator --first 7 --second 3 --operation mul\n");
    printf("  calculator -f 5 -o fact\n");
  }

  void print_error(const int error_code) {
    switch (error_code) {
      case Types::ERR_UNKNOWN_OPTION:
        printf("Error: unknown option or unexpected argument.\n");
        break;
      case Types::ERR_INVALID_NUMBER:
        printf("Error: invalid integer value.\n");
        break;
      case Types::ERR_MISSING_FIRST_NUMBER:
        printf("Error: first number is missing.\n");
        break;
      case Types::ERR_MISSING_SECOND_NUMBER:
        printf("Error: second number is missing.\n");
        break;
      case Types::ERR_MISSING_OPERATION:
        printf("Error: operation is missing.\n");
        break;
      case Types::ERR_INVALID_OPERATION:
        printf("Error: invalid operation. Use add, sub, mul, div, pow or fact.\n");
        break;
      case Types::ERR_DIVISION_BY_ZERO:
        printf("Error: division by zero.\n");
        break;
      case Types::ERR_NEGATIVE_FACTORIAL:
        printf("Error: factorial is defined only for non-negative integers.\n");
        break;
      case Types::ERR_NEGATIVE_POWER:
        printf("Error: negative power is not supported for integer result.\n");
        break;
      case Types::ERR_OVERFLOW:
        printf("Error: integer overflow detected.\n");
        break;
      default:
        printf("Error: unknown error.\n");
        break;
    }
  }

  void print_result(const Types::CalculatorData *data) {
    if (data == nullptr) {
      return;
    }

    if (data->need_help != 0) {
      print_help();
      return;
    }

    if (data->error_code != Types::ERR_NONE) {
      print_error(data->error_code);
      return;
    }

    printf("%d\n", data->result);
  }
} // namespace Calculator
