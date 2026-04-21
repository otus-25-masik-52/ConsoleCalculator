#include "runner.h"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
  try {
    return Calculator::Runner::run(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown fatal error\n";
  }
  return EXIT_FAILURE;
}
