#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <array>
#include <cmath>
#include <lds/sphere_n.hpp>
#include <numeric>
#include <vector>

TEST_CASE("Test linspace function") {
    auto result = lds::linspace(0.0, 1.0, 5);
    REQUIRE(result.size() == 5);
    CHECK_EQ(result[0], doctest::Approx(0.0));
    CHECK_EQ(result[1], doctest::Approx(0.25));
    CHECK_EQ(result[2], doctest::Approx(0.5));
    CHECK_EQ(result[3], doctest::Approx(0.75));
    CHECK_EQ(result[4], doctest::Approx(1.0));

    result = lds::linspace(0.0, 1.0, 1);
    REQUIRE(result.size() == 1);
    CHECK_EQ(result[0], doctest::Approx(0.0));

    result = lds::linspace(-1.0, 1.0, 3);
    REQUIRE(result.size() == 3);
    CHECK_EQ(result[0], doctest::Approx(-1.0));
    CHECK_EQ(result[1], doctest::Approx(0.0));
    CHECK_EQ(result[2], doctest::Approx(1.0));
}

TEST_CASE("Test simple_interp function") {
    std::vector<double> xp = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> yp = {0.0, 2.0, 4.0, 6.0};  // Linear function y = 2x

    CHECK_EQ(lds::simple_interp(0.5, xp, yp), doctest::Approx(1.0));
    CHECK_EQ(lds::simple_interp(1.5, xp, yp), doctest::Approx(3.0));
    CHECK_EQ(lds::simple_interp(-0.5, xp, yp), doctest::Approx(0.0));
    CHECK_EQ(lds::simple_interp(3.5, xp, yp), doctest::Approx(6.0));
    CHECK_EQ(lds::simple_interp(2.0, xp, yp), doctest::Approx(4.0));
}

TEST_CASE("Test get_tp function") {
    auto tp0 = lds::get_tp(0);
    REQUIRE(tp0.size() == 300);
    CHECK_EQ(tp0[0], doctest::Approx(0.0));
    CHECK_EQ(tp0.back(), doctest::Approx(lds::PI).epsilon(1e-10));

    auto tp1 = lds::get_tp(1);
    REQUIRE(tp1.size() == 300);
    CHECK_EQ(tp1[0], doctest::Approx(-std::cos(0.0)));
    CHECK_EQ(tp1.back(), doctest::Approx(-std::cos(lds::PI)).epsilon(1e-10));

    auto tp2 = lds::get_tp(2);
    REQUIRE(tp2.size() == 300);
}

TEST_CASE("Test Sphere3 basic functionality") {
    std::vector<std::uint64_t> base = {2, 3, 5};
    lds::Sphere3 sgen(base);
    sgen.reseed(0);

    auto point = sgen.pop();
    REQUIRE(point.size() == 4);

    // Check if point is on unit 3-sphere (approximately)
    double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
    CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));

    // Check values are in reasonable range
    for (double coord : point) {
        CHECK_GE(coord, -1.0 - 1e-10);
        CHECK_LE(coord, 1.0 + 1e-10);
    }
}

TEST_CASE("Test Sphere3 consistency") {
    std::vector<std::vector<std::uint64_t>> bases = {{2, 3, 5}, {2, 5, 3}, {3, 2, 7}};

    for (const auto& base : bases) {
        lds::Sphere3 sgen(base);
        sgen.reseed(0);

        // Generate multiple points
        std::vector<std::vector<double>> points;
        for (int i = 0; i < 5; ++i) {
            points.emplace_back(sgen.pop());
        }

        // Check all points are on unit 3-sphere
        for (const auto& point : points) {
            double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
            CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
        }
    }
}

TEST_CASE("Test Sphere3 reseed functionality") {
    std::vector<std::uint64_t> base = {2, 3, 5};
    lds::Sphere3 sgen(base);

    // Generate sequence with seed 0
    sgen.reseed(0);
    std::vector<std::vector<double>> seq1;
    for (int i = 0; i < 3; ++i) {
        seq1.emplace_back(sgen.pop());
    }

    // Generate sequence with seed 0 again
    sgen.reseed(0);
    std::vector<std::vector<double>> seq2;
    for (int i = 0; i < 3; ++i) {
        seq2.emplace_back(sgen.pop());
    }

    // Should be identical
    for (unsigned long i = 0; i < 3; ++i) {
        for (unsigned long j = 0; j < 4; ++j) {
            CHECK_EQ(seq1[i][j], doctest::Approx(seq2[i][j]).epsilon(1e-10));
        }
    }

    // Different seed should give different sequence
    sgen.reseed(1);
    std::vector<std::vector<double>> seq3;
    for (int i = 0; i < 3; ++i) {
        seq3.emplace_back(sgen.pop());
    }

    // Should be different from seed 0
    bool different = false;
    for (unsigned long i = 0; i < 3 && !different; ++i) {
        for (unsigned long j = 0; j < 4; ++j) {
            if (std::abs(seq1[i][j] - seq3[i][j]) > 1e-10) {
                different = true;
                break;
            }
        }
    }
    CHECK(different);
}

TEST_CASE("Test SphereN basic functionality") {
    // Test 4-sphere (5D)
    std::vector<std::uint64_t> base = {2, 3, 5, 7};
    lds::SphereN sgen(base);
    sgen.reseed(0);

    auto point = sgen.pop();
    REQUIRE(point.size() == 5);  // 4 bases produce 5D point

    // Check if point is on unit 4-sphere (approximately)
    double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
    CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
}

TEST_CASE("Test SphereN higher dimensions") {
    // Test 5-sphere (6D)
    std::vector<std::uint64_t> base = {2, 3, 5, 7, 11};
    lds::SphereN sgen(base);
    sgen.reseed(0);

    auto point = sgen.pop();
    REQUIRE(point.size() == 6);  // 5 bases produce 6D point

    // Check if point is on unit 5-sphere (approximately)
    double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
    CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
}

TEST_CASE("Test SphereN reseed functionality") {
    std::vector<std::uint64_t> base = {2, 3, 5, 7};
    lds::SphereN sgen(base);

    // Generate sequence with seed 0
    sgen.reseed(0);
    std::vector<std::vector<double>> seq1;
    for (int i = 0; i < 3; ++i) {
        seq1.emplace_back(sgen.pop());
    }

    // Generate sequence with seed 0 again
    sgen.reseed(0);
    std::vector<std::vector<double>> seq2;
    for (int i = 0; i < 3; ++i) {
        seq2.emplace_back(sgen.pop());
    }

    // Should be identical
    for (unsigned long i = 0; i < 3; ++i) {
        for (unsigned long j = 0; j < 4; ++j) {
            CHECK_EQ(seq1[i][j], doctest::Approx(seq2[i][j]).epsilon(1e-10));
        }
    }
}

TEST_CASE("Test comparison with Python implementation") {
    // Expected values from Python doctest examples
    std::vector<double> expected_sphere3
        = {0.2913440162992141, 0.8966646826186098, -0.33333333333333337, 6.123233995736766e-17};

    std::vector<double> expected_spheren
        = {0.4809684718990214, 0.6031153874276115, -0.5785601510223212, 0.2649326520763179,
           6.123233995736766e-17};

    // Test Sphere3
    std::vector<std::uint64_t> base3 = {2, 3, 5};
    lds::Sphere3 sgen3(base3);
    sgen3.reseed(0);
    auto result3 = sgen3.pop();

    REQUIRE(result3.size() == expected_sphere3.size());
    for (std::unsigned long i = 0; i < result3.size(); ++i) {
        CHECK_EQ(result3[i], doctest::Approx(expected_sphere3[i]).epsilon(1e-10));
    }

    // Test SphereN
    std::vector<std::uint64_t> baseN = {2, 3, 5, 7};
    lds::SphereN sgenN(baseN);
    sgenN.reseed(0);
    auto resultN = sgenN.pop();

    REQUIRE(resultN.size() == expected_spheren.size());
    for (std::unsigned long i = 0; i < resultN.size(); ++i) {
        CHECK_EQ(resultN[i], doctest::Approx(expected_spheren[i]).epsilon(1e-10));
    }
}

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

TEST_CASE("Sphere3 thread safety") {
    const int num_threads = 8;
    const int values_per_thread = 50;
    std::vector<std::uint64_t> base = {2, 3, 5};
    lds::Sphere3 sgen(base);
    std::vector<std::thread> threads;
    std::vector<std::vector<std::vector<double>>> results(num_threads);
    std::mutex mtx;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&sgen, &results, &mtx, i]() {
            std::vector<std::vector<double>> local_results;
            for (int j = 0; j < values_per_thread; ++j) {
                local_results.emplace_back(sgen.pop());
            }
            std::lock_guard<std::mutex> lock(mtx);
            results[static_cast<unsigned long>(i)] = std::move(local_results);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check that we got the expected number of values
    unsigned long total_points = 0;
    for (const auto& thread_results : results) {
        total_points += thread_results.size();
        // Verify all points are on unit 3-sphere
        for (const auto& point : thread_results) {
            REQUIRE(point.size() == 4);
            double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
            CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
        }
    }
    CHECK_EQ(total_points, num_threads * values_per_thread);
}

TEST_CASE("SphereN thread safety") {
    const int num_threads = 8;
    const int values_per_thread = 50;
    std::vector<std::uint64_t> base = {2, 3, 5, 7, 11};
    lds::SphereN sgen(base);
    std::vector<std::thread> threads;
    std::vector<std::vector<std::vector<double>>> results(num_threads);
    std::mutex mtx;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&sgen, &results, &mtx, i]() {
            std::vector<std::vector<double>> local_results;
            for (int j = 0; j < values_per_thread; ++j) {
                local_results.emplace_back(sgen.pop());
            }
            std::lock_guard<std::mutex> lock(mtx);
            results[static_cast<unsigned long>(i)] = std::move(local_results);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check that we got the expected number of values
    unsigned long total_points = 0;
    for (const auto& thread_results : results) {
        total_points += thread_results.size();
        // Verify all points are on unit N-sphere
        for (const auto& point : thread_results) {
            REQUIRE(point.size() == 6);  // 5 bases produce 6D point
            double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
            CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
        }
    }
    CHECK_EQ(total_points, num_threads * values_per_thread);
}

TEST_CASE("SphereWrapper thread safety") {
    const int num_threads = 8;
    const int values_per_thread = 50;
    std::vector<std::uint64_t> base = {2, 3};
    lds::SphereWrapper sgen(base);
    std::vector<std::thread> threads;
    std::vector<std::vector<std::vector<double>>> results(num_threads);
    std::mutex mtx;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&sgen, &results, &mtx, i]() {
            std::vector<std::vector<double>> local_results;
            for (int j = 0; j < values_per_thread; ++j) {
                local_results.emplace_back(sgen.pop());
            }
            std::lock_guard<std::mutex> lock(mtx);
            results[static_cast<unsigned long>(i)] = std::move(local_results);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check that we got the expected number of values
    unsigned long total_points = 0;
    for (const auto& thread_results : results) {
        total_points += thread_results.size();
        // Verify all points are on unit sphere
        for (const auto& point : thread_results) {
            REQUIRE(point.size() == 3);
            double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
            CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
        }
    }
    CHECK_EQ(total_points, num_threads * values_per_thread);
}

TEST_CASE("Concurrent reseed thread safety for sphere classes") {
    const int num_threads = 8;
    const int operations_per_thread = 25;
    std::vector<std::uint64_t> base3 = {2, 3, 5};
    std::vector<std::uint64_t> baseN = {2, 3, 5, 7};
    lds::Sphere3 sgen3(base3);
    lds::SphereN sgenN(baseN);
    std::vector<std::thread> threads;
    std::atomic<int> pop_count{0};
    std::atomic<int> reseed_count{0};
    std::mutex mtx;
    std::vector<std::vector<double>> results;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&sgen3, &sgenN, &pop_count, &reseed_count, &mtx, &results, i]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                if (j % 5 == 0) {
                    // Occasionally reseed
                    if (i % 2 == 0) {
                        sgen3.reseed(static_cast<unsigned long>(i * 10 + j));
                    } else {
                        sgenN.reseed(static_cast<unsigned long>(i * 10 + j));
                    }
                    reseed_count++;
                } else {
                    // Mostly pop
                    std::vector<double> point;
                    if (i % 2 == 0) {
                        point = sgen3.pop();
                    } else {
                        point = sgenN.pop();
                    }
                    std::lock_guard<std::mutex> lock(mtx);
                    results.emplace_back(point);
                    pop_count++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check that operations completed without crashes
    CHECK_GT(pop_count.load(), 0);
    CHECK_GT(reseed_count.load(), 0);
    CHECK_EQ(results.size(), pop_count.load());

    // Verify all generated points are valid
    for (const auto& point : results) {
        double radius_sq = std::inner_product(point.begin(), point.end(), point.begin(), 0.0);
        CHECK_EQ(radius_sq, doctest::Approx(1.0).epsilon(1e-10));
    }
}