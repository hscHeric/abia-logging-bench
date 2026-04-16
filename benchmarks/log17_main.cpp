#include <log17/log.hpp>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std::chrono;

namespace {
constexpr char kLoggerName[] = "log17";

struct FileSink {
  inline static FILE *file = nullptr;

  static void initialize(const char *filename) {
    file = fopen(filename, "w+");
    if (!file) {
      throw std::system_error(errno, std::system_category());
    }
  }

  static void finalize() {
    if (!file) {
      return;
    }

    if (fclose(file)) {
      throw std::system_error(errno, std::system_category());
    }
    file = nullptr;
  }

  static void write(const char *szstr, int size) {
    if (!fwrite(szstr, size, 1, file)) {
      throw std::system_error(errno, std::system_category());
    }
    if (fputc('\n', file) == EOF) {
      throw std::system_error(errno, std::system_category());
    }
    if (fflush(file)) {
      throw std::system_error(errno, std::system_category());
    }
  }
};

using BenchLog = Log<LogLevel::INFO, kLoggerName, FileSink>;

void print_stats(const std::string &name, size_t count, nanoseconds total_time,
                 std::vector<uint64_t> &latencies) {
  std::sort(latencies.begin(), latencies.end());
  double avg_throughput = static_cast<double>(count) / (total_time.count() / 1e9);

  std::cout << "\n=== " << name << " ===\n"
            << "Throughput: " << std::fixed << std::setprecision(2)
            << avg_throughput << " msg/sec\n"
            << "Latencies (Hot Path):\n"
            << "  p50:   " << latencies[count * 0.50] << " ns\n"
            << "  p90:   " << latencies[count * 0.90] << " ns\n"
            << "  p99:   " << latencies[count * 0.99] << " ns\n"
            << "  p99.9: " << latencies[count * 0.999] << " ns\n";
}
} // namespace

int main() {
  FileSink::initialize("log17_bench.log");

  const size_t messages = 1'000'000;
  std::vector<uint64_t> latencies;
  latencies.reserve(messages);

  for (size_t i = 0; i < 100'000; ++i) {
    BenchLog::info("Warmup message number: %zu", i);
  }

  auto start_total = high_resolution_clock::now();
  for (size_t i = 0; i < messages; ++i) {
    auto t1 = high_resolution_clock::now();
    BenchLog::info("Benchmark message number: %zu", i);
    auto t2 = high_resolution_clock::now();

    latencies.push_back(duration_cast<nanoseconds>(t2 - t1).count());
  }
  auto end_total = high_resolution_clock::now();

  print_stats("log17", messages,
              duration_cast<nanoseconds>(end_total - start_total), latencies);

  FileSink::finalize();
  return 0;
}
