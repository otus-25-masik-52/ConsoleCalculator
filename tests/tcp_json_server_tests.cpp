#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "calculator-protocol/codes/operation-code.h"
#include "calculator-protocol/codes/status-code.h"
#include "calculator-protocol/formats/json-codec.h"
#include "calculator-protocol/models/calculation-request.h"
#include "in-memory-operation-storage.h"
#include "request-handler.h"
#include "server-config.h"
#include "tcp-json-server.h"

namespace {

std::string exchange(std::uint16_t port, const std::string& request_json) {
  using Tcp = boost::asio::ip::tcp;

  boost::asio::io_context io_context;
  Tcp::socket socket(io_context);
  Tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), port);

  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  boost::system::error_code last_error;

  while (std::chrono::steady_clock::now() < deadline) {
    socket.close();
    socket = Tcp::socket(io_context);
    socket.connect(endpoint, last_error);
    if (!last_error) {
      break;
    }
  }

  if (last_error) {
    throw boost::system::system_error(last_error);
  }

  std::string outbound = request_json + "\n";
  boost::asio::write(socket, boost::asio::buffer(outbound));

  boost::asio::streambuf buffer;
  boost::asio::read_until(socket, buffer, '\n');

  std::istream input(&buffer);
  std::string line;
  std::getline(input, line);
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
  return line;
}

CalculatorProtocol::Models::CalculationRequest make_request() {
  CalculatorProtocol::Models::CalculationRequest request;
  request.set_request_id("req-network");
  request.set_operation(CalculatorProtocol::Codes::OperationCode::ADDITION);
  request.set_operands({40, 2});
  return request;
}

}  // namespace

TEST(TcpJsonServerTest, TestClientReceivesCalculationResult) {
  auto storage = std::make_shared<Calculator::Server::InMemoryOperationStorage>();
  auto handler = std::make_shared<Calculator::Server::RequestHandler>(storage);

  Calculator::Server::ServerConfig config;
  config.set_host("127.0.0.1");
  config.set_port(0);
  config.set_worker_threads(2);

  Calculator::Server::TcpJsonServer server(config, handler);
  server.start();

  const auto request_json = CalculatorProtocol::Formats::JsonCodec::serialize_request(make_request());
  const auto response_json = exchange(server.port(), request_json);
  const auto response = CalculatorProtocol::Formats::JsonCodec::parse_response(response_json);

  server.stop();

  EXPECT_EQ(response.status_code(), CalculatorProtocol::Codes::StatusCode::OK);
  ASSERT_TRUE(response.result_info().has_value());
  EXPECT_EQ(response.result_info()->value(), 42);
}
