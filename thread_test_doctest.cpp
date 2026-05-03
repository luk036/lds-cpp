#include <doctest/doctest.h>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "lds/lds.hpp"

TEST_CASE("VdCorput thread safety") {
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
    for (unsigned long i = 1; i < all_values.size(); ++i) {
        CHECK_NE(all_values[i], all_values[i - 1]);
    }

    // Check that we got the expected number of values
    CHECK_EQ(all_values.size(), num_threads * values_per_thread);
}

TEST_CASE("Halton thread safety") {
    const int num_threads = 8;
    const int values_per_thread = 100;
    lds::Halton hgen(2, 3);
    std::vector<std::thread> threads;
    std::atomic<int> pop_count{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&hgen, &pop_count, values_per_thread]() {
            for (int j = 0; j < values_per_thread; ++j) {
                auto val = hgen.pop();
                (void)val;  // suppress unused variable warning
                pop_count++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    CHECK_EQ(pop_count.load(), num_threads * values_per_thread);
}