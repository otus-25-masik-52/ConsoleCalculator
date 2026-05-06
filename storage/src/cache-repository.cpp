#include "cache-repository.h"

#include "logger.h"
#include "operation-key.h"

namespace Calculator::Storage {
void CacheRepository::put(const Types::OperationRecord& record) {
  cache_.insert_or_assign(record.cache_key(), record);
  Logger::debug("Operation saved to cache with key: " + record.cache_key());
}

void CacheRepository::warm_up(const std::vector<Types::OperationRecord>& records) {
  for (const auto& record : records) {
    put(record);
  }

  Logger::info("Cache warm-up completed. Records loaded: " + std::to_string(cache_.size()));
}

std::optional<Types::OperationRecord> CacheRepository::find(int first_number, std::optional<int> second_number,
                                                            const std::string& operation) const {
  return find_by_key(OperationKey::make(first_number, second_number, operation));
}

std::optional<Types::OperationRecord> CacheRepository::find_by_key(const std::string& cache_key) const {
  const auto iterator = cache_.find(cache_key);
  if (iterator == cache_.end()) {
    Logger::debug("Cache miss for key: " + cache_key);
    return std::nullopt;
  }

  Logger::info("Cache hit for key: " + cache_key);
  return iterator->second;
}

std::size_t CacheRepository::size() const noexcept {
  return cache_.size();
}
} // namespace Calculator::Storage
