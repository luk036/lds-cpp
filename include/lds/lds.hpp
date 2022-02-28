#pragma once

#include <array>
#include <cmath>
#include <gsl/span>

namespace lds2 {

    using gsl::span;
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;

    static const auto TWO_PI = 2.0 * std::acos(-1.0);

    inline auto vdc(size_t k, const size_t base) -> double {
        auto vdc = 0.0;
        auto denom = 1.0;
        while (k != 0) {
            denom *= base;
            const auto remainder = k % base;
            k /= base;
            vdc += remainder / denom;
        }
        return vdc;
    }

    class Vdcorput {
        size_t count;
        size_t base;

      public:
        explicit Vdcorput(size_t base) : count{0}, base{base} {}

        auto pop() -> double {
            this->count += 1;
            return vdc(this->count, this->base);
        }

        auto reseed(size_t seed) { this->count = seed; }
    };

    /**
     * @brief Halton sequence generator
     *
     */
    class Halton {
        Vdcorput vdc0;
        Vdcorput vdc1;

      public:
        explicit Halton(span<const size_t> base) : vdc0(base[0]), vdc1(base[1]) {}

        auto pop() -> array<double, 2> {  //
            return {this->vdc0.pop(), this->vdc1.pop()};
        }

        /**
         * @brief
         *
         * @param seed
         */
        auto reseed(size_t seed) {
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
        explicit Circle(size_t base) : vdc(base) {}

        auto pop() -> array<double, 2> {
            const auto theta = this->vdc.pop() * TWO_PI;  // map to [0, 2*pi];
            return {sin(theta), cos(theta)};
        }

        auto reseed(size_t seed) { this->vdc.reseed(seed); }
    };

    /**
     * @brief Sphere sequence generator
     *
     */
    class Sphere {
        Vdcorput vdcgen;
        Circle cirgen;

      public:
        explicit Sphere(span<const size_t> base) : vdcgen(base[0]), cirgen(base[1]) {}

        auto pop() -> array<double, 3> {
            const auto cosphi = 2.0 * this->vdcgen.pop() - 1.0;  // map to [-1, 1];
            const auto sinphi = sqrt(1.0 - cosphi * cosphi);
            const auto [c, s] = this->cirgen.pop();
            return {sinphi * c, sinphi * s, cosphi};
        }

        auto reseed(size_t seed) {
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
        explicit Sphere3Hopf(span<const size_t> base)
            : vdc0(base[0]), vdc1(base[1]), vdc2(base[2]) {}

        auto pop() -> array<double, 4> {
            const auto phi = this->vdc0.pop() * TWO_PI;  // map to [0, 2*pi];
            const auto psy = this->vdc1.pop() * TWO_PI;  // map to [0, 2*pi];
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

        auto reseed(size_t seed) {
            this->vdc0.reseed(seed);
            this->vdc1.reseed(seed);
            this->vdc2.reseed(seed);
        }
    };
}  // namespace lds2
