#pragma once

#include "server-config.h"

namespace Calculator::Server {

class ServiceApplication final {
 public:
  [[nodiscard]] static int run(const ServerConfig& config);
};

}  // namespace Calculator::Server
