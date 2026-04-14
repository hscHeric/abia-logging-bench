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

add_subdirectory(external/lwlog)

add_executable(bench_lwlog benchmarks/lwlog_main.cpp)

target_include_directories(bench_lwlog PRIVATE
    ${CMAKE_SOURCE_DIR}/external/lwlog/lwlog/include
)

target_link_libraries(bench_lwlog PRIVATE lwlog::lwlog_lib)

add_executable(bench_log17
    benchmarks/log17_main.cpp
    external/log17/log.cpp
)

target_include_directories(bench_log17 PRIVATE
    ${CMAKE_SOURCE_DIR}/external/log17
)

add_executable(bench_nanolog benchmarks/nanolog_main.cpp)

target_include_directories(bench_nanolog PRIVATE
    ${CMAKE_SOURCE_DIR}/external/nanolog/runtime
)

target_link_libraries(bench_nanolog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nanolog/runtime/libNanoLog.a
    pthread
    rt
)

target_compile_options(bench_nanolog PRIVATE -Werror=format)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-O2 -Wall -Wextra)
endif()
