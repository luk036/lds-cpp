#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <lds/lds_n.hpp>  // for halton_n

TEST_CASE("HaltonN") {
    auto hgen = lds::HaltonN<3>({2, 3, 5});
    hgen.reseed(1);
    auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.25));
    CHECK_EQ(res[1], doctest::Approx(2.0 / 3.0));
    CHECK_EQ(res[2], doctest::Approx(2.0 / 5.0));
    hgen.reseed(0);
    res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0 / 3.0));
    CHECK_EQ(res[2], doctest::Approx(1.0 / 5.0));
}
