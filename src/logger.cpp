#include "logger.h"

#include <filesystem>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace Calculator {
class Logger::Impl final {
 public:
  void init(std::string_view file_path, std::string_view logger_name) {
    if (logger_ != nullptr) {
      return;
    }

    namespace fs = std::filesystem;

    const fs::path log_path{file_path};
    if (log_path.has_parent_path()) {
      std::error_code ec;
      fs::create_directories(log_path.parent_path(), ec);
    }

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%n] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path.string(), true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [pid:%P] [tid:%t] %v");

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(console_sink);
    sinks.emplace_back(file_sink);

    logger_ = std::make_shared<spdlog::logger>(std::string(logger_name), sinks.begin(), sinks.end());
    logger_->set_level(spdlog::level::trace);
    logger_->flush_on(spdlog::level::err);

    spdlog::register_logger(logger_);
    spdlog::set_default_logger(logger_);
    spdlog::set_level(spdlog::level::trace);
  }

  [[nodiscard]] bool is_initialized() const noexcept {
    return static_cast<bool>(logger_);
  }

  void shutdown() noexcept {
    logger_.reset();
    spdlog::shutdown();
  }

  void trace(std::string_view message) {
    logger().trace(message);
  }
  void debug(std::string_view message) {
    logger().debug(message);
  }
  void info(std::string_view message) {
    logger().info(message);
  }
  void warn(std::string_view message) {
    logger().warn(message);
  }
  void error(std::string_view message) {
    logger().error(message);
  }
  void critical(std::string_view message) {
    logger().critical(message);
  }

 private:
  [[nodiscard]] spdlog::logger& logger() {
    if (logger_ == nullptr) {
      init("logs/console-calculator.log", Logger::default_logger_name());
    }
    return *logger_;
  }

  std::shared_ptr<spdlog::logger> logger_;
};

Logger::Impl& Logger::impl() {
  static Impl instance;
  return instance;
}

void Logger::init(std::string_view file_path, std::string_view logger_name) {
  impl().init(file_path, logger_name);
}
void Logger::trace(std::string_view message) {
  impl().trace(message);
}
void Logger::debug(std::string_view message) {
  impl().debug(message);
}
void Logger::info(std::string_view message) {
  impl().info(message);
}
void Logger::warn(std::string_view message) {
  impl().warn(message);
}
void Logger::error(std::string_view message) {
  impl().error(message);
}
void Logger::critical(std::string_view message) {
  impl().critical(message);
}
bool Logger::is_initialized() noexcept {
  return impl().is_initialized();
}
void Logger::shutdown() noexcept {
  impl().shutdown();
}
} // namespace Calculator