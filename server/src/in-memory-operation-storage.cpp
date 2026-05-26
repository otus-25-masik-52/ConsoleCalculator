#include "in-memory-operation-storage.h"

#include "operation-key.h"

namespace Calculator::Server {

std::optional<Storage::Types::OperationRecord> InMemoryOperationStorage::find(const int first_number,
                                                                              const std::optional<int> second_number,
                                                                              const std::string& operation) {
  const auto key = Storage::OperationKey::make(first_number, second_number, operation);
  std::scoped_lock const lock{mutex_};
  const auto iterator = records_.find(key);
  if (iterator == records_.end()) {
    return std::nullopt;
  }
  return iterator->second;
}

void InMemoryOperationStorage::save(const Storage::Types::OperationRecord& record) {
  std::scoped_lock const lock{mutex_};
  records_.insert_or_assign(record.cache_key(), record);
}

} // namespace Calculator::Server
