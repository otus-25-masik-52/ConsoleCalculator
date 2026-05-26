#pragma once

#include <atomic>
#include <csignal>
#include <functional>
#include <thread>

namespace Calculator::Server {

class SignalHandler final {
 public:
  explicit SignalHandler(std::function<void(int)> callback);
  ~SignalHandler();

  SignalHandler(const SignalHandler&) = delete;
  SignalHandler& operator=(const SignalHandler&) = delete;
  SignalHandler(SignalHandler&&) = delete;
  SignalHandler& operator=(SignalHandler&&) = delete;

  void start();
  void stop() noexcept;

 private:
  void thread_main();

  std::function<void(int)> callback_;
  std::atomic_bool running_{false};
  std::thread thread_;
};

}  // namespace Calculator::Server
