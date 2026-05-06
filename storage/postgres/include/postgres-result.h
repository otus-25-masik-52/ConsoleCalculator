#pragma once

#include <memory>
#include <string>

struct pg_result;

namespace Calculator::Storage {
class PostgresConnection;

class PostgresResult {
 public:
  ~PostgresResult() = default;

  PostgresResult(const PostgresResult&) = delete;
  PostgresResult& operator=(const PostgresResult&) = delete;
  PostgresResult(PostgresResult&&) noexcept = default;
  PostgresResult& operator=(PostgresResult&&) noexcept = default;

  void expect_command_ok() const;
  void expect_tuples_ok() const;

  [[nodiscard]] int row_count() const;
  [[nodiscard]] int column_count() const;
  [[nodiscard]] bool is_null(int row, int column) const;
  [[nodiscard]] std::string value(int row, int column) const;

 private:
  friend class PostgresConnection;

  explicit PostgresResult(pg_result* result);

  struct PgResultDeleter {
    void operator()(pg_result* result) const noexcept;
  };

  using PgResultPtr = std::unique_ptr<pg_result, PgResultDeleter>;

  void expect_status(int expected_status, const std::string& expected_status_name) const;
  [[nodiscard]] pg_result* get() const;

  PgResultPtr result_;
};
} // namespace Calculator::Storage
