#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <gsl/span>                 // for span
#include <lds/lds_n.hpp>  // for cylin_n, halton_n, sphere3, sphere_n
#include <vector>                   // for vector

TEST_CASE("Sphere3") {
    size_t base[] = {2, 3, 5};
    auto sp3gen = lds2::Sphere3(base);
    auto [x1, x2, x3, x4] = sp3gen.pop();
    // CHECK(x1 == doctest::Approx(0.8966646826));
}

TEST_CASE("HaltonN") {
    size_t base[] = {2, 3, 5, 7};
    auto hgen = lds2::HaltonN(base);
    auto res = hgen.pop();
    CHECK(res[0] == doctest::Approx(0.5));
}

TEST_CASE("CylinN") {
    size_t base[] = {2, 3, 5, 7};
    auto cygen = lds2::CylinN(base);
    auto res = cygen.pop();
    CHECK(res[0] == doctest::Approx(0.5896942325));
}

TEST_CASE("SphereN") {
    size_t base[] = {2, 3, 5, 7};
    auto spgen = lds2::SphereN(base);
    auto res = spgen.pop();
    // CHECK(res[0] == doctest::Approx(0.6092711237));
}
