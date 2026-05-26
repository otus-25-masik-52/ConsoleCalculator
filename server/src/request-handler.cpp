#include "request-handler.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <utility>

#include "abacus.h"
#include "calculator-protocol/errors/protocol-error.h"
#include "calculator-protocol/formats/json-codec.h"
#include "calculator-protocol/types/result-info.h"
#include "calculator-protocol/types/result-source.h"
#include "checker.h"
#include "exception.h"
#include "logger.h"
#include "operation-key.h"

namespace Calculator::Server {

using std::string_literals::operator""s;

namespace {

bool fits_int64_to_int(const std::int64_t value) noexcept {
  return value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max();
}

} // namespace

RequestHandler::RequestHandler(std::shared_ptr<IOperationStorage> storage) : storage_(std::move(storage)) {
  if (!storage_) {
    throw std::invalid_argument{"storage must not be null"};
  }
}

std::string RequestHandler::handle(const std::string_view request_json) {
  const auto started_at = std::chrono::steady_clock::now();
  std::string request_id;

  try {
    auto request = CalculatorProtocol::Formats::JsonCodec::parse_request(request_json);
    request_id = request.request_id();

    auto data = to_calculator_data(request);
    const auto operation = operation_to_storage_name(request.operation());
    const auto second_number = second_number_for_storage(data);

    if (const auto cached = storage_->find(data.first_number, second_number, std::string(operation));
        cached.has_value()) {
      auto response = cached->status() == static_cast<int>(Types::ErrorCode::ERROR_NONE)
                          ? CalculatorProtocol::Models::CalculationResponse::success(
                                request_id, CalculatorProtocol::Types::ResultInfo{*cached->result()})
                          : make_failure(request_id, map_internal_error(cached->status()), "cached calculation error");

      response.set_result_source(CalculatorProtocol::Types::ResultSource::CACHE);
      response.set_timing_info(make_timing(started_at));
      return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
    }

    Checker checker(data);
    checker.check_arguments();

    Abacus abacus(data);
    try {
      abacus.calculate_result();
      data.error_code = Types::ErrorCode::ERROR_NONE;
      storage_->save(
          make_operation_record(data, static_cast<int>(Types::ErrorCode::ERROR_NONE), std::string(operation)));

      auto response = CalculatorProtocol::Models::CalculationResponse::success(
          request_id, CalculatorProtocol::Types::ResultInfo{data.result});
      response.set_result_source(CalculatorProtocol::Types::ResultSource::CALCULATED);
      response.set_timing_info(make_timing(started_at));
      return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
    } catch (const CalculatorException& exception) {
      data.error_code = static_cast<Types::ErrorCode>(exception.error_code());
      if (is_storable_operation(data)) {
        try {
          storage_->save(make_operation_record(data, exception.error_code(), std::string(operation)));
        } catch (const std::exception& storage_exception) {
          Logger::error(std::string{"Failed to save failed operation: "} + storage_exception.what());
        }
      }
      throw;
    }
  } catch (const CalculatorProtocol::Errors::ProtocolError& error) {
    auto response = make_failure(request_id, error.status_code(), error.what());
    if (error.field().has_value() && response.error_info().has_value()) {
      auto error_info = *response.error_info();
      error_info.set_field(error.field());
      response.set_error_info(std::move(error_info));
    }
    response.set_timing_info(make_timing(started_at));
    return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
  } catch (const CalculatorException& error) {
    auto response = make_failure(request_id, map_internal_error(error.error_code()), error.what());
    response.set_timing_info(make_timing(started_at));
    return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
  } catch (const std::exception& error) {
    auto response = make_failure(request_id, ProtocolStatus::INTERNAL_ERROR, error.what());
    response.set_timing_info(make_timing(started_at));
    return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
  } catch (...) {
    auto response = make_failure(request_id, ProtocolStatus::UNKNOWN_ERROR, "unknown server error");
    response.set_timing_info(make_timing(started_at));
    return CalculatorProtocol::Formats::JsonCodec::serialize_response(response);
  }
}

Types::CalculatorData
RequestHandler::to_calculator_data(const CalculatorProtocol::Models::CalculationRequest& request) {
  Types::CalculatorData data;
  data.operation = to_internal_operation(request.operation());
  data.need_help = 0;

  const auto& operands = request.operands();
  if (!operands.empty()) {
    if (!fits_int64_to_int(operands[0])) {
      throw CalculatorProtocol::Errors::ProtocolError{ProtocolStatus::OPERAND_OUT_OF_RANGE,
                                                      "first operand is outside int range", "operands[0]"s};
    }
    data.first_number = static_cast<int>(operands[0]);
    data.has_first_number = 1;
  }

  if (operands.size() > 1) {
    if (!fits_int64_to_int(operands[1])) {
      throw CalculatorProtocol::Errors::ProtocolError{ProtocolStatus::OPERAND_OUT_OF_RANGE,
                                                      "second operand is outside int range", "operands[1]"s};
    }
    data.second_number = static_cast<int>(operands[1]);
    data.has_second_number = 1;
  }

  return data;
}

Types::OperationCode RequestHandler::to_internal_operation(const ProtocolOperation operation) {
  switch (operation) {
  case ProtocolOperation::ADDITION:
    return Types::OperationCode::OPERATION_ADDITION;
  case ProtocolOperation::SUBTRACTION:
    return Types::OperationCode::OPERATION_SUBTRACTION;
  case ProtocolOperation::MULTIPLICATION:
    return Types::OperationCode::OPERATION_MULTIPLICATION;
  case ProtocolOperation::DIVISION:
    return Types::OperationCode::OPERATION_DIVISION;
  case ProtocolOperation::EXPONENTIATION:
    return Types::OperationCode::OPERATION_EXPONENTIATION;
  case ProtocolOperation::FACTORIAL:
    return Types::OperationCode::OPERATION_FACTORIAL;
  case ProtocolOperation::UNKNOWN:
  default:
    throw CalculatorProtocol::Errors::ProtocolError{ProtocolStatus::INVALID_OPERATION, "unknown operation",
                                                    "operation"s};
  }
}

std::string_view RequestHandler::operation_to_storage_name(const ProtocolOperation operation) {
  switch (operation) {
  case ProtocolOperation::ADDITION:
    return "add";
  case ProtocolOperation::SUBTRACTION:
    return "sub";
  case ProtocolOperation::MULTIPLICATION:
    return "mul";
  case ProtocolOperation::DIVISION:
    return "div";
  case ProtocolOperation::EXPONENTIATION:
    return "pow";
  case ProtocolOperation::FACTORIAL:
    return "fact";
  case ProtocolOperation::UNKNOWN:
  default:
    throw CalculatorProtocol::Errors::ProtocolError{ProtocolStatus::INVALID_OPERATION, "unknown operation",
                                                    "operation"s};
  }
}

std::optional<int> RequestHandler::second_number_for_storage(const Types::CalculatorData& data) {
  if (data.operation == Types::OperationCode::OPERATION_FACTORIAL || data.has_second_number == 0) {
    return std::nullopt;
  }
  return data.second_number;
}

bool RequestHandler::is_storable_operation(const Types::CalculatorData& data) noexcept {
  if (data.need_help != 0 || data.operation == Types::OperationCode::OPERATION_NONE || data.has_first_number == 0) {
    return false;
  }
  return data.operation == Types::OperationCode::OPERATION_FACTORIAL || data.has_second_number != 0;
}

Storage::Types::OperationRecord RequestHandler::make_operation_record(const Types::CalculatorData& data,
                                                                      const int status, const std::string& operation) {
  const auto second_number = second_number_for_storage(data);
  const auto result =
      status == static_cast<int>(Types::ErrorCode::ERROR_NONE) ? std::optional<int>{data.result} : std::nullopt;
  const auto cache_key = Storage::OperationKey::make(data.first_number, second_number, operation);
  return Storage::Types::OperationRecord{data.first_number, second_number, operation, result, status, cache_key};
}

RequestHandler::ProtocolStatus RequestHandler::map_internal_error(const int error_code) noexcept {
  switch (static_cast<Types::ErrorCode>(error_code)) {
  case Types::ErrorCode::ERROR_NONE:
    return ProtocolStatus::OK;
  case Types::ErrorCode::ERROR_MISSING_FIRST_NUMBER:
  case Types::ErrorCode::ERROR_MISSING_SECOND_NUMBER:
    return ProtocolStatus::OPERANDS_COUNT_MISMATCH;
  case Types::ErrorCode::ERROR_INVALID_NUMBER:
    return ProtocolStatus::INVALID_OPERANDS;
  case Types::ErrorCode::ERROR_MISSING_OPERATION:
  case Types::ErrorCode::ERROR_INVALID_OPERATION:
    return ProtocolStatus::INVALID_OPERATION;
  case Types::ErrorCode::ERROR_DIVISION_BY_ZERO:
    return ProtocolStatus::DIVISION_BY_ZERO;
  case Types::ErrorCode::ERROR_NEGATIVE_FACTORIAL:
    return ProtocolStatus::FACTORIAL_OF_NEGATIVE_NUMBER;
  case Types::ErrorCode::ERROR_OVERFLOW:
    return ProtocolStatus::NUMERIC_OVERFLOW;
  case Types::ErrorCode::ERROR_STORAGE:
    return ProtocolStatus::STORAGE_ERROR;
  case Types::ErrorCode::ERROR_INVALID_JSON:
    return ProtocolStatus::INVALID_JSON;
  case Types::ErrorCode::ERROR_NEGATIVE_POWER:
    return ProtocolStatus::INVALID_OPERANDS;
  case Types::ErrorCode::ERROR_UNKNOWN_OPTION:
  default:
    return ProtocolStatus::CALCULATION_ERROR;
  }
}

CalculatorProtocol::Types::TimingInfo
RequestHandler::make_timing(const std::chrono::steady_clock::time_point started_at) noexcept {
  const auto elapsed = std::chrono::steady_clock::now() - started_at;
  const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  CalculatorProtocol::Types::TimingInfo timing;
  timing.set_total_duration_ms(static_cast<std::uint64_t>(std::max<std::int64_t>(0, millis)));
  return timing;
}

CalculatorProtocol::Models::CalculationResponse
RequestHandler::make_failure(std::string request_id, const ProtocolStatus status, std::string message) {
  if (request_id.empty()) {
    request_id = "unknown";
  }
  return CalculatorProtocol::Models::CalculationResponse::failure(std::move(request_id), status, std::move(message));
}

} // namespace Calculator::Server
