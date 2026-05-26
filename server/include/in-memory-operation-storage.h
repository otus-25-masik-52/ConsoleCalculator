#pragma once

#include "operation-storage.h"

#include <mutex>
#include <unordered_map>

namespace Calculator::Server {

class InMemoryOperationStorage final : public IOperationStorage {
 public:
  [[nodiscard]] std::optional<Storage::Types::OperationRecord> find(int first_number, std::optional<int> second_number,
                                                                    const std::string& operation) override;
  void save(const Storage::Types::OperationRecord& record) override;

 private:
  mutable std::mutex mutex_;
  std::unordered_map<std::string, Storage::Types::OperationRecord> records_;
};

} // namespace Calculator::Server
