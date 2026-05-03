#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <algorithm>       // for std::sort
#include <lds/lds.hpp>  // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("vdc") { CHECK_EQ(lds::vdc<2>(11), doctest::Approx(0.8125)); }

TEST_CASE("VdCorput") {
    auto vgen = lds::VdCorput<2>();
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.25));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.75));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.125));
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
}

TEST_CASE("VdCorput::reseed with non-zero") {
    auto vgen = lds::VdCorput<2>();
    vgen.reseed(5);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.375));
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
}

TEST_CASE("Circle") {
    auto cgen = lds::Circle<2>();
    const auto arr = cgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-1.0));
    CHECK_EQ(arr[1], doctest::Approx(0.0));
    cgen.reseed(0);
    const auto arr2 = cgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-1.0));
    CHECK_EQ(arr2[1], doctest::Approx(0.0));
}

TEST_CASE("Circle::pop multiple") {
    auto cgen = lds::Circle<2>();
    cgen.reseed(0);
    auto res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-1.0));
    CHECK_EQ(res[1], doctest::Approx(0.0));
    res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(1.0));
}

TEST_CASE("Circle::reseed with non-zero") {
    auto cgen = lds::Circle<2>();
    cgen.reseed(2);
    auto res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(-1.0));
    cgen.reseed(0);
    res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-1.0));
    CHECK_EQ(res[1], doctest::Approx(0.0));
}

TEST_CASE("Disk") {
    auto dgen = lds::Disk<2, 3>();
    const auto arr = dgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5773502692));
    dgen.reseed(0);
    const auto arr2 = dgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5773502692));
}

TEST_CASE("Disk::pop multiple") {
    auto dgen = lds::Disk<2, 3>();
    dgen.reseed(0);
    auto res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5773502691896257));
    CHECK_EQ(res[1], doctest::Approx(0.0));
    res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(0.816496580927726));
}

TEST_CASE("Disk::reseed with non-zero") {
    auto dgen = lds::Disk<2, 3>();
    dgen.reseed(2);
    auto res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(-0.3333333333333333));
    dgen.reseed(0);
    res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5773502691896257));
    CHECK_EQ(res[1], doctest::Approx(0.0));
}

TEST_CASE("Halton") {
    auto hgen = lds::Halton<2, 3>();
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(0.5));
    hgen.reseed(0);
    const auto arr2 = hgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(0.5));
}

TEST_CASE("Halton::pop multiple") {
    auto hgen = lds::Halton<2, 3>();
    hgen.reseed(0);
    auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0 / 3.0));
    res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.25));
    CHECK_EQ(res[1], doctest::Approx(2.0 / 3.0));
}

TEST_CASE("Halton::reseed with non-zero") {
    auto hgen = lds::Halton<2, 3>();
    hgen.reseed(5);
    auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.375));
    CHECK_EQ(res[1], doctest::Approx(2.0 / 9.0));
    hgen.reseed(0);
    res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0 / 3.0));
}

TEST_CASE("Sphere") {
    auto sgen = lds::Sphere<2, 3>();
    const auto arr = sgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5));
    sgen.reseed(0);
    const auto arr2 = sgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5));
}

TEST_CASE("Sphere::pop multiple") {
    auto sgen = lds::Sphere<2, 3>();
    sgen.reseed(0);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5));
    CHECK_EQ(res[1], doctest::Approx(0.8660254037844387));
    CHECK_EQ(res[2], doctest::Approx(0.0));
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.4330127018922197));
    CHECK_EQ(res[1], doctest::Approx(-0.75));
    CHECK_EQ(res[2], doctest::Approx(-0.5));
}

TEST_CASE("Sphere::reseed with non-zero") {
    auto sgen = lds::Sphere<2, 3>();
    sgen.reseed(1);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.4330127018922197));
    CHECK_EQ(res[1], doctest::Approx(-0.75));
    CHECK_EQ(res[2], doctest::Approx(-0.5));
    sgen.reseed(0);
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5));
    CHECK_EQ(res[1], doctest::Approx(0.8660254037844387));
    CHECK_EQ(res[2], doctest::Approx(0.0));
}

TEST_CASE("Sphere3Hopf") {
    auto shfgen = lds::Sphere3Hopf<2, 3, 5>();
    const auto arr = shfgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.22360679774997898));
    shfgen.reseed(0);
    const auto arr2 = shfgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.22360679774997898));
}

TEST_CASE("Sphere3Hopf::pop multiple") {
    auto sgen = lds::Sphere3Hopf<2, 3, 5>();
    sgen.reseed(0);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.22360679774997898));
    CHECK_EQ(res[1], doctest::Approx(0.3872983346207417));
    CHECK_EQ(res[2], doctest::Approx(0.4472135954999573));
    CHECK_EQ(res[3], doctest::Approx(-0.7745966692414837));
}

TEST_CASE("Sphere3Hopf::reseed with non-zero") {
    auto sgen = lds::Sphere3Hopf<2, 3, 5>();
    sgen.reseed(1);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.3162277660168382));
    CHECK_EQ(res[1], doctest::Approx(-0.547722557505166));
    CHECK_EQ(res[2], doctest::Approx(0.6708203932499367));
    CHECK_EQ(res[3], doctest::Approx(-0.38729833462074204));
    sgen.reseed(0);
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.22360679774997898));
    CHECK_EQ(res[1], doctest::Approx(0.3872983346207417));
    CHECK_EQ(res[2], doctest::Approx(0.4472135954999573));
    CHECK_EQ(res[3], doctest::Approx(-0.7745966692414837));
}

TEST_CASE("dummy") { CHECK_EQ(lds::dummy(15), 53); }

// #include <atomic>
// #include <mutex>
// #include <thread>
// #include <vector>

// TEST_CASE("VdCorput thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::VdCorput<2> vgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<double>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&vgen, &results, &mtx, i]() {
//             std::vector<double> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(vgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that all values are unique (no duplicates)
//     std::vector<double> all_values;
//     for (const auto& thread_results : results) {
//         all_values.insert(all_values.end(), thread_results.begin(), thread_results.end());
//     }

//     std::sort(all_values.begin(), all_values.end());
//     for (unsigned long i = 1; i < all_values.size(); ++i) {
//         CHECK_NE(all_values[i], all_values[i - 1]);
//     }

//     // Check that we got the expected number of values
//     CHECK_EQ(all_values.size(), num_threads * values_per_thread);
// }

// TEST_CASE("Halton thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::Halton<2, 3> hgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<std::array<double, 2>>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&hgen, &results, &mtx, i]() {
//             std::vector<std::array<double, 2>> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(hgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that we got the expected number of values
//     unsigned long total_values = 0;
//     for (const auto& thread_results : results) {
//         total_values += thread_results.size();
//     }
//     CHECK_EQ(total_values, num_threads * values_per_thread);
// }

// TEST_CASE("Circle thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::Circle<2> cgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<std::array<double, 2>>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&cgen, &results, &mtx, i]() {
//             std::vector<std::array<double, 2>> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(cgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that we got the expected number of values
//     unsigned long total_values = 0;
//     for (const auto& thread_results : results) {
//         total_values += thread_results.size();
//     }
//     CHECK_EQ(total_values, num_threads * values_per_thread);

//     // Verify points are on unit circle
//     for (const auto& thread_results : results) {
//         for (const auto& point : thread_results) {
//             double radius_squared = point[0] * point[0] + point[1] * point[1];
//             CHECK_EQ(radius_squared, doctest::Approx(1.0));
//         }
//     }
// }

// TEST_CASE("Disk thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::Disk<2, 3> dgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<std::array<double, 2>>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&dgen, &results, &mtx, i]() {
//             std::vector<std::array<double, 2>> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(dgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that we got the expected number of values
//     unsigned long total_values = 0;
//     for (const auto& thread_results : results) {
//         total_values += thread_results.size();
//     }
//     CHECK_EQ(total_values, num_threads * values_per_thread);

//     // Verify points are inside unit disk
//     for (const auto& thread_results : results) {
//         for (const auto& point : thread_results) {
//             double radius_squared = point[0] * point[0] + point[1] * point[1];
//             CHECK_LE(radius_squared, 1.0);
//         }
//     }
// }

// TEST_CASE("Sphere thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::Sphere<2, 3> sgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<std::array<double, 3>>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&sgen, &results, &mtx, i]() {
//             std::vector<std::array<double, 3>> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(sgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that we got the expected number of values
//     unsigned long total_values = 0;
//     for (const auto& thread_results : results) {
//         total_values += thread_results.size();
//     }
//     CHECK_EQ(total_values, num_threads * values_per_thread);

//     // Verify points are on unit sphere
//     for (const auto& thread_results : results) {
//         for (const auto& point : thread_results) {
//             double radius_squared = point[0] * point[0] + point[1] * point[1] + point[2] * point[2];
//             CHECK_EQ(radius_squared, doctest::Approx(1.0));
//         }
//     }
// }

// TEST_CASE("Sphere3Hopf thread safety") {
//     const int num_threads = 8;
//     const int values_per_thread = 100;
//     lds::Sphere3Hopf<2, 3, 5> shfgen{};
//     std::vector<std::thread> threads;
//     std::vector<std::vector<std::array<double, 4>>> results(num_threads);
//     std::mutex mtx;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&shfgen, &results, &mtx, i]() {
//             std::vector<std::array<double, 4>> local_results;
//             for (int j = 0; j < values_per_thread; ++j) {
//                 local_results.emplace_back(shfgen.pop());
//             }
//             std::lock_guard<std::mutex> lock(mtx);
//             results[static_cast<unsigned long>(i)] = std::move(local_results);
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that we got the expected number of values
//     unsigned long total_values = 0;
//     for (const auto& thread_results : results) {
//         total_values += thread_results.size();
//     }
//     CHECK_EQ(total_values, num_threads * values_per_thread);

//     // Verify points are on unit 3-sphere
//     for (const auto& thread_results : results) {
//         for (const auto& point : thread_results) {
//             double radius_squared = point[0] * point[0] + point[1] * point[1] + point[2] * point[2]
//                                     + point[3] * point[3];
//             CHECK_EQ(radius_squared, doctest::Approx(1.0));
//         }
//     }
// }

// TEST_CASE("Concurrent reseed thread safety") {
//     const int num_threads = 8;
//     const int operations_per_thread = 50;
//     lds::VdCorput<2> vgen{};
//     std::vector<std::thread> threads;
//     std::atomic<int> pop_count{0};
//     std::atomic<int> reseed_count{0};
//     std::mutex mtx;
//     std::vector<double> results;

//     for (int i = 0; i < num_threads; ++i) {
//         threads.emplace_back([&vgen, &pop_count, &reseed_count, &mtx, &results, i]() {
//             for (int j = 0; j < operations_per_thread; ++j) {
//                 if (j % 10 == 0) {
//                     // Occasionally reseed
//                     vgen.reseed(static_cast<unsigned long>(i * 10 + j));
//                     reseed_count++;
//                 } else {
//                     // Mostly pop
//                     double val = vgen.pop();
//                     std::lock_guard<std::mutex> lock(mtx);
//                     results.emplace_back(val);
//                     pop_count++;
//                 }
//             }
//         });
//     }

//     for (auto& t : threads) {
//         t.join();
//     }

//     // Check that operations completed without crashes
//     CHECK_GT(pop_count.load(), 0);
//     CHECK_GT(reseed_count.load(), 0);
//     CHECK_EQ(results.size(), pop_count.load());
// }

TEST_CASE("VdCorput::peek") {
    auto vgen = lds::VdCorput<2>();
    auto peeked = vgen.peek();
    CHECK_EQ(peeked, doctest::Approx(0.5));
    auto popped = vgen.pop();
    CHECK_EQ(popped, doctest::Approx(0.5));
    CHECK_EQ(vgen.peek(), doctest::Approx(0.25));
}

TEST_CASE("VdCorput::batch") {
    auto vgen = lds::VdCorput<2>();
    auto batch = vgen.batch(5);
    CHECK_EQ(batch.size(), 5);
    CHECK_EQ(batch[0], doctest::Approx(0.5));
    CHECK_EQ(batch[1], doctest::Approx(0.25));
    CHECK_EQ(batch[2], doctest::Approx(0.75));
    CHECK_EQ(batch[3], doctest::Approx(0.125));
    CHECK_EQ(batch[4], doctest::Approx(0.625));
}

TEST_CASE("VdCorput::skip") {
    auto vgen = lds::VdCorput<2>();
    vgen.skip(3);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.125));
    vgen.reseed(0);
    vgen.skip(4);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.625));
}

TEST_CASE("VdCorput::iterator") {
    auto vgen = lds::VdCorput<2>();
    auto it = vgen.begin();
    CHECK_EQ(*it, doctest::Approx(0.5));
    ++it;
    CHECK_EQ(*it, doctest::Approx(0.25));
}

TEST_CASE("VdCorput::iterator with STL") {
    auto vgen = lds::VdCorput<2>();
    auto it = vgen.begin();
    std::vector<double> values;
    for (int i = 0; i < 10; ++i, ++it) {
        values.emplace_back(*it);
    }
    CHECK_EQ(values.size(), 10);
    CHECK_EQ(values[0], doctest::Approx(0.5));
    CHECK_EQ(values[1], doctest::Approx(0.25));
    CHECK_EQ(values[2], doctest::Approx(0.75));
}

TEST_CASE("Halton::peek") {
    auto hgen = lds::Halton<2, 3>();
    auto peeked = hgen.peek();
    CHECK_EQ(peeked[0], doctest::Approx(0.5));
    CHECK_EQ(peeked[1], doctest::Approx(1.0 / 3.0));
    auto popped = hgen.pop();
    CHECK_EQ(popped[0], doctest::Approx(0.5));
    CHECK_EQ(popped[1], doctest::Approx(1.0 / 3.0));
}

TEST_CASE("Halton::batch") {
    auto hgen = lds::Halton<2, 3>();
    auto batch = hgen.batch(3);
    CHECK_EQ(batch.size(), 3);
    CHECK_EQ(batch[0][0], doctest::Approx(0.5));
    CHECK_EQ(batch[0][1], doctest::Approx(1.0 / 3.0));
    CHECK_EQ(batch[1][0], doctest::Approx(0.25));
    CHECK_EQ(batch[1][1], doctest::Approx(2.0 / 3.0));
}

TEST_CASE("Halton::skip") {
    auto hgen = lds::Halton<2, 3>();
    hgen.skip(2);
    auto popped = hgen.pop();
    CHECK_EQ(popped[0], doctest::Approx(0.75));
    CHECK_EQ(popped[1], doctest::Approx(1.0 / 9.0));
}

TEST_CASE("Halton::iterator") {
    auto hgen = lds::Halton<2, 3>();
    auto it = hgen.begin();
    auto val = *it;
    CHECK_EQ(val[0], doctest::Approx(0.5));
    CHECK_EQ(val[1], doctest::Approx(1.0 / 3.0));
}

TEST_CASE("Circle::peek") {
    auto cgen = lds::Circle<2>();
    auto peeked = cgen.peek();
    auto popped = cgen.pop();
    CHECK_EQ(peeked[0], doctest::Approx(popped[0]));
    CHECK_EQ(peeked[1], doctest::Approx(popped[1]));
}

TEST_CASE("Circle::batch") {
    auto cgen = lds::Circle<2>();
    auto batch = cgen.batch(3);
    CHECK_EQ(batch.size(), 3);
    double radius_squared = batch[0][0] * batch[0][0] + batch[0][1] * batch[0][1];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Circle::skip") {
    auto cgen = lds::Circle<2>();
    cgen.skip(5);
    auto popped = cgen.pop();
    double radius_squared = popped[0] * popped[0] + popped[1] * popped[1];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Circle::iterator") {
    auto cgen = lds::Circle<2>();
    auto it = cgen.begin();
    auto val = *it;
    double radius_squared = val[0] * val[0] + val[1] * val[1];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Disk::peek") {
    auto dgen = lds::Disk<2, 3>();
    auto peeked = dgen.peek();
    auto popped = dgen.pop();
    CHECK_EQ(peeked[0], doctest::Approx(popped[0]));
    CHECK_EQ(peeked[1], doctest::Approx(popped[1]));
}

TEST_CASE("Disk::batch") {
    auto dgen = lds::Disk<2, 3>();
    auto batch = dgen.batch(3);
    CHECK_EQ(batch.size(), 3);
    for (const auto& point : batch) {
        double radius_squared = point[0] * point[0] + point[1] * point[1];
        CHECK_LE(radius_squared, 1.0);
    }
}

TEST_CASE("Disk::skip") {
    auto dgen = lds::Disk<2, 3>();
    dgen.skip(5);
    auto popped = dgen.pop();
    double radius_squared = popped[0] * popped[0] + popped[1] * popped[1];
    CHECK_LE(radius_squared, 1.0);
}

TEST_CASE("Disk::iterator") {
    auto dgen = lds::Disk<2, 3>();
    auto it = dgen.begin();
    auto val = *it;
    double radius_squared = val[0] * val[0] + val[1] * val[1];
    CHECK_LE(radius_squared, 1.0);
}

TEST_CASE("Sphere::peek") {
    auto sgen = lds::Sphere<2, 3>();
    auto peeked = sgen.peek();
    auto popped = sgen.pop();
    CHECK_EQ(peeked[0], doctest::Approx(popped[0]));
    CHECK_EQ(peeked[1], doctest::Approx(popped[1]));
    CHECK_EQ(peeked[2], doctest::Approx(popped[2]));
}

TEST_CASE("Sphere::batch") {
    auto sgen = lds::Sphere<2, 3>();
    auto batch = sgen.batch(3);
    CHECK_EQ(batch.size(), 3);
    for (const auto& point : batch) {
        double radius_squared = point[0] * point[0] + point[1] * point[1] + point[2] * point[2];
        CHECK_EQ(radius_squared, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere::skip") {
    auto sgen = lds::Sphere<2, 3>();
    sgen.skip(5);
    auto popped = sgen.pop();
    double radius_squared = popped[0] * popped[0] + popped[1] * popped[1] + popped[2] * popped[2];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Sphere::iterator") {
    auto sgen = lds::Sphere<2, 3>();
    auto it = sgen.begin();
    auto val = *it;
    double radius_squared = val[0] * val[0] + val[1] * val[1] + val[2] * val[2];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Sphere3Hopf::peek") {
    auto shfgen = lds::Sphere3Hopf<2, 3, 5>();
    auto peeked = shfgen.peek();
    auto popped = shfgen.pop();
    CHECK_EQ(peeked[0], doctest::Approx(popped[0]));
    CHECK_EQ(peeked[1], doctest::Approx(popped[1]));
    CHECK_EQ(peeked[2], doctest::Approx(popped[2]));
    CHECK_EQ(peeked[3], doctest::Approx(popped[3]));
}

TEST_CASE("Sphere3Hopf::batch") {
    auto shfgen = lds::Sphere3Hopf<2, 3, 5>();
    auto batch = shfgen.batch(3);
    CHECK_EQ(batch.size(), 3);
    for (const auto& point : batch) {
        double radius_squared
            = point[0] * point[0] + point[1] * point[1] + point[2] * point[2] + point[3] * point[3];
        CHECK_EQ(radius_squared, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere3Hopf::skip") {
    auto shfgen = lds::Sphere3Hopf<2, 3, 5>();
    shfgen.skip(5);
    auto popped = shfgen.pop();
    double radius_squared = popped[0] * popped[0] + popped[1] * popped[1] + popped[2] * popped[2]
                            + popped[3] * popped[3];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("Sphere3Hopf::iterator") {
    auto shfgen = lds::Sphere3Hopf<2, 3, 5>();
    auto it = shfgen.begin();
    auto val = *it;
    double radius_squared = val[0] * val[0] + val[1] * val[1] + val[2] * val[2] + val[3] * val[3];
    CHECK_EQ(radius_squared, doctest::Approx(1.0));
}

TEST_CASE("get_index") {
    auto vgen = lds::VdCorput<2>();
    CHECK_EQ(vgen.get_index(), 0);
    vgen.pop();
    CHECK_EQ(vgen.get_index(), 1);
    vgen.pop();
    CHECK_EQ(vgen.get_index(), 2);
    vgen.reseed(5);
    CHECK_EQ(vgen.get_index(), 5);
}
