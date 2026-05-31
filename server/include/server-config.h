#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace Calculator::Server {

class ServerConfig final {
 public:
  [[nodiscard]] const std::string& host() const noexcept {
    return host_;
  }

  [[nodiscard]] std::uint16_t port() const noexcept {
    return port_;
  }

  [[nodiscard]] int worker_threads() const noexcept {
    return worker_threads_;
  }

  [[nodiscard]] std::size_t max_message_size() const noexcept {
    return max_message_size_;
  }

  [[nodiscard]] const std::string& log_file() const noexcept {
    return log_file_;
  }

  [[nodiscard]] const std::string& database_host() const noexcept {
    return database_host_;
  }

  [[nodiscard]] std::uint16_t database_port() const noexcept {
    return database_port_;
  }

  [[nodiscard]] const std::string& database_name() const noexcept {
    return database_name_;
  }

  [[nodiscard]] const std::string& database_user() const noexcept {
    return database_user_;
  }

  [[nodiscard]] const std::string& database_password() const noexcept {
    return database_password_;
  }

  [[nodiscard]] const std::string& database_password_file() const noexcept {
    return database_password_file_;
  }

  [[nodiscard]] const std::string& explicit_database_connection_string() const noexcept {
    return explicit_database_connection_string_;
  }

  [[nodiscard]] std::string database_connection_string() const;

  void set_host(std::string value) {
    host_ = std::move(value);
  }

  void set_port(const std::uint16_t value) noexcept {
    port_ = value;
  }

  void set_worker_threads(const int value) noexcept {
    worker_threads_ = value;
  }

  void set_max_message_size(const std::size_t value) noexcept {
    max_message_size_ = value;
  }

  void set_log_file(std::string value) {
    log_file_ = std::move(value);
  }

  void set_database_host(std::string value) {
    database_host_ = std::move(value);
  }
  void set_database_port(const std::uint16_t value) noexcept {
    database_port_ = value;
  }
  void set_database_name(std::string value) {
    database_name_ = std::move(value);
  }
  void set_database_user(std::string value) {
    database_user_ = std::move(value);
  }
  void set_database_password(std::string value) {
    database_password_ = std::move(value);
  }
  void set_database_password_file(std::string value) {
    database_password_file_ = std::move(value);
  }
  void set_explicit_database_connection_string(std::string value) {
    explicit_database_connection_string_ = std::move(value);
  }

  static ServerConfig from_file(const std::string& path);
  static ServerConfig from_environment_and_file(const std::string& path);

 private:
  std::string host_{"0.0.0.0"};
  std::uint16_t port_{5555};
  int worker_threads_{4};
  std::size_t max_message_size_{(64 * 1024)};
  std::string log_file_{"/var/log/console-calculator/server.log"};

  std::string database_host_{"localhost"};
  std::uint16_t database_port_{5432};
  std::string database_name_{"console_calculator"};
  std::string database_user_{"admin"};
  std::string database_password_;
  std::string database_password_file_{"/run/secrets/cc_db_password"};
  std::string explicit_database_connection_string_;
};

} // namespace Calculator::Server
