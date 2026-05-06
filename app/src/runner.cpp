#include "runner.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <utility>

#include "abacus.h"
#include "cache-repository.h"
#include "checker.h"
#include "exception.h"
#include "logger.h"
#include "operation-key.h"
#include "parser.h"
#include "postgres-connection.h"
#include "postgres-repository.h"
#include "printer.h"
#include "storage-service.h"
#include "types/error-code.h"

namespace Calculator {
std::string Runner::environment_value(const char* name, const std::string& default_value) {
  const char* value = std::getenv(name);
  if (value == nullptr || *value == '\0') {
    return default_value;
  }
  return value;
}

std::string Runner::read_secret_file(const std::string& path) {
  std::ifstream input(path);
  if (!input.is_open()) {
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_STORAGE), "Cannot open secret file: " + path);
  }

  std::ostringstream buffer;
  buffer << input.rdbuf();

  std::string secret = buffer.str();
  while (!secret.empty() && (secret.back() == '\n' || secret.back() == '\r' || secret.back() == ' ' || secret.back() == '\t')) {
    secret.pop_back();
  }

  return secret;
}

std::string Runner::database_password() {
  const std::string password_file = environment_value("CC_DB_PASSWORD_FILE", "");
  if (!password_file.empty()) {
    return read_secret_file(password_file);
  }

  return environment_value("CC_DB_PASSWORD", "local_dev_password");
}

std::string Runner::database_connection_string() {
  const std::string host = environment_value("CC_DB_HOST", "localhost");
  const std::string port = environment_value("CC_DB_PORT", "5432");
  const std::string db_name = environment_value("CC_DB_NAME", "console_calculator");
  const std::string user = environment_value("CC_DB_USER", "cc");
  const std::string password = database_password();

  return "host=" + host + " port=" + port + " dbname=" + db_name + " user=" + user + " password=" + password;
}

std::string Runner::operation_to_storage_name(Types::OperationCode operation) {
  switch (operation) {
  case Types::OperationCode::OP_ADD:
    return "add";
  case Types::OperationCode::OP_SUB:
    return "sub";
  case Types::OperationCode::OP_MUL:
    return "mul";
  case Types::OperationCode::OP_DIV:
    return "div";
  case Types::OperationCode::OP_POW:
    return "pow";
  case Types::OperationCode::OP_FACT:
    return "fact";
  case Types::OperationCode::OP_NONE:
  default:
    throw CalculatorException(static_cast<int>(Types::ErrorCode::ERR_INVALID_OPERATION),
                              "Operation cannot be converted to storage name.");
  }
}

std::optional<int> Runner::second_number_for_storage(const Types::CalculatorData& data) {
  if (data.operation == Types::OperationCode::OP_FACT) {
    return std::nullopt;
  }

  if (data.has_second_number == 0) {
    return std::nullopt;
  }

  return data.second_number;
}

bool Runner::is_storable_operation(const Types::CalculatorData& data) {
  if (data.need_help != 0 || data.operation == Types::OperationCode::OP_NONE || data.has_first_number == 0) {
    return false;
  }

  return data.operation == Types::OperationCode::OP_FACT || data.has_second_number != 0;
}

Storage::Types::OperationRecord Runner::make_operation_record(const Types::CalculatorData& data, int status) {
  const std::string operation = operation_to_storage_name(data.operation);
  const std::optional<int> second_number = second_number_for_storage(data);
  const std::optional<int> result = status == static_cast<int>(Types::ErrorCode::ERR_NONE)
                                        ? std::optional<int>{data.result}
                                        : std::nullopt;
  const std::string cache_key = Storage::OperationKey::make(data.first_number, second_number, operation);

  return Storage::Types::OperationRecord(data.first_number, second_number, operation, result, status, cache_key);
}

void Runner::apply_operation_record(Types::CalculatorData& data, const Storage::Types::OperationRecord& record) {
  data.error_code = static_cast<Types::ErrorCode>(record.status());
  if (record.result().has_value()) {
    data.result = *record.result();
  }
}

int Runner::run(int argc, char** argv) {
  Types::CalculatorData data;

  Logger::init();
  Logger::info("Application started.");

  Abacus abacus(data);
  Checker checker(data);
  Parser parser(data);
  Printer printer(data);

  try {
    parser.parse_arguments(argc, argv);

    if (data.need_help != 0) {
      printer.print_result();
      Logger::info("Application finished after help output.");
      Logger::shutdown();
      return static_cast<int>(Types::ErrorCode::ERR_NONE);
    }

    Storage::PostgresConnection connection(database_connection_string());
    Storage::PostgresRepository postgres_repository(std::move(connection));
    Storage::CacheRepository cache_repository;
    Storage::StorageService storage_service(std::move(cache_repository), std::move(postgres_repository));
    storage_service.initialize();

    checker.check_arguments();

    const std::string operation = operation_to_storage_name(data.operation);
    const std::optional<int> second_number = second_number_for_storage(data);

    const auto cached_record = storage_service.find(data.first_number, second_number, operation);
    if (cached_record.has_value()) {
      apply_operation_record(data, *cached_record);
      printer.print_result();
      Logger::info("Application finished with cached result.");
      Logger::shutdown();
      return static_cast<int>(data.error_code);
    }

    try {
      abacus.calculate_result();
      data.error_code = Types::ErrorCode::ERR_NONE;
      storage_service.save(make_operation_record(data, static_cast<int>(Types::ErrorCode::ERR_NONE)));
    } catch (const CalculatorException& exception) {
      data.error_code = static_cast<Types::ErrorCode>(exception.error_code());
      if (is_storable_operation(data)) {
        try {
          storage_service.save(make_operation_record(data, exception.error_code()));
        } catch (const std::exception& storage_exception) {
          Logger::error(std::string("Failed to save failed operation to storage: ") + storage_exception.what());
        }
      }
      throw;
    }

    printer.print_result();

    Logger::info("Application finished successfully.");
    Logger::shutdown();
    return static_cast<int>(Types::ErrorCode::ERR_NONE);
  } catch (const CalculatorException& exception) {
    data.error_code = static_cast<Types::ErrorCode>(exception.error_code());
    Logger::error(std::string("Application failed: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  } catch (const std::exception& exception) {
    data.error_code = Types::ErrorCode::ERR_UNKNOWN_OPTION;
    Logger::critical(std::string("Unhandled std::exception: ") + exception.what());
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  } catch (...) {
    data.error_code = Types::ErrorCode::ERR_UNKNOWN_OPTION;
    Logger::critical("Unhandled unknown exception.");
    printer.print_result();
    Logger::shutdown();
    return static_cast<int>(data.error_code);
  }
}
} // namespace Calculator
