#pragma once

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>

namespace ilds {

    using std::array;

    // Constants for magic numbers
    /**
     * @brief Default number of digits for the Van der Corput sequence
     *
     * This value determines the precision/scale of the integer Halton sequence.
     * Default is 10 digits.
     */
    constexpr unsigned int DEFAULT_SCALE = 10;

    /**
     * @brief Van der Corput sequence generator
     *
     * Implementation based on pre-calculating the scale factor.
     *
     */
    template <size_t Base = 2>
    class VdCorput {
        std::atomic<size_t> _count;  ///< Current count in the sequence
        size_t _factor;              ///< Precomputed scale factor (base^scale)

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * @param[in] scale The number of digits (default: 10)
         */
        explicit VdCorput(unsigned int scale = DEFAULT_SCALE)
            : _count{0}, _factor{static_cast<size_t>(std::pow(Base, scale))} {}

        /**
         * @brief Increments count and calculates the next value in the sequence.
         *
         * @return size_t
         */
        [[nodiscard]] auto pop() -> size_t {
            this->_count.fetch_add(1, std::memory_order_relaxed);

            size_t count = this->_count.load(std::memory_order_relaxed);
            size_t reslt = 0;
            size_t factor = this->_factor;

            while (count != 0) {
                const size_t remainder = count % Base;
                factor /= Base;
                count /= Base;
                reslt += remainder * factor;
            }
            return reslt;
        }

        /**
         * @brief Resets the state of the sequence generator.
         *
         * @param[in] seed
         */
        auto reseed(const size_t seed) -> void {
            this->_count.store(seed, std::memory_order_relaxed);
        }

        VdCorput(VdCorput&&) noexcept = delete;
        VdCorput& operator=(VdCorput&&) noexcept = delete;
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
    template <size_t Base1, size_t Base2>
    class Halton {
        VdCorput<Base1> vdc0;
        VdCorput<Base2> vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * Constructs a Halton sequence generator with the specified bases and scale values.
         *
         * @param[in] base array of two size_t values representing the bases for the two Van der
         * Corput generators
         * @param[in] scale array of two unsigned int values representing the number of digits for
         * each generator
         */
        Halton(const std::array<unsigned int, 2>& scale)
            : vdc0(scale[0]), vdc1(scale[1]) {}

        /**
         * @brief Generate the next point in the Halton sequence
         *
         * Returns the next point in the Halton sequence as an array of two size_t values.
         *
         * @return array<size_t, 2> the next point in the sequence
         */
        inline auto pop() -> array<size_t, 2> {  //
            return {this->vdc0.pop(), this->vdc1.pop()};
        }

        /**
         * @brief Reset the state of the Halton sequence generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        auto reseed(const size_t seed) -> void {
            this->vdc0.reseed(seed);
            this->vdc1.reseed(seed);
        }
    };

}  // namespace ilds
