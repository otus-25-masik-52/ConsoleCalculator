#include <gtest/gtest.h>

#include <memory>

#include "calculator-protocol/codes/operation-code.h"
#include "calculator-protocol/codes/status-code.h"
#include "calculator-protocol/formats/json-codec.h"
#include "calculator-protocol/models/calculation-request.h"
#include "calculator-protocol/types/result-source.h"
#include "in-memory-operation-storage.h"
#include "request-handler.h"

namespace {

CalculatorProtocol::Models::CalculationRequest make_request(std::string request_id,
                                                            CalculatorProtocol::Codes::OperationCode operation,
                                                            std::vector<std::int64_t> operands) {
  CalculatorProtocol::Models::CalculationRequest request;
  request.set_request_id(std::move(request_id));
  request.set_operation(operation);
  request.set_operands(std::move(operands));
  return request;
}

}  // namespace

TEST(RequestHandlerTest, CalculatesAdditionThroughProtocolJson) {
  auto storage = std::make_shared<Calculator::Server::InMemoryOperationStorage>();
  Calculator::Server::RequestHandler handler(storage);

  const auto request = make_request("req-add", CalculatorProtocol::Codes::OperationCode::ADDITION, {2, 3});
  const auto response_json = handler.handle(CalculatorProtocol::Formats::JsonCodec::serialize_request(request));
  const auto response = CalculatorProtocol::Formats::JsonCodec::parse_response(response_json);

  EXPECT_EQ(response.request_id(), "req-add");
  EXPECT_EQ(response.status_code(), CalculatorProtocol::Codes::StatusCode::OK);
  ASSERT_TRUE(response.result_info().has_value());
  EXPECT_EQ(response.result_info()->value(), 5);
  ASSERT_TRUE(response.result_source().has_value());
  EXPECT_EQ(*response.result_source(), CalculatorProtocol::Types::ResultSource::CALCULATED);
}

TEST(RequestHandlerTest, ReturnsProtocolErrorForDivisionByZero) {
  auto storage = std::make_shared<Calculator::Server::InMemoryOperationStorage>();
  Calculator::Server::RequestHandler handler(storage);

  const auto request = make_request("req-div0", CalculatorProtocol::Codes::OperationCode::DIVISION, {10, 0});
  const auto response_json = handler.handle(CalculatorProtocol::Formats::JsonCodec::serialize_request(request));
  const auto response = CalculatorProtocol::Formats::JsonCodec::parse_response(response_json);

  EXPECT_EQ(response.status_code(), CalculatorProtocol::Codes::StatusCode::DIVISION_BY_ZERO);
  EXPECT_FALSE(response.result_info().has_value());
  ASSERT_TRUE(response.error_info().has_value());
}

TEST(RequestHandlerTest, ReusesCachedResult) {
  auto storage = std::make_shared<Calculator::Server::InMemoryOperationStorage>();
  Calculator::Server::RequestHandler handler(storage);

  const auto first_request = make_request("req-cache-1", CalculatorProtocol::Codes::OperationCode::MULTIPLICATION, {4, 5});
  auto _ = handler.handle(CalculatorProtocol::Formats::JsonCodec::serialize_request(first_request));

  const auto second_request = make_request("req-cache-2", CalculatorProtocol::Codes::OperationCode::MULTIPLICATION, {5, 4});
  const auto response_json = handler.handle(CalculatorProtocol::Formats::JsonCodec::serialize_request(second_request));
  const auto response = CalculatorProtocol::Formats::JsonCodec::parse_response(response_json);

  EXPECT_EQ(response.status_code(), CalculatorProtocol::Codes::StatusCode::OK);
  ASSERT_TRUE(response.result_info().has_value());
  EXPECT_EQ(response.result_info()->value(), 20);
  ASSERT_TRUE(response.result_source().has_value());
  EXPECT_EQ(*response.result_source(), CalculatorProtocol::Types::ResultSource::CACHE);
}
