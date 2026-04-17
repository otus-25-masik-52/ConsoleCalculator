#include <string>

#include <gtest/gtest.h>

#include "printer.h"
#include "types/error-code.h"

TEST(PrinterTest, PrintsInvalidJsonMessage) {
  testing::internal::CaptureStdout();
  Calculator::Printer::print_error(Calculator::Types::ERR_INVALID_JSON);
  const std::string output = testing::internal::GetCapturedStdout();
  EXPECT_NE(output.find("invalid JSON input"), std::string::npos);
}

TEST(PrinterTest, PrintsHelpWithJsonOption) {
  testing::internal::CaptureStdout();
  Calculator::Printer::print_help();
  const std::string output = testing::internal::GetCapturedStdout();
  EXPECT_NE(output.find("--json"), std::string::npos);
  EXPECT_NE(output.find("path/to/input.json"), std::string::npos);
}
