#include "parser.h"

#include <cerrno>
#include <getopt.h>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator {
  int parse_int(const char *text, int *value) {
    char *end_ptr = nullptr;
    long parsed_value = 0;

    if (text == nullptr || value == nullptr) {
      return 0;
    }

    errno = 0;
    parsed_value = strtol(text, &end_ptr, 10);

    if (errno != 0) {
      return 0;
    }

    if (end_ptr == text || *end_ptr != '\0') {
      return 0;
    }

    if (parsed_value > INT_MAX || parsed_value < INT_MIN) {
      return 0;
    }

    *value = static_cast<int>(parsed_value);
    return 1;
  }

  int parse_operation(const char *text, int *operation) {
    if (text == nullptr || operation == nullptr) {
      return 0;
    }

    if (strcmp(text, "add") == 0) {
      *operation = Types::OP_ADD;
      return 1;
    }

    if (strcmp(text, "sub") == 0) {
      *operation = Types::OP_SUB;
      return 1;
    }

    if (strcmp(text, "mul") == 0) {
      *operation = Types::OP_MUL;
      return 1;
    }

    if (strcmp(text, "div") == 0) {
      *operation = Types::OP_DIV;
      return 1;
    }

    if (strcmp(text, "pow") == 0) {
      *operation = Types::OP_POW;
      return 1;
    }

    if (strcmp(text, "fact") == 0) {
      *operation = Types::OP_FACT;
      return 1;
    }

    return 0;
  }

  void parse_arguments(int argc, char **argv, Types::CalculatorData *data) {
    int option = 0;
    int long_index = 0;
    int parsed_number = 0;
    int parsed_operation = Types::OP_NONE;
    static struct option long_options[] = {
      {.name = "first", .has_arg = required_argument, .flag = nullptr, .val = 'f'},
      {.name = "second", .has_arg = required_argument, .flag = nullptr, .val = 's'},
      {.name = "operation", .has_arg = required_argument, .flag = nullptr, .val = 'o'},
      {.name = "help", .has_arg = no_argument, .flag = nullptr, .val = 'h'},
      {.name = nullptr, .has_arg = 0, .flag = nullptr, .val = 0}
    };

    if (data == nullptr) {
      return;
    }

    optind = 1;

    while (true) {
      option = getopt_long(argc, argv, "f:s:o:h", long_options, &long_index);
      if (option == -1) {
        break;
      }

      switch (option) {
        case 'f':
          if (parse_int(optarg, &parsed_number) == 0) {
            data->error_code = Types::ERR_INVALID_NUMBER;
            return;
          }
          data->first_number = parsed_number;
          data->has_first_number = 1;
          break;
        case 's':
          if (parse_int(optarg, &parsed_number) == 0) {
            data->error_code = Types::ERR_INVALID_NUMBER;
            return;
          }
          data->second_number = parsed_number;
          data->has_second_number = 1;
          break;
        case 'o':
          if (parse_operation(optarg, &parsed_operation) == 0) {
            data->error_code = Types::ERR_INVALID_OPERATION;
            return;
          }
          data->operation = parsed_operation;
          break;
        case 'h':
          data->need_help = 1;
          return;
        case '?':
        default:
          data->error_code = Types::ERR_UNKNOWN_OPTION;
          return;
      }
    }

    if (optind < argc && data->error_code == Types::ERR_NONE) {
      data->error_code = Types::ERR_UNKNOWN_OPTION;
    }
  }
} // namespace Calculator
