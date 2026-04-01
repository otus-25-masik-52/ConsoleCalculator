cmake_minimum_required(VERSION 3.28)

project(ConsoleCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  ConsoleCalculatorMathLib
  GIT_REPOSITORY https://github.com/otus-25-masik-52/ConsoleCalculatorMathLib.git
  GIT_TAG master
)
FetchContent_MakeAvailable(ConsoleCalculatorMathLib)

add_executable(calculator
  main.cpp
  src/calculator.cpp
  src/checker.cpp
  src/init.cpp
  src/parser.cpp
  src/printer.cpp
  src/runner.cpp
)

target_include_directories(calculator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(calculator PRIVATE calculator_math)

include(GNUInstallDirs)
install(TARGETS calculator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})