cmake_minimum_required(VERSION 3.16)
project(logging_bench CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(spdlog REQUIRED)
find_package(quill REQUIRED)

add_library(log17 INTERFACE)
target_include_directories(log17 INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/external
)

add_library(lwlog_lib STATIC
  external/lwlog/lwlog/include/details/topic_registry.cpp
  external/lwlog/lwlog/include/details/pattern/attribute.cpp
)
target_include_directories(lwlog_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/external/lwlog/lwlog/include
)
add_library(lwlog::lwlog_lib ALIAS lwlog_lib)

add_executable(bench_spdlog benchmarks/spdlog_main.cpp)
target_link_libraries(bench_spdlog PRIVATE spdlog::spdlog)

add_executable(bench_quill benchmarks/quill_main.cpp)
target_link_libraries(bench_quill PRIVATE quill::quill)

add_executable(bench_log17 benchmarks/log17_main.cpp)
target_link_libraries(bench_log17 PRIVATE log17)
target_sources(bench_log17 PRIVATE external/log17/log.hpp)

add_executable(bench_lwlog benchmarks/lwlog_main.cpp)
target_link_libraries(bench_lwlog PRIVATE lwlog::lwlog_lib)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-O2 -Wall -Wextra)
endif()
