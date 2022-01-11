#pragma once

#include <memory>  // import unqiue_ptr
#include <variant>
#include <vector>

#include "low_discr_seq.hpp"

namespace lds {

    /**
     * @brief Halton(n) sequence generator
     *
     */
    class halton_n {
      private:
        std::vector<vdcorput> _vec_vdc;

      public:
        /**
         * @brief Construct a new halton n object
         *
         * @param n
         * @param base
         */
        halton_n(gsl::span<const unsigned> base) {
            for (auto&& b : base) {
                this->_vec_vdc.emplace_back(vdcorput(b));
            }
        }

        /**
         * @brief
         *
         * @return auto
         */
        auto operator()() -> std::vector<double> {
            auto res = std::vector<double>{};
            for (auto& vdc : this->_vec_vdc) {
                res.emplace_back(vdc());
            }
            return res;
        }

        /**
         * @brief
         *
         * @param seed
         */
        auto reseed(unsigned seed) -> void {
            for (auto& vdc : this->_vec_vdc) {
                vdc.reseed(seed);
            }
        }
    };

    /** Generate Sphere-3 Halton sequence */
    class sphere3 {
      private:
        vdcorput _vdc;
        sphere _sphere2;

      public:
        /**
         * @brief Construct a new sphere3 object
         *
         * @param base
         */
        constexpr explicit sphere3(gsl::span<const unsigned> base) noexcept
            : _vdc(base[0]), _sphere2(base.subspan(1, 2)) {}

        /**
         * @brief
         *
         * @return std::vector<double>
         */
        auto operator()() -> std::tuple<double, double, double, double>;

        constexpr auto reseed(unsigned seed) noexcept -> void {
            this->_vdc.reseed(seed);
            this->_sphere2.reseed(seed);
        }
    };

    /**
     * @brief Circle sequence generator
     *
     */
    class cylin_2 {
      private:
        vdcorput _vdc;

      public:
        /**
         * @brief Construct a new circle object
         *
         * @param base
         */
        constexpr explicit cylin_2(unsigned base = 2) noexcept : _vdc(base) {}

        /**
         * @brief
         *
         * @return auto
         */
        auto operator()() -> std::vector<double> {
            const auto theta = this->_vdc() * twoPI;  // map to [0, 2*pi];
            return {std::sin(theta), std::cos(theta)};
        }

        /**
         * @brief
         *
         * @param seed
         */
        constexpr auto reseed(unsigned seed) noexcept -> void { this->_vdc.reseed(seed); }
    };

    /** Generate using cylindrical coordinate method */
    class cylin_n {
      private:
        vdcorput _vdc;
        std::variant<std::unique_ptr<cylin_n>, std::unique_ptr<cylin_2>> _Cgen;

      public:
        /**
         * @brief Construct a new cylin n object
         *
         * @param n dimension
         * @param base sequence base
         */
        cylin_n(gsl::span<const unsigned> base);

        /**
         * @brief
         *
         * @return std::vector<double>
         */
        auto operator()() -> std::vector<double>;
    };

    /**
     * @brief Sphere sequence generator
     *
     */
    class sphere_2 {
      private:
        vdcorput _vdc;
        circle _cirgen;

      public:
        /**
         * @brief Construct a new sphere object
         *
         * @param base
         */
        constexpr sphere_2(gsl::span<const unsigned> base) noexcept
            : _vdc(base[0]), _cirgen(base[1]) {}

        /**
         * @brief
         *
         * @return auto
         */
        auto operator()() -> std::vector<double> {
            const auto cosphi = 2 * this->_vdc() - 1;  // map to [-1, 1];
            const auto sinphi = std::sqrt(1 - cosphi * cosphi);
            auto [c, s] = this->_cirgen();
            return {sinphi * c, sinphi * s, cosphi};
        }

        /**
         * @brief
         *
         * @param seed
         */
        constexpr auto reseed(unsigned seed) noexcept -> void {
            this->_cirgen.reseed(seed);
            this->_vdc.reseed(seed);
        }
    };

    /** Generate Sphere-3 Halton sequence */
    class sphere_n {
      private:
        vdcorput _vdc;
        size_t _n;
        std::variant<std::unique_ptr<sphere_n>, std::unique_ptr<sphere_2>> _Sgen;
        double _range_t;
        double _t0;

      public:
        /**
         * @brief Construct a new sphere n object
         *
         * @param n dimension
         * @param base sequence base
         */
        sphere_n(gsl::span<const unsigned> base);

        /**
         * @brief
         *
         * @return std::vector<double>
         */
        auto operator()() -> std::vector<double>;
    };

}  // namespace lds
