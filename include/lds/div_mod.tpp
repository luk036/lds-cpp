#pragma once

// Require C++17

#include <cstdint>
#include <tuple>

// Optimized div_mod_3_iter with forced inline and reduced operations
template <typename T>
inline constexpr std::tuple<T, T> div_mod_3_iter(const T input) noexcept {
    const T q = input >> 2;
    return {q, q + (input & 0x03)};
}

// Optimized u8 division by 3 with loop unrolling and constexpr optimizations
constexpr std::tuple<uint8_t, uint8_t> div_mod_3_u8(const uint8_t n) noexcept {
    auto [q1, rem1] = div_mod_3_iter(n);
    auto [q2, rem2] = div_mod_3_iter(rem1);
    auto [q3, rem3] = div_mod_3_iter(rem2);
    auto [q4, rem4] = div_mod_3_iter(rem3);

    const uint8_t quotient_sum = q1 + q2 + q3 + q4;

    return rem4 == 3 ? std::make_tuple(quotient_sum + 1, 0)
                    : std::make_tuple(quotient_sum, rem4);
}

// Optimized u16 division by 3 with loop unrolling
constexpr std::tuple<uint16_t, uint16_t> div_mod_3_u16(const uint16_t n) noexcept {
    auto [q1, rem1] = div_mod_3_iter(n);
    auto [q2, rem2] = div_mod_3_iter(rem1);
    auto [q3, rem3] = div_mod_3_iter(rem2);
    auto [q4, rem4] = div_mod_3_iter(rem3);
    auto [q5, rem5] = div_mod_3_iter(rem4);
    auto [q6, rem6] = div_mod_3_iter(rem5);
    auto [q7, rem7] = div_mod_3_iter(rem6);
    auto [q8, rem8] = div_mod_3_iter(rem7);

    const uint16_t quotient_sum = q1 + q2 + q3 + q4 + q5 + q6 + q7 + q8;

    return rem8 == 3 ? std::make_tuple(quotient_sum + 1, 0)
                     : std::make_tuple(quotient_sum, rem8);
}

// Optimized div_mod_7_iter with forced inline
template <typename T>
inline constexpr std::tuple<T, T> div_mod_7_iter(const T input) noexcept {
    const T q = input >> 3;
    return {q, q + (input & 0x07)};
}

// Optimized u8 division by 7
constexpr std::tuple<uint8_t, uint8_t> div_mod_7_u8(const uint8_t n) noexcept {
    auto [q1, rem1] = div_mod_7_iter(n);
    auto [q2, rem2] = div_mod_7_iter(rem1);
    auto [q3, rem3] = div_mod_7_iter(rem2);

    const uint8_t quotient_sum = q1 + q2 + q3;

    return rem3 == 7 ? std::make_tuple(quotient_sum + 1, 0)
                     : std::make_tuple(quotient_sum, rem3);
}

// Optimized u16 division by 7
constexpr std::tuple<uint16_t, uint16_t> div_mod_7_u16(const uint16_t n) noexcept {
    auto [q1, rem1] = div_mod_7_iter(n);
    auto [q2, rem2] = div_mod_7_iter(rem1);
    auto [q3, rem3] = div_mod_7_iter(rem2);
    auto [q4, rem4] = div_mod_7_iter(rem3);
    auto [q5, rem5] = div_mod_7_iter(rem4);

    const uint16_t quotient_sum = q1 + q2 + q3 + q4 + q5;

    return rem5 == 7 ? std::make_tuple(quotient_sum + 1, 0)
                     : std::make_tuple(quotient_sum, rem5);
}