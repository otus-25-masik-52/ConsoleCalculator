cmake_minimum_required(VERSION 3.28)

project(ConsoleCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

# =========================
# Dependencies
# =========================

FetchContent_Declare(
  ConsoleCalculatorMathLib
  GIT_REPOSITORY https://github.com/otus-25-masik-52/ConsoleCalculatorMathLib.git
  GIT_TAG master
)

FetchContent_Declare(
  NlohmannJsonLib
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)

FetchContent_Declare(
  Googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_Declare(
  Spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.17.0
)

if (MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()

FetchContent_MakeAvailable(
  ConsoleCalculatorMathLib
  NlohmannJsonLib
  Googletest
  Spdlog
)

find_package(PostgreSQL REQUIRED)

# =========================
# support
# =========================

add_library(calculator_support
  support/src/logger.cpp
)

target_include_directories(calculator_support
  PUBLIC
  support/include
)

target_link_libraries(calculator_support
  PUBLIC
  spdlog::spdlog_header_only
)

# =========================
# core
# =========================

add_library(calculator_core
  core/src/abacus.cpp
  core/src/checker.cpp
)

target_include_directories(calculator_core
  PUBLIC
  core/include
)

target_link_libraries(calculator_core
  PUBLIC
  calculator_support
  PRIVATE
  calculator_math
)

# =========================
# io
# =========================

add_library(calculator_io
  io/src/parser.cpp
  io/src/printer.cpp
)

target_include_directories(calculator_io
  PUBLIC
  io/include
)

target_link_libraries(calculator_io
  PUBLIC
  calculator_core
  calculator_support
  nlohmann_json::nlohmann_json
)

# =========================
# storage
# =========================

add_library(calculator_storage
  storage/src/cache-repository.cpp
  storage/src/operation-key.cpp
  storage/src/storage-service.cpp

  storage/postgres/src/postgres-connection.cpp
  storage/postgres/src/postgres-result.cpp
  storage/postgres/src/postgres-repository.cpp
)

target_include_directories(calculator_storage
  PUBLIC
  storage/include
  storage/postgres/include
)

target_link_libraries(calculator_storage
  PUBLIC
  calculator_support
  PRIVATE
  PostgreSQL::PostgreSQL
)

# =========================
# app
# =========================

add_library(calculator_app
  app/src/runner.cpp
)

target_include_directories(calculator_app
  PUBLIC
  app/include
  storage/postgres/include
)

target_link_libraries(calculator_app
  PUBLIC
  calculator_core
  calculator_io
  calculator_storage
  calculator_support
)

# =========================
# executable
# =========================

add_executable(calculator
  app/main.cpp
)

target_link_libraries(calculator
  PRIVATE
  calculator_app
)

# =========================
# tests
# =========================

if (BUILD_TESTING)
  add_executable(calculator_tests
    tests/cache_repository_tests.cpp
    tests/checker_tests.cpp
    tests/operation_key_tests.cpp
    tests/parser_tests.cpp
    tests/printer_tests.cpp
  )

  target_link_libraries(calculator_tests
    PRIVATE
    calculator_core
    calculator_io
    calculator_storage
    calculator_support
    GTest::gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(calculator_tests)
endif ()

# =========================
# install
# =========================

include(GNUInstallDirs)

install(
  TARGETS calculator
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)