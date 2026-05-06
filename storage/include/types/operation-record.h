#pragma once

#include <optional>
#include <string>
#include <utility>

namespace Calculator::Storage::Types {
class OperationRecord {
 public:
  OperationRecord(int first_number, std::optional<int> second_number, std::string operation,
                  std::optional<int> result, int status, std::string cache_key)
      : first_number_(first_number),
        second_number_(second_number),
        operation_(std::move(operation)),
        result_(result),
        status_(status),
        cache_key_(std::move(cache_key)) {}

  [[nodiscard]] int first_number() const noexcept { return first_number_; }
  [[nodiscard]] const std::optional<int>& second_number() const noexcept { return second_number_; }
  [[nodiscard]] const std::string& operation() const noexcept { return operation_; }
  [[nodiscard]] const std::optional<int>& result() const noexcept { return result_; }
  [[nodiscard]] int status() const noexcept { return status_; }
  [[nodiscard]] const std::string& cache_key() const noexcept { return cache_key_; }

 private:
  int first_number_ = 0;
  std::optional<int> second_number_;
  std::string operation_;
  std::optional<int> result_;
  int status_ = 0;
  std::string cache_key_;
};
} // namespace Calculator::Storage::Types
