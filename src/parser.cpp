#include "parser.h"

#include <cerrno>
#include <climits>
#include <cstring>
#include <fstream>
#include <getopt.h>
#include <unistd.h>

#include "exception.h"
#include "logger.h"
#include "types/error-code.h"
#include "types/operation-code.h"

#include "nlohmann/json.hpp"

namespace Calculator {

Parser::Parser(Types::CalculatorData& data) : ICalculator(data) {}

int Parser::parse_int(const char* text, int* value) {
  char* end_ptr = nullptr;
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

int Parser::parse_operation(const char* text, int* operation) {
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

std::string Parser::read_json_payload(const char* text_or_path) {
  if (text_or_path == nullptr) {
    throw CalculatorException(Types::ERR_INVALID_JSON, "JSON source is null.");
  }

  std::ifstream input_file(text_or_path);
  if (input_file.is_open()) {
    std::ostringstream file_buffer;
    file_buffer << input_file.rdbuf();
    Logger::info(std::string("JSON loaded from file: ") + text_or_path);
    return file_buffer.str();
  }

  Logger::info("JSON loaded from raw text argument.");
  return text_or_path;
}

void Parser::parse_json(const char* text_or_path) {
  Logger::debug("Parser::parse_json started.");

  const std::string json_payload = read_json_payload(text_or_path);

  nlohmann::json json;
  try {
    json = nlohmann::json::parse(json_payload);
  } catch (const nlohmann::json::exception& exception) {
    Logger::error(std::string("JSON parsing failed: ") + exception.what());
    throw CalculatorException(Types::ERR_INVALID_JSON, std::string("Invalid JSON: ") + exception.what());
  }

  if (!json.is_object()) {
    throw CalculatorException(Types::ERR_INVALID_JSON, "JSON root must be an object.");
  }

  if (json.contains("first")) {
    if (!json.at("first").is_number_integer()) {
      throw CalculatorException(Types::ERR_INVALID_JSON, "Field 'first' must be an integer.");
    }
    data_.first_number = json.at("first").get<int>();
    data_.has_first_number = 1;
  }

  if (json.contains("second")) {
    if (!json.at("second").is_number_integer()) {
      throw CalculatorException(Types::ERR_INVALID_JSON, "Field 'second' must be an integer.");
    }
    data_.second_number = json.at("second").get<int>();
    data_.has_second_number = 1;
  }

  if (json.contains("operation")) {
    if (!json.at("operation").is_string()) {
      throw CalculatorException(Types::ERR_INVALID_JSON, "Field 'operation' must be a string.");
    }

    const std::string operation_text = json.at("operation").get<std::string>();
    int parsed_operation = Types::OP_NONE;
    if (parse_operation(operation_text.c_str(), &parsed_operation) == 0) {
      Logger::warn(std::string("Invalid operation in JSON: ") + operation_text);
      throw CalculatorException(Types::ERR_INVALID_OPERATION, std::string("Invalid operation: ") + operation_text);
    }

    data_.operation = parsed_operation;
  }

  Logger::debug("Parser::parse_json finished successfully.");
}

void Parser::parse_arguments(int argc, char** argv) {
  int option = 0;
  int long_index = 0;
  int parsed_number = 0;
  int parsed_operation = Types::OP_NONE;
  static struct option long_options[] = {
      {.name = "first", .has_arg = required_argument, .flag = nullptr, .val = 'f'},
      {.name = "second", .has_arg = required_argument, .flag = nullptr, .val = 's'},
      {.name = "operation", .has_arg = required_argument, .flag = nullptr, .val = 'o'},
      {.name = "json", .has_arg = required_argument, .flag = nullptr, .val = 'j'},
      {.name = "help", .has_arg = no_argument, .flag = nullptr, .val = 'h'},
      {.name = nullptr, .has_arg = 0, .flag = nullptr, .val = 0}};

  Logger::debug("Parser::parse_arguments started.");
  optind = 1;

  while (true) {
    option = getopt_long(argc, argv, "f:s:o:j:h", long_options, &long_index);
    if (option == -1) {
      break;
    }

    switch (option) {
    case 'f':
      if (parse_int(optarg, &parsed_number) == 0) {
        Logger::warn(std::string("Failed to parse first number: ") + optarg);
        throw CalculatorException(Types::ERR_INVALID_NUMBER, std::string("Invalid first number: ") + optarg);
      }
      data_.first_number = parsed_number;
      data_.has_first_number = 1;
      Logger::info(std::string("Parsed first number: ") + optarg);
      break;
    case 's':
      if (parse_int(optarg, &parsed_number) == 0) {
        Logger::warn(std::string("Failed to parse second number: ") + optarg);
        throw CalculatorException(Types::ERR_INVALID_NUMBER, std::string("Invalid second number: ") + optarg);
      }
      data_.second_number = parsed_number;
      data_.has_second_number = 1;
      Logger::info(std::string("Parsed second number: ") + optarg);
      break;
    case 'o':
      if (parse_operation(optarg, &parsed_operation) == 0) {
        Logger::warn(std::string("Failed to parse operation: ") + optarg);
        throw CalculatorException(Types::ERR_INVALID_OPERATION, std::string("Invalid operation: ") + optarg);
      }
      data_.operation = parsed_operation;
      Logger::info(std::string("Parsed operation: ") + optarg);
      break;
    case 'j':
      parse_json(optarg);
      break;
    case 'h':
      data_.need_help = 1;
      Logger::info("Help requested.");
      return;
    case '?':
    default:
      Logger::warn("Unknown option or unexpected argument received.");
      throw CalculatorException(Types::ERR_UNKNOWN_OPTION, "Unknown option or unexpected argument.");
    }
  }

  if (optind < argc) {
    Logger::warn("Unexpected positional arguments found.");
    throw CalculatorException(Types::ERR_UNKNOWN_OPTION, "Unexpected positional arguments.");
  }

  Logger::debug("Parser::parse_arguments finished.");
}
} // namespace Calculator
