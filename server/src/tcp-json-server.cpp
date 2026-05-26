#include "tcp-json-server.h"

#include <boost/asio/post.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "logger.h"

namespace Calculator::Server {

class TcpJsonServer::Session final : public std::enable_shared_from_this<TcpJsonServer::Session> {
 public:
  Session(Tcp::socket socket, std::shared_ptr<RequestHandler> request_handler, std::size_t max_message_size)
      : socket_(std::move(socket)), request_handler_(std::move(request_handler)), max_message_size_(max_message_size) {}

  void start() {
    do_read();
  }

 private:
  void do_read() {
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buffer_, '\n',
                                  [self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                    self->on_read(error, bytes_transferred);
                                  });
  }

  void on_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
      if (error != boost::asio::error::eof && error != boost::asio::error::operation_aborted) {
        Logger::warn(std::string{"TCP read failed: "} + error.message());
      }
      return;
    }

    if (bytes_transferred > max_message_size_) {
      Logger::warn("TCP message is too large; closing session.");
      close();
      return;
    }

    std::istream input(&buffer_);
    std::string request;
    std::getline(input, request);
    if (!request.empty() && request.back() == '\r') {
      request.pop_back();
    }

    std::string response = request_handler_->handle(request);
    response.push_back('\n');

    do_write(std::move(response));
  }

  void do_write(std::string response) {
    auto self = shared_from_this();
    auto output = std::make_shared<std::string>(std::move(response));
    boost::asio::async_write(socket_, boost::asio::buffer(*output),
                             [self, output](const boost::system::error_code& error, std::size_t) {
                               if (error) {
                                 Logger::warn(std::string{"TCP write failed: "} + error.message());
                                 return;
                               }
                               self->do_read();
                             });
  }

  void close() noexcept {
    boost::system::error_code ignored;
    auto _ = socket_.shutdown(Tcp::socket::shutdown_both, ignored);
    auto __ = socket_.close(ignored);
  }

  Tcp::socket socket_;
  boost::asio::streambuf buffer_;
  std::shared_ptr<RequestHandler> request_handler_;
  std::size_t max_message_size_;
};

TcpJsonServer::TcpJsonServer(ServerConfig config, std::shared_ptr<RequestHandler> request_handler)
    : config_(std::move(config)), io_context_(std::max(1, config_.worker_threads())), acceptor_(io_context_),
      work_guard_(boost::asio::make_work_guard(io_context_)), request_handler_(std::move(request_handler)) {
  if (!request_handler_) {
    throw std::invalid_argument{"request_handler must not be null"};
  }
}

TcpJsonServer::~TcpJsonServer() {
  stop();
}

void TcpJsonServer::start() {
  if (running_.exchange(true)) {
    return;
  }

  const auto address = boost::asio::ip::make_address(config_.host());
  Tcp::endpoint const endpoint(address, config_.port());

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(Tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen(boost::asio::socket_base::max_listen_connections);
  bound_port_ = acceptor_.local_endpoint().port();

  do_accept();

  const auto thread_count = std::max(1, config_.worker_threads());
  workers_.reserve(static_cast<std::size_t>(thread_count));
  for (int index = 0; index < thread_count; ++index) {
    workers_.emplace_back([this] { io_context_.run(); });
  }

  Logger::info("TCP JSON server started on " + config_.host() + ":" + std::to_string(bound_port_));
}

void TcpJsonServer::stop() noexcept {
  if (!running_.exchange(false)) {
    return;
  }

  boost::asio::post(io_context_, [this] {
    boost::system::error_code ignored;
    auto _ = acceptor_.close(ignored);
  });

  work_guard_.reset();
  io_context_.stop();

  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
  workers_.clear();

  Logger::info("TCP JSON server stopped.");
}

void TcpJsonServer::do_accept() {
  acceptor_.async_accept([this](const boost::system::error_code& error, Tcp::socket socket) {
    if (!running_) {
      return;
    }

    if (!error) {
      std::make_shared<Session>(std::move(socket), request_handler_, config_.max_message_size())->start();
    } else if (error != boost::asio::error::operation_aborted) {
      Logger::warn(std::string{"Accept failed: "} + error.message());
    }

    do_accept();
  });
}

} // namespace Calculator::Server
