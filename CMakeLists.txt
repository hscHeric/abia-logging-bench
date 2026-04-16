cmake_minimum_required(VERSION 3.16)
project(logging_bench CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(spdlog REQUIRED)
find_package(quill REQUIRED)

add_executable(bench_spdlog benchmarks/spdlog_main.cpp)
target_link_libraries(bench_spdlog PRIVATE spdlog::spdlog)

add_executable(bench_quill benchmarks/quill_main.cpp)
target_link_libraries(bench_quill PRIVATE quill::quill)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-O2 -Wall -Wextra)
endif()
