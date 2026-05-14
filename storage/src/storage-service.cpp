#include "storage-service.h"

#include <utility>

#include "logger.h"

namespace Calculator::Storage {
StorageService::StorageService(CacheRepository cache_repository, PostgresRepository postgres_repository)
    : cache_repository_(std::move(cache_repository)), postgres_repository_(std::move(postgres_repository)) {}

void StorageService::initialize() {
  Logger::info("Storage initialization started.");
  postgres_repository_.init_schema();
  cache_repository_.warm_up(postgres_repository_.load_history());
  Logger::info("Storage initialization finished.");
}

std::optional<Types::OperationRecord> StorageService::find(int first_number, std::optional<int> second_number,
                                                           const std::string& operation) const {
  return cache_repository_.find(first_number, second_number, operation);
}

void StorageService::save(const Types::OperationRecord& record) {
  cache_repository_.put(record);
  postgres_repository_.save(record);
}
} // namespace Calculator::Storage
