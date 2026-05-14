#pragma once

#include <optional>
#include <string>

#include "types/calculator-data.h"
#include "types/operation-code.h"
#include "types/operation-record.h"

namespace Calculator {
class Runner {
 public:
  static int run(int argc, char** argv);

 private:
  [[nodiscard]] static std::string environment_value(const char* name, const std::string& default_value);
  [[nodiscard]] static std::string read_secret_file(const std::string& path);
  [[nodiscard]] static std::string database_password();
  [[nodiscard]] static std::string database_connection_string();

  [[nodiscard]] static std::string operation_to_storage_name(Types::OperationCode operation);
  [[nodiscard]] static std::optional<int> second_number_for_storage(const Types::CalculatorData& data);
  [[nodiscard]] static bool is_storable_operation(const Types::CalculatorData& data);

  [[nodiscard]] static Storage::Types::OperationRecord make_operation_record(const Types::CalculatorData& data,
                                                                             int status);
  static void apply_operation_record(Types::CalculatorData& data, const Storage::Types::OperationRecord& record);
};
} // namespace Calculator
