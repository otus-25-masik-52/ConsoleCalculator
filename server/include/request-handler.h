#pragma once

#include "operation-storage.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "calculator-protocol/codes/operation-code.h"
#include "calculator-protocol/codes/status-code.h"
#include "calculator-protocol/models/calculation-request.h"
#include "calculator-protocol/models/calculation-response.h"
#include "types/calculator-data.h"
#include "types/error-code.h"
#include "types/operation-code.h"

namespace Calculator::Server {

class RequestHandler final {
 public:
  explicit RequestHandler(std::shared_ptr<IOperationStorage> storage);

  [[nodiscard]] std::string handle(std::string_view request_json);

 private:
  using ProtocolOperation = CalculatorProtocol::Codes::OperationCode;
  using ProtocolStatus = CalculatorProtocol::Codes::StatusCode;

  [[nodiscard]] static Types::CalculatorData
  to_calculator_data(const CalculatorProtocol::Models::CalculationRequest& request);
  [[nodiscard]] static Types::OperationCode to_internal_operation(ProtocolOperation operation);
  [[nodiscard]] static std::string_view operation_to_storage_name(ProtocolOperation operation);
  [[nodiscard]] static std::optional<int> second_number_for_storage(const Types::CalculatorData& data);
  [[nodiscard]] static bool is_storable_operation(const Types::CalculatorData& data) noexcept;
  [[nodiscard]] static Storage::Types::OperationRecord make_operation_record(const Types::CalculatorData& data,
                                                                             int status, const std::string& operation);
  [[nodiscard]] static ProtocolStatus map_internal_error(int error_code) noexcept;
  [[nodiscard]] static CalculatorProtocol::Types::TimingInfo
  make_timing(std::chrono::steady_clock::time_point started_at) noexcept;
  [[nodiscard]] static CalculatorProtocol::Models::CalculationResponse
  make_failure(std::string request_id, ProtocolStatus status, std::string message);

  std::shared_ptr<IOperationStorage> storage_;
};

} // namespace Calculator::Server
