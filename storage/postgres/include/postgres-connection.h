#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "postgres-result.h"

struct pg_conn;

namespace Calculator::Storage {
class PostgresConnection {
 public:
  explicit PostgresConnection(const std::string& connection_string);
  ~PostgresConnection() = default;

  PostgresConnection(const PostgresConnection&) = delete;
  PostgresConnection& operator=(const PostgresConnection&) = delete;
  PostgresConnection(PostgresConnection&&) noexcept = default;
  PostgresConnection& operator=(PostgresConnection&&) noexcept = default;

  [[nodiscard]] bool is_connected() const;
  [[nodiscard]] std::string last_error() const;

  [[nodiscard]] PostgresResult execute(const std::string& sql) const;
  [[nodiscard]] PostgresResult execute_params(const std::string& sql,
                                              const std::vector<std::optional<std::string>>& params) const;

 private:
  struct PgConnectionDeleter {
    void operator()(pg_conn* connection) const noexcept;
  };

  using PgConnectionPtr = std::unique_ptr<pg_conn, PgConnectionDeleter>;

  void ensure_connected() const;

  PgConnectionPtr connection_;
};
} // namespace Calculator::Storage
