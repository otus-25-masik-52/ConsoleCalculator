#include "postgres-connection.h"

#include <libpq-fe.h>

#include <vector>

#include "exception.h"
#include "logger.h"
#include "types/error-code.h"

namespace Calculator::Storage {
PostgresConnection::PostgresConnection(const std::string& connection_string)
    : connection_(PQconnectdb(connection_string.c_str())) {
  ensure_connected();
  Logger::info("PostgreSQL connection established.");
}

void PostgresConnection::PgConnectionDeleter::operator()(pg_conn* connection) const noexcept {
  if (connection != nullptr) {
    PQfinish(connection);
  }
}

bool PostgresConnection::is_connected() const {
  return connection_ != nullptr && PQstatus(connection_.get()) == CONNECTION_OK;
}

std::string PostgresConnection::last_error() const {
  if (connection_ == nullptr) {
    return "PostgreSQL connection is null.";
  }

  const char* message = PQerrorMessage(connection_.get());
  return message != nullptr ? std::string(message) : std::string{};
}

void PostgresConnection::ensure_connected() const {
  if (!is_connected()) {
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_STORAGE),
                              "PostgreSQL connection failed: " + last_error());
  }
}

PostgresResult PostgresConnection::execute(const std::string& sql) const {
  ensure_connected();
  return PostgresResult(PQexec(connection_.get(), sql.c_str()));
}

PostgresResult PostgresConnection::execute_params(const std::string& sql,
                                                  const std::vector<std::optional<std::string>>& params) const {
  ensure_connected();

  std::vector<const char*> values;
  values.reserve(params.size());

  for (const auto& parameter : params) {
    values.push_back(parameter.has_value() ? parameter->c_str() : nullptr);
  }

  return PostgresResult(PQexecParams(connection_.get(), sql.c_str(), static_cast<int>(values.size()), nullptr,
                                     values.data(), nullptr, nullptr, 0));
}
} // namespace Calculator::Storage
