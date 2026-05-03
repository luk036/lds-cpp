#include <doctest/doctest.h>  // for ResultBuilder, TestCase, CHECK

#include <array>            // for array
#include <lds/ilds.hpp>  // for Halton

TEST_CASE("VdCorput_i") {
    auto vgen = ilds::VdCorput<2>(10);
    CHECK_EQ(vgen.pop(), 512);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
}

TEST_CASE("VdCorput_i multiple pops") {
    auto vgen = ilds::VdCorput<2>(10);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
    CHECK_EQ(vgen.pop(), 256);
    CHECK_EQ(vgen.pop(), 768);
    CHECK_EQ(vgen.pop(), 128);
}

TEST_CASE("VdCorput_i reseed with different values") {
    auto vgen = ilds::VdCorput<2>(10);
    vgen.reseed(1);
    CHECK_EQ(vgen.pop(), 256);
    vgen.reseed(2);
    CHECK_EQ(vgen.pop(), 768);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
}

TEST_CASE("VdCorput_i different base and scale") {
    auto vgen = ilds::VdCorput<3>(8);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 2187);  // 3^7 = 2187
}

TEST_CASE("Halton_i") {
    // const std::array<unsigned long, 2> base = {2, 3};
    const std::array<unsigned int, 2> scale = {11, 7};
    auto hgen = ilds::Halton<2, 3>(scale);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], 1024);
    CHECK_EQ(arr[1], 729);
    hgen.reseed(0);
    const auto arr2 = hgen.pop();
    CHECK_EQ(arr2[0], 1024);
    CHECK_EQ(arr2[1], 729);
}

TEST_CASE("Halton_i different bases and scales") {
    const std::array<unsigned int, 2> scale = {6, 4};
    auto hgen = ilds::Halton<3, 5>(scale);
    hgen.reseed(0);

    auto res = hgen.pop();
    CHECK_EQ(res[0], 243);  // 3^5 = 243
    CHECK_EQ(res[1], 125);  // 5^3 = 125
}
