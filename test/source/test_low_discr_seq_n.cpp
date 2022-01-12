#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <gsl/span>                 // for span
#include <lds/low_discr_seq_n.hpp>  // for cylin_n, halton_n, sphere3, sphere_n
#include <vector>                   // for vector

TEST_CASE("sphere3") {
    unsigned base[] = {2, 3, 5, 7};
    auto sp3gen = lds::sphere3(base);
    auto [x1, x2, x3, x4] = sp3gen();
    CHECK(x1 == doctest::Approx(0.8966646826));
}

TEST_CASE("halton_n") {
    unsigned base[] = {2, 3, 5, 7};
    auto hgen = lds::halton_n(base);
    auto res = hgen();
    CHECK(res[0] == doctest::Approx(0.5));
}

TEST_CASE("cylin_n") {
    unsigned base[] = {2, 3, 5, 7};
    auto cygen = lds::cylin_n(base);
    auto res = cygen();
    CHECK(res[0] == doctest::Approx(0.5896942325));
}

TEST_CASE("sphere_n") {
    unsigned base[] = {2, 3, 5, 7};
    auto spgen = lds::sphere_n(base);
    auto res = spgen();
    CHECK(res[0] == doctest::Approx(0.6092711237));
}
