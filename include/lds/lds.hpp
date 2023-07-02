#pragma once

#include <array>
#include <cmath>

#if __cpp_constexpr >= 201304
#define CONSTEXPR14 constexpr
#else
#define CONSTEXPR14 inline
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

namespace lds2 {

constexpr const auto TWO_PI = 2.0 * M_PI;

/**
 * @brief Van der Corput sequence
 *
 * The `vdc` function is calculating the Van der Corput sequence value for a
 * given index `k` and base `base`. It returns a `double` value.
 *
 * @param k
 * @param base
 * @return double
 */
CONSTEXPR14 auto vdc(size_t k, const size_t base) -> double {
  auto vdc = 0.0;
  auto denom = 1.0;
  for (; k != 0; k /= base) {
    const auto remainder = k % base;
    denom *= double(base);
    vdc += double(remainder) / denom;
  }
  return vdc;
}

/**
 * @brief Van der Corput sequence generator
 *
 * `VdCorput` is a class that generates the Van der Corput sequence. The Van der
 * Corput sequence is a low-discrepancy sequence that is commonly used in
 * quasi-Monte Carlo methods. The sequence is generated by iterating over a base
 * and calculating the fractional part of the number in that base. The
 * `VdCorput` class keeps track of the current count and base, and provides a
 * `pop()` method that returns the next value in the sequence.
 */
class VdCorput {
  size_t count;
  size_t base;

public:
  /**
   * @brief Construct a new VdCorput object
   *
   * The `VdCorput(size_t base)` constructor is initializing a `VdCorput` object
   * with a given base. The base is used to generate the Van der Corput
   * sequence.
   *
   * @param base
   */
  CONSTEXPR14 explicit VdCorput(size_t base) : count{0}, base{base} {}

  /**
   * @brief pop
   *
   * The `auto pop() -> double` function is a member function of the `VdCorput`
   * class. It returns a `double` value. This function is used to generate the
   * next value in the Van der Corput sequence. It increments the count and
   * calculates the Van der Corput sequence value for that count and base.
   *
   * @return double
   */
  CONSTEXPR14 auto pop() -> double {
    this->count += 1;
    return vdc(this->count, this->base);
  }

  /**
   * @brief reseed
   *
   * The `reseed(size_t seed)` function is used to reset the state of the
   * sequence generator to a specific seed value. This allows the sequence
   * generator to start generating the sequence from the beginning, or from a
   * specific point in the sequence, depending on the value of the seed.
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void { this->count = seed; }
};

/**
 * @brief Halton sequence generator
 *
 * class The `Halton` class is a sequence generator that generates points in a
 * 2-dimensional space using the Halton sequence. The Halton sequence is a
 * low-discrepancy sequence that is commonly used in quasi-Monte Carlo methods.
 * It is generated by iterating over two different bases and calculating the
 * fractional parts of the numbers in those bases. The `Halton` class keeps
 * track of the current count and bases, and provides a `pop()` method that
 * returns the next point in the sequence as a `std::array<double, 2>`.
 */
class Halton {
  VdCorput vdc0;
  VdCorput vdc1;

public:
  /**
   * @brief Construct a new Halton object
   *
   * The `Halton(const size_t base0, const size_t base1)` is a constructor for
   * the `Halton` class. It takes two parameters `base0` and `base1`, which are
   * used as the bases for generating the Halton sequence. The `CONSTEXPR14`
   * keyword indicates that the constructor is constexpr, meaning it can be
   * evaluated at compile-time if possible.
   *
   * @param base0
   * @param base1
   */
  CONSTEXPR14 Halton(const size_t base0, const size_t base1)
      : vdc0(base0), vdc1(base1) {}

  /**
   * @brief pop
   *
   * The `pop()` function is used to generate the next value in the sequence.
   * For example, in the `VdCorput` class, `pop()` increments the count and
   * calculates the Van der Corput sequence value for that count and base. In
   * the `Halton` class, `pop()` returns the next point in the Halton sequence
   * as a `std::array<double, 2>`. Similarly, in the `Circle` class, `pop()`
   * returns the next point on the unit circle as a `std::array<double, 2>`. In
   * the `Sphere` class, `pop()` returns the next point on the unit sphere as a
   * `std::array<double, 3>`. And in the `Sphere3Hopf` class, `pop()` returns
   * the next point on the 3-sphere using the Hopf fibration as a
   * `std::array<double, 4>`.
   *
   * @return std::array<double, 2>
   */
  CONSTEXPR14 auto pop() -> std::array<double, 2> { //
    return {this->vdc0.pop(), this->vdc1.pop()};
  }

  /**
   * @brief reseed
   *
   * The `reseed(size_t seed)` function is used to reset the state of the
   * sequence generator to a specific seed value. This allows the sequence
   * generator to start generating the sequence from the beginning, or from a
   * specific point in the sequence, depending on the value of the seed.
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void {
    this->vdc0.reseed(seed);
    this->vdc1.reseed(seed);
  }
};

/**
 * @brief Circle sequence generator
 *
 * The `Circle` class is a sequence generator that generates points on a unit
 * circle using the Van der Corput sequence. It uses the `VdCorput` class to
 * generate the sequence values and maps them to points on the unit circle. The
 * `pop()` method returns the next point on the unit circle as a
 * `std::array<double, 2>`, where the first element represents the x-coordinate
 * and the second element represents the y-coordinate of the point. The
 * `reseed()` method is used to reset the state of the sequence generator to a
 * specific seed value.
 *
 */
class Circle {
  VdCorput vdc;

public:
  /**
   * @brief Construct a new Circle object
   *
   * The `Circle(size_t base)` constructor is initializing a `Circle` object
   * with a given base. The base is used to generate the Van der Corput
   * sequence, which is then mapped to points on the unit circle. The `explicit`
   * keyword indicates that this constructor can only be used for explicit
   * construction and not for implicit conversions.
   *
   * @param base
   */
  CONSTEXPR14 explicit Circle(size_t base) : vdc(base) {}

  /**
   * @brief pop
   *
   * The `pop()` function is used to generate the next value in the sequence. In
   * the `VdCorput` class, `pop()` increments the count and calculates the Van
   * der Corput sequence value for that count and base. In the `Halton` class,
   * `pop()` returns the next point in the Halton sequence as a
   * `std::array<double, 2>`. Similarly, in the `Circle` class, `pop()` returns
   * the next point on the unit circle as a `std::array<double, 2>`. In the
   * `Sphere` class, `pop()` returns the next point on the unit sphere as a
   * `std::array<double, 3>`. And in the `Sphere3Hopf` class, `pop()` returns
   * the next point on the 3-sphere using the Hopf fibration as a
   * `std::array<double, 4>`.
   *
   * @return std::array<double, 2>
   */
  inline auto pop() -> std::array<double, 2> {
    auto theta = this->vdc.pop() * TWO_PI; // map to [0, 2*pi];
    return {std::sin(theta), std::cos(std::move(theta))};
  }

  /**
   * @brief reseed
   *
   * The `reseed(size_t seed)` function is used to reset the state of the
   * sequence generator to a specific seed value. This allows the sequence
   * generator to start generating the sequence from the beginning, or from a
   * specific point in the sequence, depending on the value of the seed.
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void { this->vdc.reseed(seed); }
};

/**
 * @brief Sphere sequence generator
 *
 * The `Sphere` class is a sequence generator that generates points on a unit
 * sphere using the Van der Corput sequence. It uses the `VdCorput` class to
 * generate the sequence values and maps them to points on the unit sphere. The
 * `pop()` method returns the next point on the unit sphere as a
 * `std::array<double, 3>`, where the first element represents the x-coordinate,
 * the second element represents the y-coordinate of the point, and the third
 * element represents the z-coordinate of the point. The
 * `reseed()` method is used to reset the state of the sequence generator to a
 * specific seed value.
 */
class Sphere {
  VdCorput vdcgen;
  Circle cirgen;

public:
  /**
   * @brief Construct a new Sphere object
   *
   * @param base0
   * @param base1
   */
  CONSTEXPR14 Sphere(const size_t base0, const size_t base1)
      : vdcgen(base0), cirgen(base1) {}

  /**
   * @brief pop
   *
   * The `pop()` function is used to generate the next value in the sequence. In
   * the `VdCorput` class, `pop()` increments the count and calculates the Van
   * der Corput sequence value for that count and base. In the `Halton` class,
   * `pop()` returns the next point in the Halton sequence as a
   * `std::array<double, 2>`. Similarly, in the `Circle` class, `pop()` returns
   * the next point on the unit circle as a `std::array<double, 2>`. In the
   * `Sphere` class, `pop()` returns the next point on the unit sphere as a
   * `std::array<double, 3>`. And in the `Sphere3Hopf` class, `pop()` returns
   * the next point on the 3-sphere using the Hopf fibration as a
   * `std::array<double, 4>`.
   *
   * @return std::array<double, 3>
   */
  inline auto pop() -> std::array<double, 3> {
    auto cosphi = 2.0 * this->vdcgen.pop() - 1.0; // map to [-1, 1];
    auto sinphi = std::sqrt(1.0 - cosphi * cosphi);
    auto arr = this->cirgen.pop();
    return {sinphi * arr[0], std::move(sinphi) * arr[1], std::move(cosphi)};
  }

  /**
   * @brief reseed
   *
   * The `reseed(size_t seed)` function is used to reset the state of the
   * sequence generator to a specific seed value. This allows the sequence
   * generator to start generating the sequence from the beginning, or from a
   * specific point in the sequence, depending on the value of the seed.
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void {
    this->cirgen.reseed(seed);
    this->vdcgen.reseed(seed);
  }
};

/**
 * @brief S(3) sequence generator by Hopf fibration
 *
 * The `Sphere3Hopf` class is a sequence generator that generates points on a
 * 3-sphere using the Hopf fibration. It uses three instances of the `VdCorput`
 * class to generate the sequence values and maps them to points on the
 * 3-sphere. The `pop()` method returns the next point on the 3-sphere as a
 * `std::array<double, 4>`, where the first three elements represent the x, y,
 * and z coordinates of the point, and the fourth element represents the w
 * coordinate. The `reseed()` method is used to reset the state of the sequence
 * generator to a specific seed value.
 */
class Sphere3Hopf {
  VdCorput vdc0;
  VdCorput vdc1;
  VdCorput vdc2;

public:
  /**
   * @brief Construct a new Sphere 3 Hopf object
   *
   * @param base0
   * @param base1
   * @param base2
   */
  Sphere3Hopf(const size_t base0, const size_t base1, const size_t base2)
      : vdc0(base0), vdc1(base1), vdc2(base2) {}

  /**
   * @brief pop
   *
   * The `pop()` function is used to generate the next value in the sequence. In
   * the `VdCorput` class, `pop()` increments the count and calculates the Van
   * der Corput sequence value for that count and base. In the `Halton` class,
   * `pop()` returns the next point in the Halton sequence as a
   * `std::array<double, 2>`. Similarly, in the `Circle` class, `pop()` returns
   * the next point on the unit circle as a `std::array<double, 2>`. In the
   * `Sphere` class, `pop()` returns the next point on the unit sphere as a
   * `std::array<double, 3>`. And in the `Sphere3Hopf` class, `pop()` returns
   * the next point on the 3-sphere using the Hopf fibration as a
   * `std::array<double, 4>`.
   *
   * @return std::array<double, 4>
   */
  inline auto pop() -> std::array<double, 4> {
    auto phi = this->vdc0.pop() * TWO_PI; // map to [0, 2*pi];
    auto psy = this->vdc1.pop() * TWO_PI; // map to [0, 2*pi];
    auto vd = this->vdc2.pop();
    auto cos_eta = std::sqrt(vd);
    auto sin_eta = std::sqrt(1.0 - std::move(vd));
    return {
        cos_eta * std::cos(psy),
        std::move(cos_eta) * std::sin(psy),
        sin_eta * std::cos(phi + psy),
        std::move(sin_eta) * std::sin(std::move(phi) + std::move(psy)),
    };
  }

  /**
   * @brief reseed
   *
   * The `reseed(size_t seed)` function is used to reset the state of the
   * sequence generator to a specific seed value. This allows the sequence
   * generator to start generating the sequence from the beginning, or from a
   * specific point in the sequence, depending on the value of the seed.
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void {
    this->vdc0.reseed(seed);
    this->vdc1.reseed(seed);
    this->vdc2.reseed(seed);
  }
};
} // namespace lds2
