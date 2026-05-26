#include "server-config.h"
#include "service-application.h"

#include <exception>
#include <iostream>
#include <string>

namespace {

std::string config_path_from_args(int argc, char** argv) {
  std::string config_path = "/etc/console-calculator/server.conf";
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if ((argument == "--config" || argument == "-c") && index + 1 < argc) {
      config_path = argv[++index];
    }
  }
  return config_path;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const auto config_path = config_path_from_args(argc, argv);
    const auto config = Calculator::Server::ServerConfig::from_environment_and_file(config_path);
    return Calculator::Server::ServiceApplication::run(config);
  } catch (const std::exception& error) {
    std::cerr << "Fatal server error: " << error.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown fatal server error\n";
  }

  return 1;
}