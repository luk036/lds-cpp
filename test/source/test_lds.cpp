#include <doctest/doctest.h> // for Approx, ResultBuilder, TestCase, CHECK

#include <lds/lds.hpp> // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("Circle") {
  auto cgen = lds2::Circle(2);
  const auto arr = cgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.0));
}

TEST_CASE("Halton") {
  auto hgen = lds2::Halton(2, 3);
  const auto arr = hgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.5));
}

TEST_CASE("Sphere") {
  auto sgen = lds2::Sphere(2, 3);
  const auto arr = sgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(0.8660254038));
}

TEST_CASE("Sphere3Hopf") {
  auto shfgen = lds2::Sphere3Hopf(2, 3, 5);
  const auto arr = shfgen.pop();
  CHECK_EQ(arr[0], doctest::Approx(-0.2236067977));
}
