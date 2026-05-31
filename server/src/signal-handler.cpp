#include "signal-handler.h"

#include <pthread.h>
#include <stdexcept>
#include <utility>

namespace Calculator::Server {

SignalHandler::SignalHandler(std::function<void(int)> callback) : callback_(std::move(callback)) {}

SignalHandler::~SignalHandler() {
  stop();
}

void SignalHandler::start() {
  if (running_.exchange(true)) {
    return;
  }

  sigset_t signals;
  sigemptyset(&signals);
  sigaddset(&signals, SIGINT);
  sigaddset(&signals, SIGTERM);

  if (pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0) {
    running_ = false;
    throw std::runtime_error{"failed to block service signals"};
  }

  thread_ = std::thread(&SignalHandler::thread_main, this);
}

void SignalHandler::stop() noexcept {
  if (const bool was_running = running_.exchange(false); was_running && thread_.joinable()) {
    pthread_kill(thread_.native_handle(), SIGTERM);
  }
  if (thread_.joinable()) {
    thread_.join();
  }
}

void SignalHandler::thread_main() {
  sigset_t signals;
  sigemptyset(&signals);
  sigaddset(&signals, SIGINT);
  sigaddset(&signals, SIGTERM);

  while (running_) {
    int signal = 0;
    if (const int result = sigwait(&signals, &signal); result == 0 && running_) {
      running_ = false;
      callback_(signal);
      return;
    }
  }
}

} // namespace Calculator::Server
