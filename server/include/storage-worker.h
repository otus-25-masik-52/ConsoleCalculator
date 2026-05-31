#pragma once

#include "operation-storage.h"

#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <thread>

namespace Calculator::Storage {
class StorageService;
}

namespace Calculator::Server {

class StorageWorker final : public IOperationStorage {
 public:
  explicit StorageWorker(std::string database_connection_string);
  ~StorageWorker() override;

  StorageWorker(const StorageWorker&) = delete;
  StorageWorker& operator=(const StorageWorker&) = delete;
  StorageWorker(StorageWorker&&) = delete;
  StorageWorker& operator=(StorageWorker&&) = delete;

  void start();
  void stop() noexcept;

  [[nodiscard]] std::optional<Storage::Types::OperationRecord> find(int first_number, std::optional<int> second_number,
                                                                    const std::string& operation) override;
  void save(const Storage::Types::OperationRecord& record) override;

 private:
  using Job = std::function<void(Storage::StorageService&)>;

  void thread_main(std::promise<void> init_promise);
  void enqueue(Job job);

  std::string database_connection_string_;

  std::mutex mutex_;
  std::condition_variable condition_;
  std::deque<Job> jobs_;
  bool stopping_{false};
  std::thread worker_;
};

} // namespace Calculator::Server
