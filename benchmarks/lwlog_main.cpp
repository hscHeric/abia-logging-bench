#include <lwlog.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace std::chrono;

namespace {
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
  const size_t messages = 1'000'000;
  std::vector<uint64_t> latencies;
  latencies.reserve(messages);

  {
    auto logger = std::make_shared<lwlog::async_file_logger>(
        "async_logger", "logs/lwlog_bench.log");
    logger->set_pattern("%v\n");

    for (size_t i = 0; i < 100'000; ++i) {
      logger->info("Warmup message number: {}", i);
    }

    auto start_total = high_resolution_clock::now();
    for (size_t i = 0; i < messages; ++i) {
      auto t1 = high_resolution_clock::now();
      logger->info("Benchmark message number: {}", i);
      auto t2 = high_resolution_clock::now();

      latencies.push_back(duration_cast<nanoseconds>(t2 - t1).count());
    }
    auto end_total = high_resolution_clock::now();

    print_stats("lwlog Async", messages,
                duration_cast<nanoseconds>(end_total - start_total), latencies);
  }

  return 0;
}
