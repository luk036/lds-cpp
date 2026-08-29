#pragma once

/** @file ilds.hpp
 *  @brief Integer low-discrepancy sequence generators (van der Corput, Halton).
 */

#include <array>
#include <lds/lds.hpp>

namespace ilds {

    using std::array;

    // Constants for magic numbers
    /**
     * @brief Default number of digits for the van der Corput sequence
     *
     * This value determines the precision/scale of the integer Halton sequence.
     * Default is 10 digits.
     */
    constexpr unsigned int DEFAULT_SCALE = 10;
    constexpr unsigned int MAX_REVERSE_BITS = 64;

    /**
     * @brief van der Corput sequence generator
     *
     * Implementation based on pre-calculating the scale factor.
     *
     * @dot
     *   digraph ilds_flow {
     *     rankdir=LR;
     *     bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     input [label="Integer n", fillcolor="#a9cce3"];
     *     base [label="Base b"];
     *     digits [label="Extract base-b\ndigits"];
     *     factor [label="Multiply by\nprecomputed factor"];
     *     sum [label="Sum results\nphi_b(n)", fillcolor="#7fb3d8"];
     *     input -> digits;
     *     base -> digits;
     *     digits -> factor -> sum;
     *   }
     * @enddot
     *
     */
    template <unsigned long Base = 2> class VdCorput
        : public lds::GeneratorBase<VdCorput<Base>, unsigned long> {
        std::array<unsigned long, MAX_REVERSE_BITS>
            factor_lst{};  ///< Precomputed scale factors for each digit
        static_assert(MAX_REVERSE_BITS >= sizeof(unsigned long) * 8,
                      "MAX_REVERSE_BITS must be at least the number of bits in unsigned long");

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * @param[in] scale The number of digits (default: 10)
         */
        constexpr explicit VdCorput(unsigned int scale = DEFAULT_SCALE) {
            unsigned long factor = 1;
            unsigned int n = scale < MAX_REVERSE_BITS ? scale : MAX_REVERSE_BITS;
            for (unsigned int i = 0; i < n; ++i) {
                factor_lst[n - 1 - i] = factor;
                factor *= Base;
            }
        }

        /**
         * @brief Evaluate the integer sequence value at a given index (pure, no state change)
         *
         * @f[
         *     \phi_b^{\mathbb{Z}}(n) = \sum_{k=0}^{\infty} a_k(n) \cdot \mathrm{factor}_k
         * @f]
         * where \f$\mathrm{factor}_k = b^{\mathrm{scale}-1-k}\f$ scales the reversed
         * base-b digits into an integer.
         *
         * @param[in] n The sequence index.
         * @return The integer van der Corput value for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> unsigned long {
            return lds::detail::vdc_digit_sum<unsigned long>(n, Base, this->factor_lst);
        }
    };

    /**
     * @brief Halton sequence generator
     *
     * @verbatim
     *     Integer Halton([2,3], [2,2]):
     *     pop() -> [1, 4]   (VdC_i(2,2,2), VdC_i(2,3,2))
     *     pop() -> [2, 5]   (next in each sequence)
     *     ...
     * @endverbatim
     */
    template <unsigned long Base1, unsigned long Base2> class Halton
        : public lds::GeneratorBase<Halton<Base1, Base2>, array<unsigned long, 2>> {
        VdCorput<Base1> vdc0;
        VdCorput<Base2> vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * Constructs a Halton sequence generator with the specified bases and scale values.
         *
         * @param[in] scale array of two unsigned int values representing the number of digits for
         * each generator
         */
        constexpr explicit Halton(const std::array<unsigned int, 2>& scale)
            : vdc0(scale[0]), vdc1(scale[1]) {}

        /**
         * @brief Evaluate the integer 2D Halton point at a given index (pure)
         *
         * @f[
         *     H(n) = (\phi_{b_1}^{\mathbb{Z}}(n), \phi_{b_2}^{\mathbb{Z}}(n))
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The integer 2D Halton point for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> array<unsigned long, 2> {
            return {this->vdc0.value_at(n), this->vdc1.value_at(n)};
        }
    };

}  // namespace ilds
