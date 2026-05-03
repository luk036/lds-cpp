#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "lds/lds.hpp"

int main() {
    std::cout << "Testing thread safety of VdCorput...\n";

    const int num_threads = 8;
    const int values_per_thread = 100;
    lds::VdCorput vgen(2);
    std::vector<std::thread> threads;
    std::vector<std::vector<double>> results(num_threads);
    std::mutex mtx;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&vgen, &results, &mtx, i, values_per_thread]() {
            std::vector<double> local_results;
            for (int j = 0; j < values_per_thread; ++j) {
                local_results.emplace_back(vgen.pop());
            }
            std::lock_guard<std::mutex> lock(mtx);
            results[i] = std::move(local_results);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check that all values are unique (no duplicates)
    std::vector<double> all_values;
    for (const auto& thread_results : results) {
        all_values.insert(all_values.end(), thread_results.begin(), thread_results.end());
    }

    std::sort(all_values.begin(), all_values.end());
    bool has_duplicates = false;
    for (unsigned long i = 1; i < all_values.size(); ++i) {
        if (all_values[i] == all_values[i - 1]) {
            has_duplicates = true;
            break;
        }
    }

    if (has_duplicates) {
        std::cout << "FAIL: Found duplicate values!\n";
        return 1;
    }

    // Check that we got the expected number of values
    if (all_values.size() != num_threads * values_per_thread) {
        std::cout << "FAIL: Expected " << num_threads * values_per_thread << " values, got "
                  << all_values.size() << "\n";
        return 1;
    }

    std::cout << "PASS: VdCorput thread safety test passed!\n";

    // Test other generators
    std::cout << "Testing thread safety of Halton...\n";
    lds::Halton hgen(2, 3);
    std::vector<std::thread> hthreads;
    std::atomic<int> hpop_count{0};

    for (int i = 0; i < num_threads; ++i) {
        hthreads.emplace_back([&hgen, &hpop_count, values_per_thread]() {
            for (int j = 0; j < values_per_thread; ++j) {
                auto val = hgen.pop();
                (void)val;  // suppress unused variable warning
                hpop_count++;
            }
        });
    }

    for (auto& t : hthreads) {
        t.join();
    }

    if (hpop_count.load() != num_threads * values_per_thread) {
        std::cout << "FAIL: Halton pop count mismatch!\n";
        return 1;
    }

    std::cout << "PASS: Halton thread safety test passed!\n";

    std::cout << "All thread safety tests passed!\n";
    return 0;
}