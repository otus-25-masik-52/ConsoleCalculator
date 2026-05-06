#include "postgres-repository.h"

#include <string>
#include <utility>

#include "logger.h"

namespace Calculator::Storage {
PostgresRepository::PostgresRepository(PostgresConnection connection) : connection_(std::move(connection)) {}

std::optional<int> PostgresRepository::parse_optional_int(const PostgresResult& result, int row, int column) {
  if (result.is_null(row, column)) {
    return std::nullopt;
  }
  return std::stoi(result.value(row, column));
}

std::optional<std::string> PostgresRepository::to_optional_string(const std::optional<int>& value) {
  if (!value.has_value()) {
    return std::nullopt;
  }
  return std::to_string(*value);
}

void PostgresRepository::init_schema() const {
  static constexpr const char* kSql = R"sql(
CREATE TABLE IF NOT EXISTS operation_history (
    id BIGSERIAL PRIMARY KEY,
    first_number INTEGER NOT NULL,
    second_number INTEGER,
    operation TEXT NOT NULL,
    result INTEGER,
    status INTEGER NOT NULL,
    cache_key TEXT NOT NULL UNIQUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
)sql";

  auto result = connection_.execute(kSql);
  result.expect_command_ok();
  Logger::info("PostgreSQL schema initialized.");
}

std::vector<Types::OperationRecord> PostgresRepository::load_history() const {
  static constexpr const char* kSql = R"sql(
SELECT first_number, second_number, operation, result, status, cache_key
FROM operation_history
ORDER BY id ASC;
)sql";

  auto result = connection_.execute(kSql);
  result.expect_tuples_ok();

  std::vector<Types::OperationRecord> records;
  records.reserve(static_cast<std::size_t>(result.row_count()));

  for (int row = 0; row < result.row_count(); ++row) {
    const int first_number = std::stoi(result.value(row, 0));
    const std::optional<int> second_number = parse_optional_int(result, row, 1);
    const std::string operation = result.value(row, 2);
    const std::optional<int> operation_result = parse_optional_int(result, row, 3);
    const int status = std::stoi(result.value(row, 4));
    const std::string cache_key = result.value(row, 5);

    records.emplace_back(first_number, second_number, operation, operation_result, status, cache_key);
  }

  Logger::info("Operation history loaded from PostgreSQL. Records: " + std::to_string(records.size()));
  return records;
}

void PostgresRepository::save(const Types::OperationRecord& record) const {
  static constexpr const char* kSql = R"sql(
INSERT INTO operation_history (first_number, second_number, operation, result, status, cache_key)
VALUES ($1, $2, $3, $4, $5, $6)
ON CONFLICT (cache_key) DO NOTHING;
)sql";

  const std::vector<std::optional<std::string>> params = {
      std::to_string(record.first_number()),
      to_optional_string(record.second_number()),
      record.operation(),
      to_optional_string(record.result()),
      std::to_string(record.status()),
      record.cache_key(),
  };

  auto result = connection_.execute_params(kSql, params);
  result.expect_command_ok();
  Logger::info("Operation saved to PostgreSQL with key: " + record.cache_key());
}
} // namespace Calculator::Storage
