#include "server-config.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Calculator::Server {
namespace {

std::string trim(std::string value) {
  const auto is_space = [](unsigned char ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; };
  value.erase(value.begin(), std::ranges::find_if(value, [&](char ch) { return !is_space(ch); }));
  value.erase(std::ranges::find_if(std::views::reverse(value), [&](char ch) { return !is_space(ch); }).base(),
              value.end());
  return value;
}

std::string unquote(std::string value) {
  value = trim(std::move(value));
  if (value.size() >= 2 &&
      ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\''))) {
    return value.substr(1, value.size() - 2);
  }
  return value;
}

std::string read_text_file(const std::string& path) {
  std::ifstream input(path);
  if (!input.is_open()) {
    return {};
  }

  std::ostringstream output;
  output << input.rdbuf();
  return trim(output.str());
}

std::string libpq_escape_value(const std::string& value) {
  std::string escaped;
  escaped.reserve(value.size() + 2);
  escaped.push_back('\'');
  for (const char ch : value) {
    if (ch == '\\' || ch == '\'') {
      escaped.push_back('\\');
    }
    escaped.push_back(ch);
  }
  escaped.push_back('\'');
  return escaped;
}

const char* environment_value(const char* name) noexcept {
  const char* value = std::getenv(name);
  if (value == nullptr || *value == '\0') {
    return nullptr;
  }
  return value;
}

std::uint16_t parse_uint16(const std::string& value, const std::string& field_name) {
  const auto parsed = std::stoul(value);
  if (parsed > 65535U) {
    throw std::runtime_error{field_name + " is out of uint16 range"};
  }
  return static_cast<std::uint16_t>(parsed);
}

void apply_key_value(ServerConfig& config, const std::string& key, const std::string& value) {
  if (key == "host") {
    config.set_host(value);
  } else if (key == "port") {
    config.set_port(parse_uint16(value, "port"));
  } else if (key == "worker_threads") {
    config.set_worker_threads(std::stoi(value));
  } else if (key == "max_message_size") {
    config.set_max_message_size(static_cast<std::size_t>(std::stoull(value)));
  } else if (key == "log_file") {
    config.set_log_file(value);
  } else if (key == "database_host") {
    config.set_database_host(value);
  } else if (key == "database_port") {
    config.set_database_port(parse_uint16(value, "database_port"));
  } else if (key == "database_name") {
    config.set_database_name(value);
  } else if (key == "database_user") {
    config.set_database_user(value);
  } else if (key == "database_password") {
    config.set_database_password(value);
  } else if (key == "database_password_file") {
    config.set_database_password_file(value);
  } else if (key == "database_connection_string") {
    config.set_explicit_database_connection_string(value);
  }
}

} // namespace

std::string ServerConfig::database_connection_string() const {
  if (!explicit_database_connection_string_.empty()) {
    return explicit_database_connection_string_;
  }

  std::string password = database_password_;
  if (password.empty() && !database_password_file_.empty()) {
    password = read_text_file(database_password_file_);
  }

  std::ostringstream output;
  output << "host=" << libpq_escape_value(database_host_) << " port=" << database_port_
         << " dbname=" << libpq_escape_value(database_name_) << " user=" << libpq_escape_value(database_user_);

  if (!password.empty()) {
    output << " password=" << libpq_escape_value(password);
  }

  return output.str();
}

ServerConfig ServerConfig::from_file(const std::string& path) {
  ServerConfig config;

  std::ifstream input(path);
  if (!input.is_open()) {
    return config;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (auto comment = line.find('#'); comment != std::string::npos) {
      line.erase(comment);
    }

    line = trim(std::move(line));
    if (line.empty()) {
      continue;
    }

    const auto separator = line.find('=');
    if (separator == std::string::npos) {
      continue;
    }

    auto key = trim(line.substr(0, separator));
    auto value = unquote(line.substr(separator + 1));
    apply_key_value(config, key, value);
  }

  return config;
}

ServerConfig ServerConfig::from_environment_and_file(const std::string& path) {
  ServerConfig config = from_file(path);

  if (const char* value = environment_value("CC_SERVER_HOST")) {
    config.set_host(value);
  }
  if (const char* value = environment_value("CC_SERVER_PORT")) {
    config.set_port(parse_uint16(value, "CC_SERVER_PORT"));
  }
  if (const char* value = environment_value("CC_SERVER_WORKER_THREADS")) {
    config.set_worker_threads(std::stoi(value));
  }
  if (const char* value = environment_value("CC_SERVER_MAX_MESSAGE_SIZE")) {
    config.set_max_message_size(static_cast<std::size_t>(std::stoull(value)));
  }
  if (const char* value = environment_value("CC_SERVER_LOG_FILE")) {
    config.set_log_file(value);
  }

  if (const char* value = environment_value("CC_DB_HOST")) {
    config.set_database_host(value);
  }
  if (const char* value = environment_value("CC_DB_PORT")) {
    config.set_database_port(parse_uint16(value, "CC_DB_PORT"));
  }
  if (const char* value = environment_value("CC_DB_NAME")) {
    config.set_database_name(value);
  }
  if (const char* value = environment_value("CC_DB_USER")) {
    config.set_database_user(value);
  }
  if (const char* value = environment_value("CC_DB_PASSWORD")) {
    config.set_database_password(value);
  }
  if (const char* value = environment_value("CC_DB_PASSWORD_FILE")) {
    config.set_database_password_file(value);
  }
  if (const char* value = environment_value("CC_DB_CONNECTION_STRING")) {
    config.set_explicit_database_connection_string(value);
  }

  return config;
}

} // namespace Calculator::Server
