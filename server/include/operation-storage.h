#pragma once

#include <optional>
#include <string>

#include "types/operation-record.h"

namespace Calculator::Server {

class IOperationStorage {
 public:
  virtual ~IOperationStorage() = default;

  [[nodiscard]] virtual std::optional<Storage::Types::OperationRecord>
  find(int first_number, std::optional<int> second_number, const std::string& operation) = 0;
  virtual void save(const Storage::Types::OperationRecord& record) = 0;
};

} // namespace Calculator::Server
