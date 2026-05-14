#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "types/operation-record.h"

namespace Calculator::Storage {
class CacheRepository {
 public:
  CacheRepository() = default;
  ~CacheRepository() = default;

  CacheRepository(const CacheRepository&) = default;
  CacheRepository& operator=(const CacheRepository&) = default;
  CacheRepository(CacheRepository&&) noexcept = default;
  CacheRepository& operator=(CacheRepository&&) noexcept = default;

  void put(const Types::OperationRecord& record);
  void warm_up(const std::vector<Types::OperationRecord>& records);

  [[nodiscard]] std::optional<Types::OperationRecord> find(int first_number, std::optional<int> second_number,
                                                           const std::string& operation) const;
  [[nodiscard]] std::optional<Types::OperationRecord> find_by_key(const std::string& cache_key) const;
  [[nodiscard]] std::size_t size() const noexcept;

 private:
  std::unordered_map<std::string, Types::OperationRecord> cache_;
};
} // namespace Calculator::Storage
