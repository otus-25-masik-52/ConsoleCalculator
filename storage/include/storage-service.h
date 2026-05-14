#pragma once

#include <optional>
#include <string>

#include "cache-repository.h"
#include "postgres-repository.h"
#include "types/operation-record.h"

namespace Calculator::Storage {
class StorageService {
 public:
  StorageService(CacheRepository cache_repository, PostgresRepository postgres_repository);
  ~StorageService() = default;

  StorageService(const StorageService&) = delete;
  StorageService& operator=(const StorageService&) = delete;
  StorageService(StorageService&&) noexcept = default;
  StorageService& operator=(StorageService&&) noexcept = default;

  void initialize();

  [[nodiscard]] std::optional<Types::OperationRecord> find(int first_number, std::optional<int> second_number,
                                                           const std::string& operation) const;

  void save(const Types::OperationRecord& record);

 private:
  CacheRepository cache_repository_;
  PostgresRepository postgres_repository_;
};
} // namespace Calculator::Storage
