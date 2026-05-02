#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <mutex>
// #include <numbers>
#include <span>
#include <vector>

#include "lds.hpp"

#ifndef M_PI
#    define M_PI 3.14159265358979323846264338327950288
#endif

namespace lds {

    constexpr double PI = M_PI;
    constexpr double HALF_PI = M_PI / 2.0;

    /**
     * @brief Generate evenly spaced numbers over a specified interval
     *
     * Simple implementation of numpy.linspace.
     *
     * @param[in] start the starting value of the sequence
     * @param[in] stop the end value of the sequence
     * @param[in] num number of samples to generate
     * @return std::vector<double> vector of evenly spaced numbers
     */
    std::vector<double> linspace(double start, double stop, std::size_t num);

    /**
     * @brief Perform one-dimensional linear interpolation
     *
     * Simple implementation of numpy.interp for 1D interpolation.
     *
     * @param[in] x_value the x-coordinate at which to evaluate the interpolated value
     * @param[in] x_points the x-coordinates of the data points
     * @param[in] y_points the y-coordinates of the data points
     * @return double the interpolated value at x_value
     */
    double simple_interp(double x_value, std::span<const double> x_points,
                         std::span<const double> y_points);

    /**
     * @brief Calculate the table-lookup of the mapping function for n
     *
     * Calculates the table-lookup of the mapping function for n.
     *
     * @param[in] n the dimension parameter
     * @return std::vector<double> the table-lookup values
     */
    std::vector<double> get_tp(int n);

    /**
     * @brief Base class for sphere generators
     *
     * Provides a common interface for all sphere sequence generators.
     */
    class SphereGen {
      public:
        virtual ~SphereGen() = default;

        /**
         * @brief Generate the next point on the sphere
         *
         * Returns the next point on the sphere as a vector of double values.
         *
         * @return std::vector<double> the next point on the sphere
         */
        virtual std::vector<double> pop() = 0;

        /**
         * @brief Reset the state of the sphere generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        virtual void reseed(std::uint64_t seed) = 0;
    };

    /**
     * @brief 3-Sphere sequence generator
     *
     * Generates points on a 3-sphere using a combination of van der Corput and Sphere generators.
     */
    class Sphere3 : public SphereGen {
      public:
        /**
         * @brief Construct a new Sphere3 object
         *
         * Constructs a 3-sphere sequence generator with the specified bases.
         *
         * @param[in] base span of uint64_t values representing the bases for the generators
         */
        explicit Sphere3(std::span<const std::uint64_t> base);

        /**
         * @brief Generate the next point on the 3-sphere
         *
         * Returns the next point on the 3-sphere as a vector of double values.
         *
         * @return std::vector<double> the next point on the 3-sphere
         */
        std::vector<double> pop() override;

        /**
         * @brief Reset the state of the Sphere3 generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        void reseed(std::uint64_t seed) override;

      private:
        VdCorput vdc_;
        Sphere sphere2_;
        mutable std::mutex mutex_;
    };

    /**
     * @brief Wrapper class to make Sphere compatible with SphereGen interface
     *
     * Provides a wrapper around the Sphere class to make it compatible with the SphereGen
     * interface.
     */
    class SphereWrapper : public SphereGen {
      public:
        /**
         * @brief Construct a new SphereWrapper object
         *
         * Constructs a Sphere wrapper with the specified bases.
         *
         * @param[in] base span of uint64_t values representing the bases for the Sphere generator
         */
        explicit SphereWrapper(std::span<const std::uint64_t> base);

        /**
         * @brief Generate the next point on the sphere
         *
         * Returns the next point on the sphere as a vector of double values.
         *
         * @return std::vector<double> the next point on the sphere
         */
        std::vector<double> pop() override;

        /**
         * @brief Reset the state of the SphereWrapper generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        void reseed(std::uint64_t seed) override;

      private:
        Sphere sphere_;
        mutable std::mutex mutex_;
    };

    /**
     * @brief N-dimensional sphere sequence generator
     *
     * Generates points on an N-dimensional sphere using a recursive approach.
     */
    class SphereN : public SphereGen {
      public:
        /**
         * @brief Construct a new SphereN object
         *
         * Constructs an N-dimensional sphere sequence generator with the specified bases.
         *
         * @param[in] base span of uint64_t values representing the bases for the generators
         */
        explicit SphereN(std::span<const std::uint64_t> base);

        /**
         * @brief Generate the next point on the N-dimensional sphere
         *
         * Returns the next point on the N-dimensional sphere as a vector of double values.
         *
         * @return std::vector<double> the next point on the N-dimensional sphere
         */
        std::vector<double> pop() override;

        /**
         * @brief Reset the state of the SphereN generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        void reseed(std::uint64_t seed) override;

      private:
        VdCorput vdc_;
        std::unique_ptr<SphereGen> s_gen_;
        int n_;
        double range_;
        mutable std::mutex mutex_;
    };
}  // namespace lds