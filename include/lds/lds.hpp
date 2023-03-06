#pragma once

#include <array>
#include <cmath>

namespace lds2 {

using std::array;
using std::cos;
using std::sin;
using std::sqrt;

constexpr const auto TWO_PI = 6.283185307179586;

#if __cpp_constexpr >= 201304
#define CONSTEXPR14 constexpr
#else
#define CONSTEXPR14 inline
#endif

/**
 * @brief Van der Corput sequence
 *
 * @param k
 * @param base
 * @return double
 */
CONSTEXPR14 auto vdc(size_t k, const size_t base) -> double {
  auto vdc = 0.0;
  auto denom = 1.0;
  while (k != 0) {
    const auto remainder = k % base;
    denom *= double(base);
    k /= base;
    vdc += double(remainder) / denom;
  }
  return vdc;
}

/**
 * @brief Van der Corput sequence generator
 *
 */
class Vdcorput {
  size_t count;
  size_t base;

public:
  /**
   * @brief Construct a new Vdcorput object
   *
   * @param base
   */
  CONSTEXPR14 explicit Vdcorput(size_t base) : count{0}, base{base} {}

  /**
   * @brief
   *
   * @return double
   */
  CONSTEXPR14 auto pop() -> double {
    this->count += 1;
    return vdc(this->count, this->base);
  }

  /**
   * @brief
   *
   * @param seed
   * @return auto
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void { this->count = seed; }
};

/**
 * @brief Halton sequence generator
 *
 */
class Halton {
  Vdcorput vdc0;
  Vdcorput vdc1;

public:
  /**
   * @brief Construct a new Halton object
   *
   * @param base
   */
  CONSTEXPR14 explicit Halton(const size_t base[])
      : vdc0(base[0]), vdc1(base[1]) {}

  /**
   * @brief
   *
   * @return array<double, 2>
   */
  CONSTEXPR14 auto pop() -> array<double, 2> { //
    return {this->vdc0.pop(), this->vdc1.pop()};
  }

  /**
   * @brief
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
 */
class Circle {
  Vdcorput vdc;

public:
  /**
   * @brief Construct a new Circle object
   *
   * @param base
   */
  CONSTEXPR14 explicit Circle(size_t base) : vdc(base) {}

  /**
   * @brief
   *
   * @return array<double, 2>
   */
  inline auto pop() -> array<double, 2> {
    const auto theta = this->vdc.pop() * TWO_PI; // map to [0, 2*pi];
    return {sin(theta), cos(theta)};
  }

  /**
   * @brief
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void { this->vdc.reseed(seed); }
};

/**
 * @brief Sphere sequence generator
 *
 */
class Sphere {
  Vdcorput vdcgen;
  Circle cirgen;

public:
  /**
   * @brief Construct a new Sphere object
   *
   * @param base
   */
  CONSTEXPR14 explicit Sphere(const size_t base[])
      : vdcgen(base[0]), cirgen(base[1]) {}

  /**
   * @brief
   *
   * @return array<double, 3>
   */
  inline auto pop() -> array<double, 3> {
    const auto cosphi = 2.0 * this->vdcgen.pop() - 1.0; // map to [-1, 1];
    const auto sinphi = sqrt(1.0 - cosphi * cosphi);
    const auto arr = this->cirgen.pop();
    return {sinphi * arr[0], sinphi * arr[1], cosphi};
  }

  /**
   * @brief
   *
   * @param seed
   */
  CONSTEXPR14 auto reseed(size_t seed) -> void {
    this->cirgen.reseed(seed);
    this->vdcgen.reseed(seed);
  }
};

/**
 * @brief S(3) sequence generator by Hopf
 *
 */
class Sphere3Hopf {
  Vdcorput vdc0;
  Vdcorput vdc1;
  Vdcorput vdc2;

public:
  /**
   * @brief Construct a new Sphere 3 Hopf object
   *
   * @param base
   */
  CONSTEXPR14 explicit Sphere3Hopf(const size_t base[])
      : vdc0(base[0]), vdc1(base[1]), vdc2(base[2]) {}

  /**
   * @brief
   *
   * @return array<double, 4>
   */
  inline auto pop() -> array<double, 4> {
    const auto phi = this->vdc0.pop() * TWO_PI; // map to [0, 2*pi];
    const auto psy = this->vdc1.pop() * TWO_PI; // map to [0, 2*pi];
    const auto vd = this->vdc2.pop();
    const auto cos_eta = sqrt(vd);
    const auto sin_eta = sqrt(1.0 - vd);
    return {
        cos_eta * cos(psy),
        cos_eta * sin(psy),
        sin_eta * cos(phi + psy),
        sin_eta * sin(phi + psy),
    };
  }

  /**
   * @brief
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
