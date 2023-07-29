#include <doctest/doctest.h> // for ResultBuilder, TestCase, CHECK

#include <lds/ilds.hpp> // for Halton

TEST_CASE("Halton") {
    const size_t base[] = {2, 3};
    const unsigned int scale[] = {11, 7};
    auto hgen = ilds2::Halton(base, scale);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], 1024);
    CHECK_EQ(arr[1], 729);
}
