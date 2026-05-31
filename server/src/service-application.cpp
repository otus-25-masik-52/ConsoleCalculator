#include "service-application.h"

#include <condition_variable>
#include <memory>
#include <mutex>

#include "logger.h"
#include "request-handler.h"
#include "signal-handler.h"
#include "storage-worker.h"
#include "tcp-json-server.h"

namespace Calculator::Server {

int ServiceApplication::run(const ServerConfig& config) {
  Logger::init(config.log_file());
  Logger::info("ConsoleCalculatorServer service starting.");

  std::mutex mutex;
  std::condition_variable condition;
  bool stopping = false;

  SignalHandler signal_handler([&](int signal) {
    Logger::info("Shutdown signal received: " + std::to_string(signal));
    {
      std::scoped_lock const lock{mutex};
      stopping = true;
    }
    condition.notify_one();
  });

  // Block service shutdown signals in the main thread before creating worker threads.
  // New worker threads inherit this signal mask; the dedicated signal thread uses sigwait().
  signal_handler.start();

  auto storage = std::make_shared<StorageWorker>(config.database_connection_string());
  storage->start();

  auto handler = std::make_shared<RequestHandler>(storage);
  TcpJsonServer server(config, handler);
  server.start();

  {
    std::unique_lock lock{mutex};
    condition.wait(lock, [&] { return stopping; });
  }

  server.stop();
  storage->stop();
  signal_handler.stop();

  Logger::info("ConsoleCalculatorServer service stopped.");
  Logger::shutdown();
  return 0;
}

}  // namespace Calculator::Server
