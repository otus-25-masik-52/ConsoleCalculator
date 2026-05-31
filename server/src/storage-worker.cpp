#include "storage-worker.h"

#include <utility>

#include "cache-repository.h"
#include "logger.h"
#include "postgres-connection.h"
#include "postgres-repository.h"
#include "storage-service.h"

namespace Calculator::Server {

StorageWorker::StorageWorker(std::string database_connection_string)
    : database_connection_string_(std::move(database_connection_string)) {}

StorageWorker::~StorageWorker() {
  stop();
}

void StorageWorker::start() {
  std::promise<void> init_promise;
  auto init_future = init_promise.get_future();

  {
    std::scoped_lock const lock{mutex_};
    stopping_ = false;
  }

  worker_ = std::thread(&StorageWorker::thread_main, this, std::move(init_promise));
  init_future.get();
}

void StorageWorker::stop() noexcept {
  {
    std::scoped_lock const lock{mutex_};
    stopping_ = true;
  }
  condition_.notify_all();

  if (worker_.joinable()) {
    worker_.join();
  }
}

std::optional<Storage::Types::OperationRecord> StorageWorker::find(int first_number, std::optional<int> second_number,
                                                                   const std::string& operation) {
  auto promise = std::make_shared<std::promise<std::optional<Storage::Types::OperationRecord>>>();
  auto future = promise->get_future();

  enqueue([promise, first_number, second_number, operation](const Storage::StorageService& storage) mutable {
    try {
      promise->set_value(storage.find(first_number, second_number, operation));
    } catch (...) {
      promise->set_exception(std::current_exception());
    }
  });

  return future.get();
}

void StorageWorker::save(const Storage::Types::OperationRecord& record) {
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future();

  enqueue([promise, record](Storage::StorageService& storage) mutable {
    try {
      storage.save(record);
      promise->set_value();
    } catch (...) {
      promise->set_exception(std::current_exception());
    }
  });

  future.get();
}

void StorageWorker::enqueue(Job job) {
  {
    std::scoped_lock const lock{mutex_};
    if (stopping_) {
      throw std::runtime_error{"storage worker is stopping"};
    }
    jobs_.push_back(std::move(job));
  }
  condition_.notify_one();
}

void StorageWorker::thread_main(std::promise<void> init_promise) {
  try {
    Storage::PostgresConnection connection(database_connection_string_);
    Storage::PostgresRepository postgres_repository(std::move(connection));
    Storage::CacheRepository cache_repository;
    Storage::StorageService storage_service(std::move(cache_repository), std::move(postgres_repository));
    storage_service.initialize();

    Logger::info("Storage worker initialized.");
    init_promise.set_value();

    while (true) {
      Job job;
      {
        std::unique_lock lock{mutex_};
        condition_.wait(lock, [this] { return stopping_ || !jobs_.empty(); });
        if (stopping_ && jobs_.empty()) {
          break;
        }
        job = std::move(jobs_.front());
        jobs_.pop_front();
      }

      job(storage_service);
    }

    Logger::info("Storage worker stopped.");
  } catch (...) {
    try {
      init_promise.set_exception(std::current_exception());
    } catch (...) {
    }
  }
}

} // namespace Calculator::Server
