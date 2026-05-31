cmake_minimum_required(VERSION 3.28)

project(ConsoleCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CALCULATOR_SERVER_ENABLE_TSAN "Build with ThreadSanitizer" OFF)

include(FetchContent)
include(CTest)
include(GNUInstallDirs)

# =========================
# Dependencies
# =========================

FetchContent_Declare(
  ConsoleCalculatorMathLib
  GIT_REPOSITORY https://github.com/otus-25-masik-52/ConsoleCalculatorMathLib.git
  GIT_TAG master
)

FetchContent_Declare(
  ConsoleCalculatorProtocol
  GIT_REPOSITORY https://github.com/otus-25-masik-52/ConsoleCalculatorProtocol.git
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
  ConsoleCalculatorProtocol
  NlohmannJsonLib
  Googletest
  Spdlog
)

find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

function(calculator_enable_tsan target_name)
  if (CALCULATOR_SERVER_ENABLE_TSAN AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
    target_compile_options(${target_name} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(${target_name} PRIVATE -fsanitize=thread)
  endif ()
endfunction()

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

calculator_enable_tsan(calculator_support)

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

calculator_enable_tsan(calculator_core)

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

calculator_enable_tsan(calculator_storage)

# =========================
# Linux service / TCP JSON server
# =========================

add_library(calculator_server_lib
  server/src/in-memory-operation-storage.cpp
  server/src/request-handler.cpp
  server/src/server-config.cpp
  server/src/service-application.cpp
  server/src/signal-handler.cpp
  server/src/storage-worker.cpp
  server/src/tcp-json-server.cpp
)

target_include_directories(calculator_server_lib
  PUBLIC
  server/include
)

target_link_libraries(calculator_server_lib
  PUBLIC
  calculator_core
  calculator_storage
  calculator_support
  calculator_protocol
  Boost::system
  Threads::Threads
)

calculator_enable_tsan(calculator_server_lib)

add_executable(calculator_server
  server/main.cpp
)

target_link_libraries(calculator_server
  PRIVATE
  calculator_server_lib
)

calculator_enable_tsan(calculator_server)

# =========================
# tests
# =========================

if (BUILD_TESTING)
  add_executable(calculator_tests
    tests/cache_repository_tests.cpp
    tests/checker_tests.cpp
    tests/operation_key_tests.cpp
    tests/request_handler_tests.cpp
    tests/tcp_json_server_tests.cpp
  )

  target_link_libraries(calculator_tests
    PRIVATE
    calculator_core
    calculator_storage
    calculator_support
    calculator_server_lib
    calculator_protocol
    GTest::gtest_main
  )

  calculator_enable_tsan(calculator_tests)

  include(GoogleTest)
  gtest_discover_tests(calculator_tests)
endif ()

# =========================
# install
# =========================

install(
  TARGETS calculator_server
  RUNTIME DESTINATION usr/bin
)

install(
  FILES config/server.conf
  DESTINATION etc/console-calculator
)

install(
  FILES systemd/console-calculator-server.service
  DESTINATION lib/systemd/system
)