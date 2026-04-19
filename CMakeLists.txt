cmake_minimum_required(VERSION 3.28)

project(ConsoleCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

# Mathing
FetchContent_Declare(
  ConsoleCalculatorMathLib
  GIT_REPOSITORY https://github.com/otus-25-masik-52/ConsoleCalculatorMathLib.git
  GIT_TAG master
)
# JSONing
FetchContent_Declare(
  NlohmannJsonLib
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
# Testing
FetchContent_Declare(
  Googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Logging
FetchContent_Declare(
  Spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.17.0
)

# For MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ConsoleCalculatorMathLib NlohmannJsonLib Googletest Spdlog)

add_library(calculator_core
  src/abacus.cpp
  src/checker.cpp
  src/logger.cpp
  src/parser.cpp
  src/printer.cpp
  src/runner.cpp
)

target_include_directories(calculator_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(calculator_core PRIVATE calculator_math spdlog::spdlog_header_only nlohmann_json::nlohmann_json)

add_executable(calculator main.cpp)
target_link_libraries(calculator PRIVATE calculator_core)

if(BUILD_TESTING)
  enable_testing()

  add_executable(calculator_tests
    tests/checker_tests.cpp
    tests/parser_tests.cpp
    tests/printer_tests.cpp
  )

  target_link_libraries(calculator_tests PRIVATE calculator_core GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(calculator_tests)
endif()

include(GNUInstallDirs)
install(TARGETS calculator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})