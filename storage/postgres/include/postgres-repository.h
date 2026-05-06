#pragma once

#include <optional>
#include <string>
#include <vector>

#include "postgres-connection.h"
#include "types/operation-record.h"

namespace Calculator::Storage {
class PostgresRepository {
 public:
  explicit PostgresRepository(PostgresConnection connection);
  ~PostgresRepository() = default;

  PostgresRepository(const PostgresRepository&) = delete;
  PostgresRepository& operator=(const PostgresRepository&) = delete;
  PostgresRepository(PostgresRepository&&) noexcept = default;
  PostgresRepository& operator=(PostgresRepository&&) noexcept = default;

  void init_schema() const;
  [[nodiscard]] std::vector<Types::OperationRecord> load_history() const;
  void save(const Types::OperationRecord& record) const;

 private:
  [[nodiscard]] static std::optional<int> parse_optional_int(const PostgresResult& result, int row, int column);
  [[nodiscard]] static std::optional<std::string> to_optional_string(const std::optional<int>& value);

  PostgresConnection connection_;
};
} // namespace Calculator::Storage
