#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <gsl/span>     // for span
#include <lds/lds.hpp>  // for circle, halton, sphere, sphere3_hopf

TEST_CASE("Circle") {
    auto cgen = lds2::Circle(2);
    auto [x, y] = cgen.pop();
    CHECK(x == doctest::Approx(0.0));
}

TEST_CASE("Halton") {
    size_t base[] = {2, 3};
    auto hgen = lds2::Halton(base);
    auto [x, y] = hgen.pop();
    CHECK(x == doctest::Approx(0.5));
}

TEST_CASE("Sphere") {
    size_t base[] = {2, 3};
    auto sgen = lds2::Sphere(base);
    auto [s0, s1, s2] = sgen.pop();
    CHECK(s0 == doctest::Approx(0.8660254038));
}

TEST_CASE("Sphere3Hopf") {
    size_t base[] = {2, 3, 5};
    auto shfgen = lds2::Sphere3Hopf(base);
    auto [s0, s1, s2, s3] = shfgen.pop();
    CHECK(s0 == doctest::Approx(-0.2236067977));
}
