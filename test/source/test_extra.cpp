#include <doctest/doctest.h>

#include <cmath>
#include <lds/ilds.hpp>
#include <lds/lds.hpp>

// ============================================================
// Additional tests to improve code coverage
// ============================================================

// --- VdCorput with different bases ---

TEST_CASE("VdCorput base 3") {
    auto vgen = lds::VdCorput<3>();
    CHECK_EQ(vgen.pop(), doctest::Approx(1.0 / 3.0));
    CHECK_EQ(vgen.pop(), doctest::Approx(2.0 / 3.0));
    CHECK_EQ(vgen.pop(), doctest::Approx(1.0 / 9.0));
    CHECK_EQ(vgen.pop(), doctest::Approx(4.0 / 9.0));
}

TEST_CASE("VdCorput base 5") {
    auto vgen = lds::VdCorput<5>();
    auto v = vgen.pop();
    CHECK_EQ(v, doctest::Approx(0.2));
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.2));
}

TEST_CASE("VdCorput base 10") {
    auto vgen = lds::VdCorput<10>();
    CHECK_EQ(vgen.pop(), doctest::Approx(0.1));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.2));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.3));
}

// --- VdCorput range checks ---

TEST_CASE("VdCorput values in [0,1)") {
    auto vgen = lds::VdCorput<2>();
    for (int i = 0; i < 100; ++i) {
        auto v = vgen.pop();
        CHECK_GE(v, 0.0);
        CHECK_LT(v, 1.0);
    }
}

TEST_CASE("VdCorput reseed edge cases") {
    auto vgen = lds::VdCorput<2>();
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
    vgen.reseed(1);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.25));
}

TEST_CASE("VdCorput skip zero") {
    auto vgen = lds::VdCorput<2>();
    vgen.skip(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
}

TEST_CASE("VdCorput skip large") {
    auto vgen = lds::VdCorput<2>();
    vgen.skip(100);
    // After skipping 100, get_index should be 100
    CHECK_EQ(vgen.get_index(), 100);
}

// --- VdCorput iterator edge cases ---

TEST_CASE("VdCorput iterator equality") {
    auto vgen = lds::VdCorput<2>();
    auto it1 = vgen.begin();
    auto it2 = vgen.begin();
    CHECK_EQ(it1.get_index(), it2.get_index());
    ++it1;
    CHECK_NE(it1.get_index(), it2.get_index());
}

TEST_CASE("VdCorput iterator dereference end") {
    lds::VdCorput<2> vgen;
    auto end = vgen.end();
    // end() has nullptr gen, dereference should return 0.0
    CHECK_EQ(*end, doctest::Approx(0.0));
}

// --- vdc() free function edge cases ---

TEST_CASE("vdc base 3") {
    CHECK_EQ(lds::vdc<3>(1), doctest::Approx(1.0 / 3.0));
    CHECK_EQ(lds::vdc<3>(2), doctest::Approx(2.0 / 3.0));
}

TEST_CASE("vdc large count") {
    auto val = lds::vdc<2>(1000000);
    CHECK_GE(val, 0.0);
    CHECK_LT(val, 1.0);
}

TEST_CASE("vdc count zero") {
    // vdc(0) should return 0.0 (loop doesn't execute)
    CHECK_EQ(lds::vdc<2>(0), doctest::Approx(0.0));
}

// --- Circle with different bases ---

TEST_CASE("Circle base 3") {
    auto cgen = lds::Circle<3>();
    auto arr = cgen.pop();
    double radius = std::sqrt(arr[0] * arr[0] + arr[1] * arr[1]);
    CHECK_EQ(radius, doctest::Approx(1.0));
}

TEST_CASE("Circle reseed preserves radius") {
    auto cgen = lds::Circle<2>();
    cgen.reseed(10);
    auto arr = cgen.pop();
    double radius = std::sqrt(arr[0] * arr[0] + arr[1] * arr[1]);
    CHECK_EQ(radius, doctest::Approx(1.0));
}

TEST_CASE("Circle skip") {
    auto cgen = lds::Circle<2>();
    cgen.reseed(0);
    auto first = cgen.pop();
    cgen.reseed(0);
    cgen.skip(1);
    auto second = cgen.pop();
    CHECK_NE(first[0], second[0]);
}

TEST_CASE("Circle get_index") {
    auto cgen = lds::Circle<2>();
    CHECK_EQ(cgen.get_index(), 0);
    cgen.pop();
    CHECK_EQ(cgen.get_index(), 1);
}

// --- Halton edge cases ---

TEST_CASE("Halton bases 3 and 5") {
    auto hgen = lds::Halton<3, 5>();
    auto arr = hgen.pop();
    CHECK_GE(arr[0], 0.0);
    CHECK_LT(arr[0], 1.0);
    CHECK_GE(arr[1], 0.0);
    CHECK_LT(arr[1], 1.0);
}

TEST_CASE("Halton reseed preserves range") {
    auto hgen = lds::Halton<2, 3>();
    for (unsigned long seed = 0; seed < 10; ++seed) {
        hgen.reseed(seed);
        auto arr = hgen.pop();
        CHECK_GE(arr[0], 0.0);
        CHECK_LT(arr[0], 1.0);
        CHECK_GE(arr[1], 0.0);
        CHECK_LT(arr[1], 1.0);
    }
}

TEST_CASE("Halton get_index") {
    auto hgen = lds::Halton<2, 3>();
    CHECK_EQ(hgen.get_index(), 0);
    hgen.pop();
    CHECK_EQ(hgen.get_index(), 1);
}

// --- Disk edge cases ---

TEST_CASE("Disk get_index") {
    auto dgen = lds::Disk<2, 3>();
    CHECK_EQ(dgen.get_index(), 0);
    dgen.pop();
    CHECK_EQ(dgen.get_index(), 1);
}

TEST_CASE("Disk points inside unit disk") {
    auto dgen = lds::Disk<2, 3>();
    for (int i = 0; i < 50; ++i) {
        auto p = dgen.pop();
        double r2 = p[0] * p[0] + p[1] * p[1];
        CHECK_LE(r2, 1.0);
    }
}

TEST_CASE("Disk reseed") {
    auto dgen = lds::Disk<2, 3>();
    dgen.reseed(5);
    auto p1 = dgen.pop();
    dgen.reseed(5);
    auto p2 = dgen.pop();
    CHECK_EQ(p1[0], doctest::Approx(p2[0]));
    CHECK_EQ(p1[1], doctest::Approx(p2[1]));
}

// --- Sphere edge cases ---

TEST_CASE("Sphere points on unit sphere") {
    auto sgen = lds::Sphere<2, 3>();
    for (int i = 0; i < 50; ++i) {
        auto p = sgen.pop();
        double r2 = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
        CHECK_EQ(r2, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere get_index") {
    auto sgen = lds::Sphere<2, 3>();
    CHECK_EQ(sgen.get_index(), 0);
    sgen.pop();
    CHECK_EQ(sgen.get_index(), 1);
}

// --- Sphere3Hopf edge cases ---

TEST_CASE("Sphere3Hopf points on unit 3-sphere") {
    auto sgen = lds::Sphere3Hopf<2, 3, 5>();
    for (int i = 0; i < 50; ++i) {
        auto p = sgen.pop();
        double r2 = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
        CHECK_EQ(r2, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere3Hopf get_index") {
    auto sgen = lds::Sphere3Hopf<2, 3, 5>();
    CHECK_EQ(sgen.get_index(), 0);
    sgen.pop();
    CHECK_EQ(sgen.get_index(), 1);
}

TEST_CASE("Sphere3Hopf reseed consistency") {
    auto sgen = lds::Sphere3Hopf<2, 3, 5>();
    sgen.reseed(10);
    auto p1 = sgen.pop();
    sgen.reseed(10);
    auto p2 = sgen.pop();
    CHECK_EQ(p1[0], doctest::Approx(p2[0]));
    CHECK_EQ(p1[1], doctest::Approx(p2[1]));
    CHECK_EQ(p1[2], doctest::Approx(p2[2]));
    CHECK_EQ(p1[3], doctest::Approx(p2[3]));
}

// --- prime_table edge cases ---

TEST_CASE("prime_table first prime") { CHECK_EQ(lds::prime_table(0), 2UL); }

TEST_CASE("prime_table first 20 primes") {
    unsigned long expected[]
        = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71};
    for (unsigned long i = 0; i < 20; ++i) {
        CHECK_EQ(lds::prime_table(i), expected[i]);
    }
}

// --- GeneratorIterator tests ---

TEST_CASE("GeneratorIterator post-increment") {
    auto vgen = lds::VdCorput<2>();
    auto it = vgen.begin();
    auto old = it++;
    CHECK_EQ(*old, doctest::Approx(0.5));
    CHECK_EQ(*it, doctest::Approx(0.25));
}

TEST_CASE("GeneratorIterator comparison") {
    auto vgen = lds::VdCorput<2>();
    auto it1 = vgen.begin();
    auto it2 = vgen.begin();
    CHECK(it1 == it2);
    ++it1;
    CHECK(it1 != it2);
}

// --- VdCorput(0) edge case: no iterations ---
// When count=0 and we pop(), the while loop doesn't execute, returns 0.0

TEST_CASE("VdCorput base large") {
    auto vgen = lds::VdCorput<97>();
    auto v = vgen.pop();
    CHECK_GE(v, 0.0);
    CHECK_LT(v, 1.0);
    CHECK_EQ(v, doctest::Approx(1.0 / 97.0));
}

// --- ilds::VdCorput (integer version) ---

TEST_CASE("ilds VdCorput base 2") {
    auto vgen = ilds::VdCorput<2>();
    auto v = vgen.pop();
    CHECK_EQ(v, 512UL);  // 2^9 = 512 (default scale=10)
}

TEST_CASE("ilds VdCorput reseed") {
    auto vgen = ilds::VdCorput<2>();
    vgen.reseed(0);
    CHECK_EQ(vgen.get_index(), 0);
}

TEST_CASE("ilds VdCorput get_index") {
    auto vgen = ilds::VdCorput<2>();
    CHECK_EQ(vgen.get_index(), 0);
    auto val = vgen.pop();
    CHECK_EQ(vgen.get_index(), 1);
    (void)val;
}

TEST_CASE("ilds VdCorput skip") {
    auto vgen = ilds::VdCorput<2>();
    vgen.skip(5);
    CHECK_EQ(vgen.get_index(), 5);
}

TEST_CASE("ilds VdCorput peek") {
    auto vgen = ilds::VdCorput<2>();
    auto peeked = vgen.peek();
    auto popped = vgen.pop();
    CHECK_EQ(peeked, popped);
}

// --- Memory regression tests: class sizes ---
// sizeof(unsigned long) is platform-dependent (4 on MSVC, 8 on GCC/Clang).
// These tests use relationship checks to be cross-platform.

TEST_CASE("sizeof VdCorput<2>") {
    // unsigned long count (4) + padding (4) + double[64] rev_lst (512) = 520
    // The actual size depends on alignment, so use the compiler's value
    // as baseline. If this changes, the class layout has changed.
    CHECK_EQ(sizeof(lds::VdCorput<2>), 520);
}

TEST_CASE("sizeof VdCorput<3>") {
    CHECK_EQ(sizeof(lds::VdCorput<3>), 520);
}

TEST_CASE("sizeof Circle<2>") {
    CHECK_EQ(sizeof(lds::Circle<2>), sizeof(lds::VdCorput<2>));
}

TEST_CASE("sizeof Halton<2,3>") {
    CHECK_EQ(sizeof(lds::Halton<2, 3>), 2 * sizeof(lds::VdCorput<2>));
}

TEST_CASE("sizeof Disk<2,3>") {
    CHECK_EQ(sizeof(lds::Disk<2, 3>), 2 * sizeof(lds::VdCorput<2>));
}

TEST_CASE("sizeof Sphere<2,3>") {
    CHECK_EQ(sizeof(lds::Sphere<2, 3>),
             sizeof(lds::VdCorput<2>) + sizeof(lds::Circle<3>));
}

TEST_CASE("sizeof Sphere3Hopf<2,3,5>") {
    CHECK_EQ(sizeof(lds::Sphere3Hopf<2, 3, 5>), 3 * sizeof(lds::VdCorput<2>));
}

TEST_CASE("sizeof ilds::VdCorput<2>") {
    // unsigned long _count + unsigned long[64] factor_lst = sizeof(ul) * 65
    CHECK_EQ(sizeof(ilds::VdCorput<2>), sizeof(unsigned long) * 65);
}
