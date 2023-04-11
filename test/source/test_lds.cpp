#include <doctest/doctest.h> // for Approx, ResultBuilder, TestCase, CHECK

#include <lds/lds.hpp> // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("Circle") {
  auto cgen = lds2::Circle(2);
  const auto arr = cgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.0));
}

TEST_CASE("Halton") {
  const size_t base[] = {2, 3};
  auto hgen = lds2::Halton(base);
  const auto arr = hgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.5));
}

TEST_CASE("Sphere") {
  const size_t base[] = {2, 3};
  auto sgen = lds2::Sphere(base);
  const auto arr = sgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.8660254038));
}

TEST_CASE("Sphere3Hopf") {
  const size_t base[] = {2, 3, 5};
  auto shfgen = lds2::Sphere3Hopf(base);
  const auto arr = shfgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(-0.2236067977));
}
