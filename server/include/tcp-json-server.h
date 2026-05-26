#pragma once

#include "request-handler.h"
#include "server-config.h"

#include <boost/asio.hpp>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace Calculator::Server {

class TcpJsonServer final {
 public:
  TcpJsonServer(ServerConfig config, std::shared_ptr<RequestHandler> request_handler);
  ~TcpJsonServer();

  TcpJsonServer(const TcpJsonServer&) = delete;
  TcpJsonServer& operator=(const TcpJsonServer&) = delete;
  TcpJsonServer(TcpJsonServer&&) = delete;
  TcpJsonServer& operator=(TcpJsonServer&&) = delete;

  void start();
  void stop() noexcept;
  [[nodiscard]] std::uint16_t port() const noexcept { return bound_port_; }

 private:
  using Tcp = boost::asio::ip::tcp;

  class Session;

  void do_accept();

  ServerConfig config_;
  boost::asio::io_context io_context_;
  Tcp::acceptor acceptor_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
  std::shared_ptr<RequestHandler> request_handler_;
  std::vector<std::thread> workers_;
  std::atomic_bool running_{false};
  std::uint16_t bound_port_{0};
};

}  // namespace Calculator::Server
