#pragma once

/** @file lds.hpp
 *  @brief Low-discrepancy sequence generators (van der Corput, Halton, Circle, Disk, Sphere).
 */

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <numbers>

namespace lds {

    constexpr const auto TWO_PI = 2.0 * std::numbers::pi;

    // Constants for magic numbers
    constexpr unsigned long MAX_REVERSE_BITS = 64;
    constexpr double MAPPING_FACTOR = 2.0;

    namespace detail {

        /**
         * @brief Core base-b digit/weight summation shared by all van der Corput generators.
         *
         * Extracts the base-b digits of `n` (least significant first) and accumulates each
         * digit times its precomputed weight:
         * @f[
         *     \sum_k a_k(n) \cdot \mathrm{weights}[k], \qquad
         *     n = \sum_k a_k(n) \, b^k
         * @f]
         * The `weights` table determines the value type and scaling of the result: reverse
         * powers \f$b^{-k-1}\f$ for the floating-point generators, ascending integer powers
         * for the integer generators.
         *
         * @tparam T Accumulator/result type (e.g. double or unsigned long).
         * @tparam Table Random-access table type (e.g. std::array).
         * @param[in] n The sequence index to evaluate.
         * @param[in] base The numeric base.
         * @param[in] weights Precomputed digit weights.
         * @return The weighted digit sum for index `n`.
         */
        template <typename T, typename Table>
        constexpr auto vdc_digit_sum(unsigned long n, unsigned long base, const Table& weights)
            -> T {
            T reslt{};
            std::size_t idx = 0;
            while (n != 0) {
                const auto remainder = n % base;
                n /= base;
                reslt += static_cast<T>(remainder) * weights[idx];
                ++idx;
            }
            return reslt;
        }

    }  // namespace detail

    /**
     * @brief Forward iterator for sequence generators
     *
     * Provides STL-compatible iterator interface for all generators.
     * Allows use in range-based for loops and STL algorithms.
     *
     * @verbatim
     * VdCorput gen(2);
     * std::vector<double> points(gen.begin(), gen.begin() + 100);
     * @endverbatim
     *
     * @tparam Generator The generator class
     * @tparam Value The value type (double or array)
     *
     * @note Iterator pattern: decouples traversal (STL algorithms, range-for) from
     * the stateful generator "container"; dereference generates a value on demand
     * from the underlying generator. The `gen` pointer plus the `index` cursor
     * realize the iteration state, and the class provides the full iterator
     * protocol (iterator_category, value_type, operator*, operator++, comparisons)
     * so generators can be consumed with std::begin/std::end and range-for.
     */
    template <typename Generator, typename Value> class GeneratorIterator {
        Generator* gen;
        unsigned long index;

      public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = value_type;

        explicit GeneratorIterator(Generator* g = nullptr, unsigned long idx = 0)
            : gen{g}, index{idx} {}

        /**
         * @brief Dereference operator
         */
        auto operator*() const -> Value {
            if (gen) {
                auto temp_idx = gen->get_index();
                gen->reseed(index);
                auto value = gen->pop();
                gen->reseed(temp_idx);
                return value;
            }
            return Value{};
        }

        /**
         * @brief Pre-increment operator
         */
        auto operator++() -> GeneratorIterator& {
            ++index;
            return *this;
        }

        /**
         * @brief Post-increment operator
         */
        auto operator++(int) -> GeneratorIterator {
            auto temp = *this;
            ++index;
            return temp;
        }

        /**
         * @brief Equality comparison
         */
        auto operator==(const GeneratorIterator& other) const -> bool {
            return index == other.index;
        }

        /**
         * @brief Inequality comparison
         */
        auto operator!=(const GeneratorIterator& other) const -> bool {
            return index != other.index;
        }

        /**
         * @brief Get current index
         */
        [[nodiscard]] auto get_index() const -> unsigned long { return index; }
    };

    /**
     * @brief Concept for the sequence generator protocol
     *
     * Requires the uniform strategy interface shared by every generator: pop/peek/skip/
     * reseed/get_index. Both the compile-time template generators (VdCorput<Base>, ...)
     * and the runtime-polymorphic ones (VdCorputBase, HaltonN) satisfy it, so generic code
     * can be written against either family interchangeably.
     *
     * @tparam G The candidate generator type.
     * @tparam V The value type it produces.
     */
    template <typename G, typename V>
    concept SequenceGenerator = requires(G g, unsigned long n) {
        { g.pop() } -> std::convertible_to<V>;
        { g.peek() } -> std::convertible_to<V>;
        g.skip(n);
        g.reseed(n);
        { g.get_index() } -> std::convertible_to<unsigned long>;
    };

    /**
     * @brief CRTP base implementing the sequence generator protocol
     *
     * Implements the shared stateful protocol (pop/peek/skip/reseed/get_index) in terms of
     * a single pure computation `value_at(n)` supplied by the derived class.
     *
     * @note Template Method pattern via CRTP: the base fixes the protocol skeleton —
     * `pop()` evaluates `value_at(++count_)` and `peek()` evaluates `value_at(count_ + 1)` —
     * while the derived class supplies only the pure index-to-value computation. This
     * guarantees pop/peek consistency by construction, centralizes the sequence state in
     * one counter, and removes the duplicated protocol from every concrete generator.
     *
     * @tparam Derived The CRTP-derived generator class.
     * @tparam Value The value type produced by pop()/peek().
     */
    template <typename Derived, typename Value> class GeneratorBase {
      protected:
        GeneratorBase() = default;
        friend Derived;

      public:
        /**
         * @brief Generate the next value in the sequence (advances state).
         *
         * @return The value at the incremented sequence index.
         */
        constexpr auto pop() -> Value { return derived().value_at(++this->count_); }

        /**
         * @brief Peek at the next value without advancing state.
         *
         * @return The value at the next sequence index.
         */
        [[nodiscard]] constexpr auto peek() -> Value {
            return derived().value_at(this->count_ + 1);
        }

        /**
         * @brief Skip n values in the sequence.
         *
         * @param[in] n number of values to skip
         */
        constexpr auto skip(unsigned long n) -> void { this->count_ += n; }

        /**
         * @brief Reset the generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        constexpr auto reseed(const unsigned long& seed) -> void { this->count_ = seed; }

        /**
         * @brief Get current index in the sequence.
         *
         * @return unsigned long current index in the sequence
         */
        [[nodiscard]] constexpr auto get_index() const -> unsigned long { return this->count_; }

      protected:
        unsigned long count_{0};  ///< Current sequence index (single source of state)

      private:
        constexpr auto derived() -> Derived& { return static_cast<Derived&>(*this); }
    };

    /**
     * @brief CRTP mixin adding STL iterator support to a GeneratorBase.
     *
     * Provides begin()/end() so generators can be consumed with range-for, std::begin/
     * std::end and STL algorithms. Combined with GeneratorIterator, this realizes the
     * Iterator pattern for every generator without repeating the iterator boilerplate.
     *
     * @tparam Derived The CRTP-derived generator class.
     * @tparam Value The value type produced by the generator.
     */
    template <typename Derived, typename Value> class GeneratorIterable
        : public GeneratorBase<Derived, Value> {
      protected:
        GeneratorIterable() = default;
        friend Derived;

      public:
        /**
         * @brief Get iterator to beginning
         *
         * @return GeneratorIterator<Derived, Value>
         */
        constexpr auto begin() -> GeneratorIterator<Derived, Value> {
            return GeneratorIterator<Derived, Value>(static_cast<Derived*>(this));
        }

        /**
         * @brief Get iterator to end (infinite sequence)
         *
         * For infinite sequences, you typically use begin() + n to get a specific position
         *
         * @return GeneratorIterator<Derived, Value>
         */
        [[nodiscard]] constexpr auto end() const -> GeneratorIterator<Derived, Value> {
            return GeneratorIterator<Derived, Value>(nullptr,
                                                     std::numeric_limits<unsigned long>::max());
        }
    };

    /**
     * @brief van der Corput sequence
     *
     * The `vdc` function is calculating the van der Corput sequence value for a
     * given index \f$n\f$ and base \f$b\f$. It returns a `double` value.
     *
     * The sequence is defined as:
     * @f[
     *     \phi_b(n) = \sum_{k=0}^{\infty} \frac{a_k}{b^{k+1}}
     * @f]
     * where \f$a_k\f$ are the base-\f$b\f$ digits of \f$n\f$:
     * @f[
     *     n = \sum_{k=0}^{\infty} a_k \, b^k
     * @f]
     *
     * @verbatim
     *     Base 2 Example:
     *     count=1 -> 0.5  (0.1 in base 2)
     *     count=2 -> 0.25 (0.01 in base 2)
     *     count=3 -> 0.75 (0.11 in base 2)
     *     count=4 -> 0.125(0.001 in base 2)
     * @endverbatim
     *
     * @param[in] cnt index of the sequence
     * @tparam Base base of the sequence
     * @return double
     */
    template <unsigned long Base = 2> constexpr auto vdc(unsigned long cnt) -> double {
        auto reslt = 0.0;
        auto denom = 1.0;
        auto count = cnt;
        while (count != 0) {
            const auto remainder = count % Base;
            count /= Base;
            denom *= static_cast<double>(Base);
            reslt += static_cast<double>(remainder) / denom;
        }
        return reslt;
    }

    /**
     * @brief van der Corput sequence generator
     *
     * `VdCorput` is a class that generates the van der Corput sequence. The van der
     * Corput sequence is a low-discrepancy sequence that is commonly used in
     * quasi-Monte Carlo methods. The sequence is generated by iterating over a base
     * and calculating the fractional part of the number in that base. The
     * `VdCorput` class keeps track of the current count and base, and provides a
     * `pop()` method that returns the next value in the sequence.
     *
     * For a given base \f$b\f$, the \f$n\f$-th value is:
     * @f[
     *     \phi_b(n) = \sum_{k=0}^{\infty} a_k(n) \, b^{-k-1}
     * @f]
     * where \f$a_k(n)\f$ are the base-\f$b\f$ digits of \f$n\f$.
     *
     * @dot
     *   digraph vdc_flow {
     *     rankdir=LR;
     *     bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     n [label="Input n\n(index)", fillcolor="#a9cce3"];
     *     base [label="Base b", fillcolor="#a9cce3"];
     *     digits [label="Extract base-b\ndigits a_k"];
     *     ratio [label="Compute\nratio a_k / b^{k+1}"];
     *     sum [label="Sum\nphi_b(n)", fillcolor="#7fb3d8"];
     *     n -> digits;
     *     base -> digits;
     *     digits -> ratio;
     *     ratio -> sum;
     *   }
     * @enddot
     *
     * @verbatim
     *     VdCorput(2) sequence:
     *     pop() -> 0.5   (0.1 base 2)
     *     pop() -> 0.25  (0.01 base 2)
     *     pop() -> 0.75  (0.11 base 2)
     *     pop() -> 0.125 (0.001 base 2)
     *     ...
     * @endverbatim
     */
    template <unsigned long Base = 2> class VdCorput
        : public GeneratorIterable<VdCorput<Base>, double> {
        std::array<double, MAX_REVERSE_BITS> rev_lst{};

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * Constructs a VdCorput sequence generator using the template parameter
         * `Base` to generate the van der Corput sequence.
         *
         * Precomputes reverse powers of Base for fast lookup in value_at(),
         * avoiding repeated division at the cost of 512 bytes per instance.
         *
         * @tparam Base the base of the van der Corput sequence
         */
        constexpr VdCorput() {
            double reverse = 1.0;
            for (unsigned long i = 0; i < MAX_REVERSE_BITS; ++i) {
                reverse /= static_cast<double>(Base);
                this->rev_lst[i] = reverse;
            }
        }

        /**
         * @brief Evaluate the sequence value at a given index (pure, no state change)
         *
         * Computes the van der Corput value for index \f$n\f$:
         * @f[
         *     \phi_b(n) = \sum_{k=0}^{\infty} a_k(n) \, b^{-k-1}
         * @f]
         * where \f$a_k(n)\f$ are the base-\f$b\f$ digits of \f$n\f$.
         *
         * @param[in] n The sequence index.
         * @return The van der Corput value for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> double {
            return detail::vdc_digit_sum<double>(n, Base, this->rev_lst);
        }
    };

    /**
     * @brief Circle sequence generator
     *
     * The `Circle` class is a sequence generator that generates points on a unit
     * circle using the van der Corput sequence. It uses the `VdCorput` class to
     * generate the sequence values and maps them to points on the unit circle. The
     * `pop()` method returns the next point on the unit circle as a
     * `std::array<double, 2>`, where the first element represents the x-coordinate
     * and the second element represents the y-coordinate of the point. The
     * `reseed()` method is used to reset the state of the sequence generator to a
     * specific seed value.
     *
     * Points are generated by mapping the van der Corput sequence to the angle:
     * @f[
     *     \theta = 2\pi \cdot \phi_b(n), \qquad
     *     P(n) = \bigl(\cos\theta,\; \sin\theta\bigr)
     * @f]
     *
     * @verbatim
     *     Unit Circle:
     *         (0,1)
     *           *
     *    (-1,0) *   * (1,0)
     *           *
     *        (0,-1)
     *
     *     Points distributed more evenly
     *     than random sampling
     * @endverbatim
     */
    template <unsigned long Base = 2> class Circle
        : public GeneratorIterable<Circle<Base>, std::array<double, 2>> {
        VdCorput<Base> vdc;

      public:
        /**
         * @brief Construct a new Circle object
         *
         * Constructs a Circle sequence generator with the specified base for generating
         * the van der Corput sequence, which is then mapped to points on the unit circle.
         *
         * @tparam Base the base for the van der Corput sequence generator
         */
        constexpr Circle() : vdc() {}

        /**
         * @brief Evaluate the point on the unit circle at a given index (pure)
         *
         * Maps the van der Corput value to the angle:
         * @f[
         *     \theta = 2\pi \cdot \phi_b(n), \qquad
         *     P(n) = \bigl(\cos\theta,\; \sin\theta\bigr)
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The point on the unit circle for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> std::array<double, 2> {
            auto theta = this->vdc.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            return {std::cos(theta), std::sin(theta)};
        }
    };

    /**
     * @brief Halton sequence generator
     *
     * The `Halton` class is a sequence generator that generates points in a
     * 2-dimensional space using the Halton sequence. The Halton sequence is a
     * low-discrepancy sequence that is commonly used in quasi-Monte Carlo methods.
     * It is generated by iterating over two different bases and calculating the
     * fractional parts of the numbers in those bases. The `Halton` class keeps
     * track of the current count and bases, and provides a `pop()` method that
     * returns the next point in the sequence as a `std::array<double, 2>`.
     *
     * The Halton sequence in \f$d\f$ dimensions is defined as:
     * @f[
     *     H_b(n) = \bigl(\phi_{b_1}(n), \phi_{b_2}(n), \dots, \phi_{b_d}(n)\bigr)
     * @f]
     * where \f$\phi_{b_i}\f$ is the van der Corput sequence in base \f$b_i\f$
     * and the bases \f$b_1, b_2, \dots, b_d\f$ are pairwise coprime.
     *
     * @verbatim
     *     Halton(2,3) sequence:
     *     pop() -> (0.5, 0.333)  (VdC(2) -> 0.5, VdC(3) -> 0.333)
     *     pop() -> (0.25, 0.666) (VdC(2) -> 0.25, VdC(3) -> 0.666)
     *     pop() -> (0.75, 0.111) (VdC(2) -> 0.75, VdC(3) -> 0.111)
     *     ...
     * @endverbatim
     */
    template <unsigned long Base0 = 2, unsigned long Base1 = 3> class Halton
        : public GeneratorIterable<Halton<Base0, Base1>, std::array<double, 2>> {
        VdCorput<Base0> vdc0;
        VdCorput<Base1> vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * Constructs a Halton sequence generator with the specified bases for the
         * two dimensions.
         */
        constexpr Halton() : vdc0(), vdc1() {}

        /**
         * @brief Evaluate the 2D Halton point at a given index (pure)
         *
         * @f[
         *     H(n) = (\phi_{b_0}(n), \phi_{b_1}(n))
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The 2D Halton point for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> std::array<double, 2> {
            return {this->vdc0.value_at(n), this->vdc1.value_at(n)};
        }
    };

    /**
     * @brief Disk sequence generator
     *
     * The `Disk` class is a sequence generator that generates points in a
     * 2-dimensional space using the Disk sequence. The Disk sequence is a
     * low-discrepancy sequence that is commonly used in quasi-Monte Carlo methods.
     * It is generated by iterating over two different bases and calculating the
     * fractional parts of the numbers in those bases. The `Disk` class keeps
     * track of the current count and bases, and provides a `pop()` method that
     * returns the next point in the sequence as a `std::array<double, 2>`.
     *
     * Points are sampled uniformly within the unit disk using polar mapping:
     * @f[
     *     \theta = 2\pi \cdot \phi_{b_0}(n), \qquad
     *     r = \sqrt{\phi_{b_1}(n)}
     * @f]
     * @f[
     *     P(n) = \bigl(r\cos\theta,\; r\sin\theta\bigr)
     * @f]
     *
     * @verbatim
     *     Unit Disk:
     *         *****
     *      ***     ***
     *    **         **
     *   *             *
     *   *             *  More evenly
     *   *             *  distributed
     *    **         **   than random
     *      ***     ***
     *         *****
     * @endverbatim
     */
    template <unsigned long Base0 = 2, unsigned long Base1 = 3> class Disk
        : public GeneratorIterable<Disk<Base0, Base1>, std::array<double, 2>> {
        VdCorput<Base0> vdc0;
        VdCorput<Base1> vdc1;

      public:
        /**
         * @brief Construct a new Disk object
         *
         * Constructs a Disk sequence generator with the specified bases for the two
         * dimensions.
         *
         * @tparam Base0 the base for the first dimension (angle)
         * @tparam Base1 the base for the second dimension (radius)
         */
        constexpr Disk() : vdc0(), vdc1() {}

        /**
         * @brief Evaluate the point in the unit disk at a given index (pure)
         *
         * Samples uniformly within the unit disk using polar mapping:
         * @f[
         *     \theta = 2\pi \cdot \phi_{b_0}(n), \qquad
         *     r = \sqrt{\phi_{b_1}(n)}
         * @f]
         * @f[
         *     P(n) = \bigl(r\cos\theta,\; r\sin\theta\bigr)
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The point in the unit disk for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> std::array<double, 2> {
            auto theta = this->vdc0.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto radius = std::sqrt(this->vdc1.value_at(n));
            return {radius * std::cos(theta), radius * std::sin(theta)};
        }
    };

    /**
     * @brief Sphere sequence generator
     *
     * The `Sphere` class is a sequence generator that generates points on a unit
     * sphere using the van der Corput sequence. It uses the `VdCorput` class to
     * generate the sequence values and maps them to points on the unit sphere. The
     * `pop()` method returns the next point on the unit sphere as a
     * `std::array<double, 3>`, where the first element represents the x-coordinate,
     * the second element represents the y-coordinate of the point, and the third
     * element represents the z-coordinate of the point. The
     * `reseed()` method is used to reset the state of the sequence generator to a
     * specific seed value.
     *
     * Points are distributed on the sphere using cylindrical mapping:
     * @f[
     *     \phi = 2\pi \cdot \phi_{b_1}(n), \qquad
     *     \cos\theta = 2\phi_{b_0}(n) - 1
     * @f]
     * @f[
     *     P(n) = \bigl(
     *         \sin\theta\cos\phi,\;
     *         \sin\theta\sin\phi,\;
     *         \cos\theta
     *     \bigr)
     * @f]
     *
     * @verbatim
     *     Unit Sphere:
     *          *****
     *       **       **
     *     **           **
     *    *               *
     *    *      O        *  Points distributed
     *    *               *  evenly on surface
     *     **           **
     *       **       **
     *          *****
     * @endverbatim
     *
     * @tparam Base0 the base for the van der Corput generator (phi coordinate)
     * @tparam Base1 the base for the Circle generator (theta coordinate)
     */
    template <unsigned long Base0 = 2, unsigned long Base1 = 3> class Sphere
        : public GeneratorIterable<Sphere<Base0, Base1>, std::array<double, 3>> {
        VdCorput<Base0> vdcgen;
        Circle<Base1> cirgen;

      public:
        /**
         * @brief Construct a new Sphere object
         *
         * Constructs a Sphere sequence generator with the specified bases for generating
         * points on the unit sphere.
         */
        constexpr Sphere() : vdcgen(), cirgen() {}

        /**
         * @brief Evaluate the point on the unit sphere at a given index (pure)
         *
         * Distributes points on the sphere using cylindrical mapping:
         * @f[
         *     \phi = 2\pi \cdot \phi_{b_1}(n), \qquad
         *     \cos\theta = 2\phi_{b_0}(n) - 1
         * @f]
         * @f[
         *     P(n) = \bigl(
         *         \sin\theta\cos\phi,\;
         *         \sin\theta\sin\phi,\;
         *         \cos\theta
         *     \bigr)
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The point on the unit sphere for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> std::array<double, 3> {
            auto cosphi = (MAPPING_FACTOR * this->vdcgen.value_at(n)) - 1.0;  // map to [-1, 1];
            auto sinphi = std::sqrt(1.0 - (cosphi * cosphi));
            auto arr = this->cirgen.value_at(n);
            return {sinphi * arr[0], sinphi * arr[1], cosphi};
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
     *
     * The Hopf fibration parametrizes \f$S^3\f$ using angles \f$\phi, \psi, \eta\f$:
     *
     * @dot
     *   digraph hopf_fibration {
     *     rankdir=LR;
     *     bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d5f5e3"];
     *     vdc0 [label="phi\n= 2pi * VdC(b0)", fillcolor="#a9dfbf"];
     *     vdc1 [label="psi\n= 2pi * VdC(b1)", fillcolor="#a9dfbf"];
     *     vdc2 [label="eta\n= arccos(sqrt(VdC(b2)))", fillcolor="#a9dfbf"];
     *     s3 [label="S^3 point\n(x,y,z,w)", fillcolor="#7dcea0"];
     *     vdc0 -> s3 [label="phi + psi"];
     *     vdc1 -> s3;
     *     vdc2 -> s3;
     *   }
     * @enddot
     * @f[
     *     \begin{aligned}
     *     x &= \cos\eta \cos\psi \\
     *     y &= \cos\eta \sin\psi \\
     *     z &= \sin\eta \cos(\phi + \psi) \\
     *     w &= \sin\eta \sin(\phi + \psi)
     *     \end{aligned}
     * @f]
     * where \f$\phi = 2\pi\phi_{b_0}(n)\f$, \f$\psi = 2\pi\phi_{b_1}(n)\f$,
     * and \f$\eta = \arccos\sqrt{\phi_{b_2}(n)}\f$.
     *
     * @verbatim
     *     3-Sphere (S3) visualization:
     *     A 4D hypersphere where points (x,y,z,w)
     *     satisfy x²+y²+z²+w² = 1
     *
     *         4D hypersurface
     *            _____
     *         .-'     '-.
     *       ,'           ',
     *      /               \
     *     |        O        |  (4D analog of sphere)
     *      \               /
     *       '.           .'
     *         '-.....-'
     * @endverbatim
     *
     * @tparam Base0 the base for the first van der Corput generator (phi coordinate)
     * @tparam Base1 the base for the second van der Corput generator (psi coordinate)
     * @tparam Base2 the base for the third van der Corput generator (eta coordinate)
     */
    template <unsigned long Base0 = 2, unsigned long Base1 = 3, unsigned long Base2 = 5>
    class Sphere3Hopf
        : public GeneratorIterable<Sphere3Hopf<Base0, Base1, Base2>, std::array<double, 4>> {
        VdCorput<Base0> vdc0;
        VdCorput<Base1> vdc1;
        VdCorput<Base2> vdc2;

      public:
        /**
         * @brief Construct a new Sphere 3 Hopf object
         *
         * Constructs a 3-sphere sequence generator using the Hopf fibration with the
         * specified bases.
         */
        constexpr Sphere3Hopf() : vdc0(), vdc1(), vdc2() {}

        /**
         * @brief Evaluate the point on the 3-sphere at a given index (pure)
         *
         * Uses the Hopf fibration parametrization:
         * @f[
         *     \begin{aligned}
         *     x &= \cos\eta \cos\psi \\
         *     y &= \cos\eta \sin\psi \\
         *     z &= \sin\eta \cos(\phi + \psi) \\
         *     w &= \sin\eta \sin(\phi + \psi)
         *     \end{aligned}
         * @f]
         * where \f$\phi = 2\pi\phi_{b_0}(n)\f$, \f$\psi = 2\pi\phi_{b_1}(n)\f$,
         * and \f$\eta = \arccos\sqrt{\phi_{b_2}(n)}\f$.
         *
         * @param[in] n The sequence index.
         * @return The point on the 3-sphere for index n.
         */
        [[nodiscard]] constexpr auto value_at(unsigned long n) const -> std::array<double, 4> {
            auto phi = this->vdc0.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto psy = this->vdc1.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto vdc = this->vdc2.value_at(n);
            auto cos_eta = std::sqrt(vdc);
            auto sin_eta = std::sqrt(1.0 - vdc);
            return {
                cos_eta * std::cos(psy),
                cos_eta * std::sin(psy),
                sin_eta * std::cos(phi + psy),
                sin_eta * std::sin(phi + psy),
            };
        }
    };

    // Compile-time contract checks: every template generator satisfies the protocol concept.
    static_assert(SequenceGenerator<VdCorput<2>, double>);
    static_assert(SequenceGenerator<Circle<2>, std::array<double, 2>>);
    static_assert(SequenceGenerator<Halton<2, 3>, std::array<double, 2>>);
    static_assert(SequenceGenerator<Disk<2, 3>, std::array<double, 2>>);
    static_assert(SequenceGenerator<Sphere<2, 3>, std::array<double, 3>>);
    static_assert(SequenceGenerator<Sphere3Hopf<2, 3, 5>, std::array<double, 4>>);

    /**
     * @brief Look up the n-th prime number from a precomputed table.
     * @param[in] index Zero-based index into the prime table.
     * @return The prime number at the given index.
     */
    extern unsigned long prime_table(unsigned long index);

    /**
     * @brief Look up a precomputed van der Corput value for base 2.
     * @param[in] index The sequence index.
     * @return The van der Corput value for base 2 at the given index.
     */
    extern double vdc2_table(unsigned long index);

    /**
     * @brief Look up a precomputed Circle x-coordinate for base 2.
     * @param[in] index The sequence index.
     * @return The x-coordinate on the unit circle.
     */
    extern double circle2_table_x(unsigned long index);

    /**
     * @brief Look up a precomputed Circle y-coordinate for base 2.
     * @param[in] index The sequence index.
     * @return The y-coordinate on the unit circle.
     */
    extern double circle2_table_y(unsigned long index);
}  // namespace lds
