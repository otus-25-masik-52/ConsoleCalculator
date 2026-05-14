#pragma once

#include <memory>
#include <string_view>

namespace Calculator {
class Logger final {
 public:
  Logger() = delete;
  ~Logger() = delete;

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  static void init(std::string_view file_path = "logs/console-calculator.log",
                   std::string_view logger_name = default_logger_name());

  static void trace(std::string_view message);
  static void debug(std::string_view message);
  static void info(std::string_view message);
  static void warn(std::string_view message);
  static void error(std::string_view message);
  static void critical(std::string_view message);

  [[nodiscard]] static bool is_initialized() noexcept;
  static void shutdown() noexcept;
  [[nodiscard]] static constexpr std::string_view default_logger_name() noexcept {
    return "console-calculator";
  }

 private:
  class Impl;

  static Impl& impl();
  static void log(std::string_view message, int level);
};
} // namespace Calculator