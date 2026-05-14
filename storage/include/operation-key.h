#pragma once

#include <optional>
#include <string>

namespace Calculator::Storage {
class OperationKey final {
 public:
  OperationKey() = delete;
  ~OperationKey() = delete;

  OperationKey(const OperationKey&) = delete;
  OperationKey& operator=(const OperationKey&) = delete;
  OperationKey(OperationKey&&) = delete;
  OperationKey& operator=(OperationKey&&) = delete;

  [[nodiscard]] static std::string make(int first_number, std::optional<int> second_number,
                                        const std::string& operation);

 private:
  [[nodiscard]] static bool is_commutative(const std::string& operation);
  [[nodiscard]] static std::string symbol_for(const std::string& operation);
  [[nodiscard]] static bool is_factorial(const std::string& operation);
};
} // namespace Calculator::Storage
