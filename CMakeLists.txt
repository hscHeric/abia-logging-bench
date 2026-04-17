cmake_minimum_required(VERSION 3.16)
project(logging_bench CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" LOGGING_BENCH_SYSTEM_PROCESSOR)
set(LOGGING_BENCH_NANOLOG_SUPPORTED_ARCH OFF)
if (LOGGING_BENCH_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|x86|i[3-6]86)$")
  set(LOGGING_BENCH_NANOLOG_SUPPORTED_ARCH ON)
endif()

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

if (LOGGING_BENCH_NANOLOG_SUPPORTED_ARCH)
  add_library(nanolog_lib STATIC
    external/NanoLog/runtime/Cycles.cc
    external/NanoLog/runtime/Util.cc
    external/NanoLog/runtime/Log.cc
    external/NanoLog/runtime/NanoLog.cc
    external/NanoLog/runtime/RuntimeLogger.cc
    external/NanoLog/runtime/TimeTrace.cc
    benchmarks/nanolog_generated_code_stub.cc
  )
  target_include_directories(nanolog_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/NanoLog/runtime
  )
  target_link_libraries(nanolog_lib PUBLIC pthread rt)
  add_library(NanoLog::nanolog_lib ALIAS nanolog_lib)
else()
  message(STATUS
    "Skipping NanoLog benchmark on '${CMAKE_SYSTEM_PROCESSOR}': upstream NanoLog runtime requires x86 rdtsc/SSE intrinsics.")
endif()

add_executable(bench_spdlog benchmarks/spdlog_main.cpp)
target_link_libraries(bench_spdlog PRIVATE spdlog::spdlog)

add_executable(bench_quill benchmarks/quill_main.cpp)
target_link_libraries(bench_quill PRIVATE quill::quill)

add_executable(bench_log17 benchmarks/log17_main.cpp)
target_link_libraries(bench_log17 PRIVATE log17)
target_sources(bench_log17 PRIVATE external/log17/log.hpp)

add_executable(bench_lwlog benchmarks/lwlog_main.cpp)
target_link_libraries(bench_lwlog PRIVATE lwlog::lwlog_lib)

if (LOGGING_BENCH_NANOLOG_SUPPORTED_ARCH)
  add_executable(bench_nanolog benchmarks/nanolog_main.cpp)
  target_link_libraries(bench_nanolog PRIVATE NanoLog::nanolog_lib)
  target_compile_options(bench_nanolog PRIVATE -Werror=format)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-O2 -Wall -Wextra)
endif()
