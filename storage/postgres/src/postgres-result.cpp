#include "postgres-result.h"

#include <libpq-fe.h>

#include "exception.h"
#include "types/error-code.h"

namespace Calculator::Storage {
PostgresResult::PostgresResult(pg_result* result) : result_(result) {
  if (result_ == nullptr) {
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_STORAGE), "PostgreSQL result was not created.");
  }
}

void PostgresResult::PgResultDeleter::operator()(pg_result* result) const noexcept {
  if (result != nullptr) {
    PQclear(result);
  }
}

pg_result* PostgresResult::get() const {
  if (result_ == nullptr) {
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_STORAGE), "PostgreSQL result is empty.");
  }
  return result_.get();
}

void PostgresResult::expect_status(int expected_status, const std::string& expected_status_name) const {
  const auto actual_status = PQresultStatus(get());
  if (actual_status != expected_status) {
    const char* error_message = PQresultErrorMessage(get());
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_STORAGE),
                              "Unexpected PostgreSQL result status. Expected " + expected_status_name +
                                  ", got " + std::string(PQresStatus(actual_status)) + ". " +
                                  (error_message != nullptr ? std::string(error_message) : std::string{}));
  }
}

void PostgresResult::expect_command_ok() const {
  expect_status(PGRES_COMMAND_OK, "PGRES_COMMAND_OK");
}

void PostgresResult::expect_tuples_ok() const {
  expect_status(PGRES_TUPLES_OK, "PGRES_TUPLES_OK");
}

int PostgresResult::row_count() const {
  return PQntuples(get());
}

int PostgresResult::column_count() const {
  return PQnfields(get());
}

bool PostgresResult::is_null(int row, int column) const {
  return PQgetisnull(get(), row, column) != 0;
}

std::string PostgresResult::value(int row, int column) const {
  if (is_null(row, column)) {
    return {};
  }

  const char* text = PQgetvalue(get(), row, column);
  return text != nullptr ? std::string(text) : std::string{};
}
} // namespace Calculator::Storage
