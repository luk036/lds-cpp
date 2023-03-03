#include <doctest/doctest.h> // for Approx, ResultBuilder, TestCase

#include <lds/lds_n.hpp> // for cylin_n, halton_n, sphere3, sphere_n
#include <vector>

TEST_CASE("HaltonN") {
  const std::vector<size_t> base = {2, 3, 5, 7};
  auto hgen = lds2::HaltonN(base);
  const auto res = hgen.pop();
  CHECK_EQ(res[0], doctest::Approx(0.5));
}
