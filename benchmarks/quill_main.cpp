#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/FileSink.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std::chrono;

void print_stats(const std::string &name, size_t count, nanoseconds total_time,
                 std::vector<uint64_t> &latencies) {
  std::sort(latencies.begin(), latencies.end());
  double avg_throughput = (double)count / (total_time.count() / 1e9);

  std::cout << "\n=== " << name << " ===\n"
            << "Throughput: " << std::fixed << std::setprecision(2)
            << avg_throughput << " msg/sec\n"
            << "Latencies (Hot Path):\n"
            << "  p50:   " << latencies[count * 0.50] << " ns\n"
            << "  p90:   " << latencies[count * 0.90] << " ns\n"
            << "  p99:   " << latencies[count * 0.99] << " ns\n"
            << "  p99.9: " << latencies[count * 0.999] << " ns\n";
}

int main() {
  quill::Backend::start();
  auto sink =
      quill::Frontend::create_or_get_sink<quill::FileSink>("quill_bench.log");
  auto logger = quill::Frontend::create_or_get_logger("bench", std::move(sink));

  const size_t messages = 1'000'000;
  std::vector<uint64_t> latencies;
  latencies.reserve(messages);

  for (size_t i = 0; i < 100'000; ++i) {
    LOG_INFO(logger, "Warmup message number: {}", i);
  }

  auto start_total = high_resolution_clock::now();
  for (size_t i = 0; i < messages; ++i) {
    auto t1 = high_resolution_clock::now();
    LOG_INFO(logger, "Benchmark message number: {}", i);
    auto t2 = high_resolution_clock::now();

    latencies.push_back(duration_cast<nanoseconds>(t2 - t1).count());
  }
  auto end_total = high_resolution_clock::now();

  print_stats("Quill Async", messages,
              duration_cast<nanoseconds>(end_total - start_total), latencies);

  return 0;
}
