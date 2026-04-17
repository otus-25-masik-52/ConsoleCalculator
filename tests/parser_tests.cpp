#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "exception.h"
#include "initializer.h"
#include "parser.h"
#include "types/calculator-data.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace {
Calculator::Types::CalculatorData make_data() {
  return Calculator::Types::CalculatorData{0, 0, 0, 0, 0, Calculator::Types::OP_NONE, Calculator::Types::ERR_NONE, 0};
}
} // namespace

TEST(ParserJsonTest, ParsesRawJsonText) {
  auto data = make_data();
  Calculator::Initializer initializer(data);
  initializer.init_calculator_data();
  Calculator::Parser parser(data);

  parser.parse_json(R"({"first":10,"second":5,"operation":"add"})");

  EXPECT_EQ(data.first_number, 10);
  EXPECT_EQ(data.second_number, 5);
  EXPECT_EQ(data.operation, Calculator::Types::OP_ADD);
  EXPECT_EQ(data.has_first_number, 1);
  EXPECT_EQ(data.has_second_number, 1);
}

TEST(ParserJsonTest, ParsesJsonFromFilePath) {
  const std::string path = "parser_test_input.json";
  {
    std::ofstream output(path);
    output << R"({"first":7,"operation":"fact"})";
  }

  auto data = make_data();
  Calculator::Initializer initializer(data);
  initializer.init_calculator_data();
  Calculator::Parser parser(data);

  parser.parse_json(path.c_str());

  EXPECT_EQ(data.first_number, 7);
  EXPECT_EQ(data.operation, Calculator::Types::OP_FACT);
  EXPECT_EQ(data.has_first_number, 1);
  EXPECT_EQ(data.has_second_number, 0);

  std::remove(path.c_str());
}

TEST(ParserJsonTest, ThrowsOnInvalidJsonFieldType) {
  auto data = make_data();
  Calculator::Initializer initializer(data);
  initializer.init_calculator_data();
  Calculator::Parser parser(data);

  try {
    parser.parse_json(R"({"first":"ten","operation":"add"})");
    FAIL();
  } catch (const Calculator::CalculatorException& exception) {
    EXPECT_EQ(exception.error_code(), Calculator::Types::ERR_INVALID_JSON);
  }
}

TEST(ParserArgumentsTest, ThrowsOnInvalidNumber) {
  auto data = make_data();
  Calculator::Initializer initializer(data);
  initializer.init_calculator_data();
  Calculator::Parser parser(data);

  const char* argv[] = {"calculator", "-f", "abc", "-o", "fact"};

  try {
    parser.parse_arguments(5, const_cast<char**>(argv));
    FAIL();
  } catch (const Calculator::CalculatorException& exception) {
    EXPECT_EQ(exception.error_code(), Calculator::Types::ERR_INVALID_NUMBER);
  }
}
